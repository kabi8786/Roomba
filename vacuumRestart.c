#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
#include "helpers.h"
#include "vacuum.h"
#include "coll.h"

// Vacuum state
#define VACUUM_HEIGHT 9
#define VACUUM_WIDTH 9
#define VACUUM_SPEED 0.2

//Base state
#define BASE_HEIGHT 3
#define BASE_WIDTH 9
const double baseX = 75;
const double baseY = 5;

static double vx, vy, vdx, vdy;
static int angle;
static int load;
static int battery;
static int battery_max = 100;
static bool cleaning;

const int load_max = 65;

//Vacuum sprite
char * vacuum = 
"  @@@@@  "
" @XXXXX@ "
"@XXXXXXX@"
"@XXXXXXX@"
"@XXXXXXX@"
"@XXXXXXX@"
"@XXXXXXX@"
" @XXXXX@ "
"  @@@@@  "
;

char * base = 
"#########"
"#########"
"#########"
;

/**
 * Draws base sprite
 */
void draw_base(){
	int w = screen_width();
	int base_pos = w / 2;
	draw_sprite(base_pos, 5, BASE_WIDTH, BASE_HEIGHT, base, false);
}

/**
 * Draws vacuum sprite
 */
void draw_vacuum(){
	int w, h;
	get_screen_size(&w, &h);
	draw_sprite(get_vacuum_x(), get_vacuum_y(), 
		VACUUM_WIDTH, VACUUM_HEIGHT, vacuum, true);
}

 /**
 *  Set up the vacuum at the centre of the terminal.
 */
 void setup_vacuum(){	 
	int w, h;
	get_screen_size(&w, &h);
	
	//if width of screen is uneven
	 if (w % 2 != 0){
		set_vacuum_x(w / 2);
	} else {
		//offset by 1
		set_vacuum_x((w / 2) + 1);
	}

	//if height of screen is uneven
	if (h % 2 != 0){
		set_vacuum_y(h / 2);
	} else {
		//offset by 1
		set_vacuum_y((h / 2) + 1);
	}  
	set_vacuum_angle(90);
	set_vacuum_mode(true);
	set_load(0);
 }
 
 /**
 * Decrease battery reading by 1% per second
 */
 void update_battery(double current_time, double start_time){
	int time_difference = current_time - start_time;
	set_battery(battery_max - time_difference);
 } 
 
 /**
  * Determine new angle for vacuum to turn when it hits a wall
  * between 30 and 60 degrees inclusive
  */
void update_direction(){
	int rng = rand() % (60 + 1 - 30) + 30;
	
	//add randomly generated angle to current angle
	//int new_angle = get_vacuum_angle() + rng;
	//double new_direction = rng * (M_PI/180);
	
	set_vacuum_angle(rng);
	vdx = VACUUM_SPEED * cos (rng);
	vdy = VACUUM_SPEED * sin (rng);
 }
 
 /**
  * Stop movement of vacuum by storing current x, y coords into vdy and vdx
  */
 void stationary_vacuum(){
	vdy = get_vacuum_y();
	vdx = get_vacuum_x();
} 

/**
 * Move vacuum within boundary of terminal. If wall of terminal encountered, 
 * vacuum bounces off at a 30 - 60 degree angle. 
 * Header reading isn't portrayed accurately
 */
void update_vacuum(bool paused){
	int h, w;
	get_screen_size(&w, &h);
	
	//if not paused, move vacuum
	if (paused == false){
		vdx = VACUUM_SPEED * cos(angle);
		vdy = VACUUM_SPEED * sin(angle);
	} else {
		//stop movement of vacuum
		stationary_vacuum();
	}
	
	//get location of vacuum
	int new_x = round(vx + vdx) - VACUUM_WIDTH / 2;
	int new_y = round(vy + vdy) - VACUUM_HEIGHT / 2;
	bool bounced = false;
	
	//if top or bottom of vacuum hits bottom or upper borders of terminal or bottom of base
	if (new_y == 1 || new_y + VACUUM_HEIGHT == h - 6 ){
		//reflect off wall
		vdy = -vdy;
		bounced = true;
		update_direction();
	}
	
	//if left of right side of vacuum hits left or right borders of terminal	
	if (new_x == -3 || new_x + VACUUM_WIDTH == w - 4){
		vdx = -vdx;
		bounced = true;
		update_direction();
	}
	
	//for the beginning of simulation, only vy would be used
	if (get_vacuum_angle() == 90 || get_vacuum_angle() == 270){
		vdx = get_vacuum_x();
		vy += vdy;
	//after bouncing, both vx and vy are utilised to update movement of vacuum
	} /* else if (get_vacuum_angle() == 180 || get_vacuum_angle() == 0){
		vdy = get_vacuum_y();
		vx += vdx;
	} */else if (!bounced){
		vx += vdx;
		vy += vdy;
	} 
}
 

/**
 * Allow user to change battery level
 */
void do_battery_cheat(){
	int new_battery = get_int("Set battery level as? (0 - 100% inclusive)");
	battery_max = new_battery;
}

/**
 *  Returns true iff and only if the supplied argument is a vacuum navigation control.
 */
bool is_vacuum_ctrl( int ch ) {
    return ( ch == 'j' || ch == 'k' || ch == 'l' || ch == 'i' );
}

/**
 * Update vacuum location based on character input 
 * Need to update direction of vacuum if manual controls are enabled
 */
void move_vacuum( int ch ) {
	//move vacuum one unit to the left without overlapping left border
    if ( ch == 'j' && vx - VACUUM_WIDTH / 2 > - 3 ) {
        vx--;
    }
	//move vacuum one unit to the right
    else if ( ch == 'l' && vx + VACUUM_WIDTH / 2 < screen_width() - 6 ) {
        vx++;
    }
	//move vacuum one unit downwards
    else if ( ch == 'k' && vy + VACUUM_HEIGHT / 2 < screen_height() - 8 ) {
        vy++;
    }
	//move vacuum one unit upwards
    else if ( ch == 'i' && vy - VACUUM_HEIGHT / 2 > 1 ) {
        vy--;
    }
}

/**
 * Reposition vacuum position and direction based on specified values
 */
 void do_vacuum_cheat(){
	 int h, w;
	 get_screen_size(&w, &h);
	 
	 //store previous position and angle of vacuum
	 int old_x = get_vacuum_x();
	 int old_y = get_vacuum_y();
	 int old_angle = get_vacuum_angle();
	 
	 int x = get_int("New x position of vacuum?");
	 //if given x coord is out of range, set to previous x coordinate
	 if (x < 0 || x > w - 1){
		 x = old_x;
	 }
	 
	 int y = get_int("New y position of vacuum?");
	 if (y < 4 || y > h - 3){
		 y = old_y;
	 }
	 
	 int v_angle = get_int("New orientation? (0 - 359 degrees)");
	 //if specified angle is not within range, change back to original orientation
	 if (v_angle < 0 && v_angle > 359){
		 set_vacuum_angle(old_angle);
	 } else {
		 set_vacuum_angle(angle);
	 }
	 
	 //update movement of vacuum
	 vdx = VACUUM_SPEED * cos(get_vacuum_angle() * M_PI/180);
	 vdy = VACUUM_SPEED * sin(get_vacuum_angle() * M_PI/180);
 }

void do_load_cheat(){
	int old_load = get_load();
	int new_load = get_int("New load stored in vacuum?");
	
	//if new_load exceeds load limit
	if (new_load < 0 || new_load > load_max){
		set_load(old_load);
	} else {
		set_load(new_load);
	}
}

/**
 * Updates the x, y vectors of the vacuum based on the distance between vacuum and base
 * Move the vacuum towards the base. 
 */
void move_vacuum_to_base(){
	if (get_battery() < 25){
		
		int t1 = get_baseX() - get_vacuum_x();
		int t2 = get_baseY() - get_vacuum_y();
		int d = sqrt(t1 * t1 + t2 * t2);
		
		vdx = t1 * VACUUM_SPEED / d;
		vdy = t2 * VACUUM_SPEED / d;
		
		vx += vdx;
		vy += vdy;
	}
	
	set_max_battery(100);
	set_battery(100);
	get_load(0);
}

 /** 
  * Access x coord of vacuum
  */
double get_vacuum_x(){
	return vx;
}

 /** 
  * Set x coord of vacuum
  */
void set_vacuum_x(double x){
	vx = x;
};

 /** 
  * Access y coord of vaccum
  */
double get_vacuum_y(){
	return vy;
}

 /** 
  * Set y coord of vacuum
  */
void set_vacuum_y(double y){
	vy = y;
}

/**
 * Access the vacuum width
 */
int get_vacuum_width(){
	return VACUUM_WIDTH;
}

/** 
 * Access the vacuum height
 */
int get_vacuum_height(){
	return VACUUM_HEIGHT;
}

/**
 * Access the base X coord
 */
double get_baseX(){
	return baseX;
}

/**
 * Access the base Y coord
 */
double get_baseY(){
	return baseY;
}

/**
 * Access the base height
 */
int get_base_height(){
	return BASE_HEIGHT;
}

/**
 * Access the base width
 */
int get_base_width(){
	return BASE_WIDTH;
}

/**
 * Access the base bitmap
 */
char * get_base_bmap(){
	return base;
}

/**
 * Access the vacuum bitmap
 */
char * get_vacuum_bmap(){
	return vacuum;
}
 /** 
  * Access vacuum angle to be displayed
  */
int get_vacuum_angle(){
	return angle;
}

 /** 
  * Set angle of vacuum to be displayed
  */
void set_vacuum_angle(int new_angle){
	if (new_angle >= 0 && new_angle <= 359){
		angle = new_angle;
	}
}

/**
 * Access battery level to be displayed
 */
 int get_battery(){
	 return battery;
 }
 
/**
 * Set battery level to be displayed
 */
 void set_battery(int given_battery){
	//store current battery valid if invalid given_battery
	int current_battery = get_battery();
	
	if (given_battery >= 0 && given_battery <= 100){
		battery = given_battery;
	} else {
		battery = current_battery;
	}
}

/**
 * Set max battery to count down from
 */
void set_max_battery(int new_max){
	int max = battery_max;
	if (new_max >= 0 && new_max <= 100){
		battery_max = new_max;
	} else {
		battery_max = max;
	}
}

 /**
  * Access vacuum capacity
  */
 int get_load(){
	 return load;
 }
 
 /**
  * Set vacuum capacity to be displayed
  */
void set_load(int capacity){
	int current_load = get_load();
	if (capacity >= 0 && capacity <= 65){
		load = capacity;
	} else {
		load = current_load;
	}
}

/**
 * Access vacuum pump state
 */
bool get_vacuum_mode(){
	return cleaning;
}

/**
 * Change vacuum pump state
 */
void set_vacuum_mode(bool on_off){
	cleaning = on_off;
}




