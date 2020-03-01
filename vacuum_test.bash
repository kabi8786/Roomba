#!/bin/bash

test_num=0
REPORT_FILE=test_report.txt

echo "Remember to do these two commands before running this script:
export COLUMNS
export LINES
"
echo "Test run starting at $(date)" >>${REPORT_FILE}

#==============================================================================
#	Operations available in the vacuum simulation.
#==============================================================================
function pause_resume() {
	printf "p "
}

function reset() {
	printf "r${1} "
}

function set_timeout(){
	printf "o${1} "
}

function set_delay(){
	printf "m${1} "
}

function reset () {
	printf "r${1} "
}

function left () {
	printf "j "
}

function right () {
	printf "l "
}

function up () {
	printf "i "
}

function down () {
	printf "k "
}

function quit () {
	printf "q "
}

function loop () {
	if (( $# >= 1 )) && (( $1 > 0 )) 
	then
		for (( i = 0; i < $1; i++ ))
		do
			printf " "
		done
	else
		printf " "
	fi
}

function create_trash(){
	printf "t${1},${2}"
}

function create_slime(){
	printf "s${1},${2}"
}

function create_dust(){
	printf "d${1},${2}"
}

function enter() {
	printf "${1},${2},${3} "
}

function vacuum_cheat(){
	printf "v${1},${2},${3} "
}

function change_battery(){
	printf "y${1} "
}

function return_to_base(){
	printf "b "
}
#========================================================================
#	Run a single test.
#========================================================================	

function run_test () {
	(( test_num ++ ))
	echo "Test    : ${test_num}"
	echo "Category: ${category}"
	echo "Details : ${details}"
	echo "Expect  : ${expect}"
	
	read -p "Press r to run the test or s to skip..." run_or_skip
	
	if [ "${run_or_skip}" == "r" ]
	then
		echo ${cmd} | ./Scratch
		read -p "Did the test pass or fail (p/f)? " result
		echo "Test ${test_num}: ${result}" >>${REPORT_FILE}
	fi
}

#==============================================================================
category="Setup vacuum"
details="Vacuum alone in center of small terminal window with the base at the top of the terminal underneath display status."
expect="Vacuum is displayed in centre; no trash is visible. The timer is running, but the battery is not decreasing."

cmd=$(
	enter 0 0 0
	set_timeout 3
)

run_test 
#==============================================================================
category="Setup one piece of trash"
details="Vacuum and 1 piece of trash."
expect="Vacuum is displayed in the centre; 1 piece of trash is visible and not overlapping with vacuum, base or terminal."

cmd=$(
	enter 0 0 1
	set_timeout 3
)
run_test
#==============================================================================
category="Setup max trash"
details="Vacuum and 5 pieces of trash."
expect="Vacuum is displayed in centre; 5 pieces of trash are visible; none overlap the vacuum, border or the base."

cmd=$(
	enter 0 0 5
	set_timeout 3
)

run_test 
#==============================================================================
category="Setup max slime"
details="Vacuum and 10 pieces of slime."
expect="Vacuum is displayed in centre; 10 pieces of slime are visible; none overlap the vacuum, border or the base."

cmd=$(
	enter 0 10 0
	set_timeout 3
)

run_test 
#==============================================================================
category="Setup max dust"
details="Vacuum and 1000 pieces of dust."
expect="Vacuum is displayed in centre; 1000 pieces of dust are visible; none overlap the vacuum, border of the base."

cmd=$(
	enter 1000 0 0
	set_timeout 3
)

run_test 

#==============================================================================
category="Setup trash and slime"
details="Vacuum, 10 pieces of slime and 5 pieces of trash."
expect="Vacuum is displayed in the centre; 5 pieces of trash and 10 pieces of slime are visible; none of the trash overlap with other trash, the vacuum, base or the terminal."

cmd=$(
	enter 0 10 5
	set_timeout 5
)
run_test

#===============================================================================
category="Setup dust and trash"
details="Vacuum, 1000 pieces of dust and 3 pieces of trash."
expect="Vacuum is displayed in the centre; 3 pieces of trash and 1000 pieces of dust are visible; none of the trash overlap with other objects or the terminal."

cmd=$(
	enter 1000 0 3
	set_timeout 5
)
run_test
#===============================================================================
category="Vacuum bounce"
details="Vacuum alone; moves downwards and bounces when it collides with terminal borders."
expect="Vacuum is displayed in the centre; starts moving downwards until it hits the terminal border and bounces off. The battery starts decrementing once the vacuum is moving."

cmd=$(
	enter 0 0 0
	pause_resume
	set_timeout 5
)
run_test
#==============================================================================
category="Trash collection (minimum screen size)"
details="Vacuum collects a piece of trash"
expect="Vacuum is displayed in center and piece of trash created underneath vacuum. When unpaused, the vacuum collects the piece of trash. The collected piece of trash should disappear and decrement from the display status."

cmd=$(
	enter 0 0 0
	set_timeout 3
	pause_resume
	create_trash 75 40
)
run_test
#==============================================================================
category="Max trash collection"
details="Vacuum and maximum number of trash"
expect="Vacuum is able to collect multiple pieces of trash. The trash display should decrement and the load should increment by 20g."

cmd=$(
	enter 0 0 5
	set_timeout 15
	pause_resume
)
run_test
#==============================================================================
category="Slime collection(minimum screen size)"
details="Vacuum and a piece of slime at 75, 40"
expect="Vacuum moves downwards and collects piece of slime. The slime should disappear, the display should decrement and the load should increment by 5g."

cmd=$(
	enter 0 0 0
	set_timeout 3
	pause_resume
	create_slime 75 40
)
run_test
#==============================================================================
category="Max slime collection"
details="Vacuum and maximum number of slime"
expect="Vacuum is able to collect multiple pieces of slime. The slime display should decrement and the load should increment by 5g."

cmd=$(
	enter 0 10 0
	set_timeout 15
	pause_resume
)
run_test
#==============================================================================
category="Dust collection"
details="Vacuum collecting dust."
expect="Vacuum is displayed in the center and starts moving downwards. 1000 dust is visible and the vacuum collects dust while moving. Dust display should decrement with each dust collected."

cmd=$(
	enter 1000 0 0
	set_timeout 5
	pause_resume
)
run_test
#==============================================================================
category="Slime, trash and dust collection"
details="Vacuum surrounded by rubbish."
expect="Vacuum moves downwards and collects trash along the way to the bottom of the terminal. 
Each trash item is randomly generated. The display should decrement the number of trash and update the vacuum load."

cmd=$(
	enter 1000 10 5
	pause_resume
	set_timeout 10
)
run_test
#==============================================================================
category="Battery update"
details="Changing battery level to test if it accurately decrements from a set level."
expect="Battery is initially 100 and set to 75 and continues decreasing by 1% per second from 75."

cmd=$(
	enter 0 0 0
	pause_resume
	change_battery 75
	set_timeout 5
)
run_test	
#===============================================================================
category="Return to base to recharge battery."
details="Vacuum is alone and reorientates towards base once battery is less than 25."
expect="Vacuum should reorientate itself and head towards base."

cmd=$(
	enter 0 0 0
	pause_resume
	change_battery 25
	return_to_base
	set_timeout 5
)
run_test
#================================================================================
category="Depleted battery"
details="Battery decreases to 0% from 3%, Vacuum collecting dust."
expect="Vacuum stops moving and collecting dust after 3 seconds. Simulation over message to be displayed."

cmd=$(
	enter 1000 0 0
	pause_resume
	set_timeout 5
	change_battery 3	
)
run_test
