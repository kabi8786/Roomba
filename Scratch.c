#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
#include "helpers.h"
#include "vacuum.h"
#include "trash.h"
#include "coll.h"

// Global variables
bool game_over = false;
int delay = 10;
int timeout = INT_MAX;
double start_time;
double paused_start_time;
static bool paused = true;


bool coolided(
	double x0, double y0, int w0, int h0, 
	double x1, double y1, int w1, int h1
);
	

void draw_display_rows(int w){
	int columnH = 6;
	
	draw_line(1, 2, w - 2, columnH/2 - 1, '-');
	draw_line(1, columnH - 2, w - 2, columnH - 2, '-');
}

/**
 * Draws vertical lines, horizontal lines and corners of display 
 */
void draw_alternating_char(int w, int columnW){	
	 for (int x = 0; x <= columnW * 2; x += columnW){
		 //determine symbol to draw
		for (int y = 0; y <= 4; y++){
			 //even y-coord or top of display
			if (y % 2 == 0 || y == 0){
				draw_char(x, y, '+');
				//draw corner for right side of display
					//when screen-size isn't divisible for 3 columns
				draw_char(w - 1, y, '+');
			} else {
				//draw '|' for odd y-coord
				draw_char(x, y, '|');
			}
		}
	 }
	 //draw inner intersections of display as '|'
	 draw_char(columnW, 2, '|');
	 draw_char(columnW * 2, 2, '|');
}

/**
 * Draws border lines of terminal 
 */
void draw_terminal_borders(){
	//get screen dimensions
	int w, h;
	get_screen_size(&w, &h);
	
	//draw horizontal lines of terminal border
	draw_rect(0, 0, w - 1, h - 3, '-');
	//right edge
    draw_line(w - 1, 1, w - 1, h - 3, '|');
	//left edge
    draw_line(0, 1, 0, h - 3, '|');
	
	//draw corners at bottom of terminal
	draw_char(0, h - 3, '+'); //bottom left corner
	draw_char(w - 1, h - 3, '+'); //bottom right corner
}

/**
 * Display values in status display
 */
void display_info(int columnW){
	int x_position = columnW / 2;
	
	//display student No
	draw_string(x_position - 4, 1, "N10269142");
	//display angle of vacuum
	draw_formatted(x_position * 2 + 10, 1, "Current angle: %d ", get_vacuum_angle());
	//display the current battery levels of the vaccum
	draw_formatted(x_position * 4 + 10, 1, "Current battery: %d", get_battery());
	
	//calculate elapsed time from current time and start time
	int elapsed_time = round(get_current_time() - start_time);
	int min = elapsed_time / 60;
	int sec = elapsed_time % 60;
	//display elapsed time
	draw_formatted(x_position - 10, 3, "Elapsed time: %02d:%02d", min, sec);
	
	//display current weight in vacuum
	draw_formatted(x_position * 2 + 10, 3, "Current load(g): %d", get_load()); 
	//display trash waiting to be collected
	draw_formatted(x_position * 4 + 10, 3, "Litter (d, s, t): %d, %d, %d", 
					get_numdust_display(), get_numslime_display(), get_numtrash_display());
}

/**
 * Draws border lines of status display and displays information
 */
void draw_display(){
	int w, h;
	get_screen_size(&w, &h);
	int columnW = (w - 1) / 3;
	
	draw_display_rows(w);
	draw_alternating_char(w, columnW);
	
	display_info(columnW);
}

/**
 * Draws border lines of terminal and status display
 */
void draw_border(){
	
	draw_terminal_borders();

	draw_display();
}

/**
 *  Draw all objects in the simulation.
 */
 void draw_all(){
	 clear_screen();
	 draw_border();
	 draw_base();
	 draw_vacuum();
	 draw_all_rubbish();

	 show_screen();
 }
 
 
 /**
 *  Set up all objects in the simulation.
 */
 void setup(void){
	 start_time = get_current_time();
	 paused_start_time = start_time;
	 setup_vacuum();
	 set_battery(100);
	 setup_rubbish();
	 draw_all();
 }
 
 /**
 *  Resets the simulation.
 */
 void reset(){
	 clear_screen();
	 paused = true;
	 setup();
 }
 
 /**
 *  Returns true if the simulation has been running for longer than the 
 *  current designated time-out value.
 */
bool timed_out() {
    return get_current_time() - start_time >= timeout;
}
 
 /**
 *  Obtain a time-out value from standard input and store it for future use.
 */
void do_timeout() {
    timeout = get_int("How many seconds shall the program run?");
}
 
 
  /**
 *  Toggles vacuum motion between paused and not-paused.
	
 */
 void do_pause(){
	 paused = !paused;	 
	 
	 //Updating battery states when simulation pause/unpauses
	 if (paused == true){
		 //set max battery as the current battery when paused
		 set_max_battery(get_battery());
	 } else {
		 //update start time as the first second the game resumes
		 paused_start_time = get_current_time();
	 }
 }
 
/**
 *  Toggles vacuum motion between paused and not-paused.
	when not paused, move vacuum to move downwards
 */
void do_delay_cheat(){
	delay = get_int("New delay (milliseconds)?");
	
	if (delay < 0) delay = 10;
}

/**
 *  Dialog which displays a help screen.
 */
void do_help() {
    clear_screen();
    draw_formatted_lines(0, 0,
        "!!!!!!!!!!!!!!!!!!!!!!!!!\n"
        "!!! Vacuum Simulator  !!!\n"
        "!!!!!!!!!!!!!!!!!!!!!!!!!\n"
        "\n"
		"b: tell the vacuum to stop cleaning and return to base\n"
		"d: drop a piece of dust on the floor\n"
        "i, j, k, l: move vacuum north, west, south and east by one unit respectively\n"
        "m: set millisecond delay between successive invocations of the loop function\n"
		"o: set the time-out period i.e. the total permitted run time measured in seconds\n"
		"p: pause or resume vacuum movement\n"
		"q: quit simulation\n"
		"r: restart simulation\n"
        "s: drop a piece of slime on the floor\n"
		"t: drop a piece of trash on the floor\n"
		"v: move the vacuum to a new location and specify direction\n"
		"w: change the load stored in the vacuum\n"
		"y: change the battery level of the vacuum\n"
        "?: display this help screen\n\n\n"
        "Press a key to return to the game."
    );
    show_screen();

    while (get_char() > 0) {}
    while (!timed_out() && wait_char() <= 0) {}
}

void create_rubbish(int ch){
	if (ch == 't'){
		create_trash();
		//update display number
		set_numtrash_display(get_trash_num());
	} else if (ch == 's'){
		create_slime();
		//update display number
		set_numslime_display(get_slime_num());
	} else if (ch == 'd'){
		create_dust();
		//update display number
		set_numdust_display(get_dust_num());
	} 
}
	
/**
 * Display a 'Simulation over' message once battery has depleted
 */
void game_over_display(){
	
	draw_formatted((screen_width() - 35) / 2, (screen_height() - 7) / 2, 
		"***********************************\n"
		"*        Simulation Over!         *\n"
		"*                                 *\n"
		"* Press 'r' to restart simulation *\n"
		"*               OR                *\n"
		"* Press 'q' to exit simulation    *\n"
		"***********************************\n"
	);
	
	while (get_char() > 0){
		if (get_char() == 'r'){
		reset();
		} else if (get_char() == 'q'){
			return;
		}
	}
	while (!timed_out() && wait_char() <= 0) {}
	
}

/**
 *  Jump table which chooses the action (if any) which corresponds to a 
 *  supplied command character.
 *
 *  Parameters:
 *      ch: a command character. Currently recognised comands are listed in 
 *          the do_help function.
 */
 void do_operation(int ch){
	 if (ch == 'r'){
		 reset();
	 }
	 else if (ch == 'o'){
		 do_timeout();
	 }
	 else if (ch == 'p'){
		 do_pause();
	 } 
	 else if (ch == 'm'){
		 do_delay_cheat();
	 }
	 else if (ch == '?'){
		 do_help();
	 } 
	 else if (is_vacuum_ctrl(ch)){
		 move_vacuum(ch);
	 } 
	 else if (ch == 'v'){
		do_vacuum_cheat();
	 } 
	 else if (ch == 'y'){
		 do_pause();
		 do_battery_cheat();
		 do_pause();
	 }
	 else if ((ch == 't') || (ch == 's') || (ch == 'd')){
		 create_rubbish(ch);
	 } 
	 
 }
 
 /**
  *  The loop function: called once per time slice to run the simulation.
  */
 void loop() {
	 int ch = get_char();
	 
	if (ch == 'q' || get_battery() == 0){
		 game_over = true;
		 return;
	 }
	 else if (ch >= ' '){
		 do_operation(ch);
	 }
	 else if (paused == false){
		 //move the vacuum when the game isn't paused
		 update_vacuum(paused);
		 //update the battery when the game isn't paused
		 update_battery(get_current_time(), paused_start_time);
	 } 
	 else if (get_battery() == 0){
		do_pause();
		game_over_display();
	 } 
	 else if (get_battery() < 25 || get_load() > 45 || ch == 'b'){
		set_vacuum_mode(false);
		move_vacuum_to_base();
		check_base_vacuum_collision();
		check_vacuum_rubbish_collisions();
	 }
	 check_base_vacuum_collision();
	 check_vacuum_rubbish_collisions();
	 
	 draw_all();

}

/**
 *  Program start and event loop.
 */
 int main(void){
	 srand(get_current_time());
	 
	 setup_screen();
	 setup();
	 
	 while(!game_over && !timed_out()){
		 loop();
		 timer_pause(delay);
	 } 		 
	 
	 if (get_battery() == 0){
		 stationary_vacuum();
		 game_over_display();
	 }
	 return 0;
 }

