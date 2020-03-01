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

// Trash state
#define DUST_WEIGHT 1
#define DUST_MAX 1000
#define DUST_DIM 1

#define SLIME_WEIGHT 5
#define SLIME_MAX 10
#define SLIME_DIM 5

#define TRASH_WEIGHT 20
#define TRASH_WIDTH 11
#define TRASH_HEIGHT 6
#define TRASH_MAX 5

static int num_trash;
static int numtrash_display;

static int num_slime;
static int numslime_display;

static int num_dust;
static int numdust_display;
double trashX[TRASH_MAX], trashY[TRASH_MAX]; 
double slimeX[SLIME_MAX], slimeY[SLIME_MAX];
double dustX[DUST_MAX], dustY[DUST_MAX]; 
	
char * dust = 
"."
;

char * slime = 
"  ~  "
" ~~~ "
"~~~~~"
" ~~~ "
"  ~  "
;

char * trash = 
"     &     "
"    &&&    " 
"   &&&&&   "
"  &&&&&&&  "
" &&&&&&&&& "
"&&&&&&&&&&&"
;

/**
 * 'Erases' trash item if collected by the vacuum
 */
void erase_trash(int erase_trashID){
	//iterate through trash items
	for (int i = 0; i < num_trash; i++){
		//if the current trash item is the same one that 
			//the vacuum collided with
		if (i == erase_trashID){
			//decrement num trash display
			int new_trash_display = get_numtrash_display();
			new_trash_display--;
			set_numtrash_display(new_trash_display);
			//move trash item off-screen
			trashX[i] = 1000;
			trashY[i] = 1000;
			//add weight of trash to load
			int new_load = get_load() + TRASH_WEIGHT;
			set_load(new_load);
			return;
		}
	}
}

/**
 * 'Erases slime items if collected by the vacuum
 */
void erase_slime(int erase_slimeID){
	for (int s = 0; s < num_slime; s++){
		if (s == erase_slimeID){
			int new_slime_display = get_numslime_display();
			new_slime_display--;
			set_numslime_display(new_slime_display);
			
			slimeX[s] = 1000;
			slimeY[s] = 1000;
			
			int new_load = get_load() + SLIME_WEIGHT;
			set_load(new_load);
			return;
		}
	}
}

/**
 * 'Erases' dust if collected by the vacuum
 */
 void erase_dust(int erase_dustID){
	for (int d = 0; d < num_dust; d++){
		if (d == erase_dustID){
			int new_dust_display = get_numdust_display();
			new_dust_display--;
			set_numdust_display(new_dust_display);
			
			dustX[d] = 1000;
			dustY[d] = 1000;
			
			int new_load = get_load() + DUST_WEIGHT;
			set_load(new_load);
			return;
		}
	}
 }
 
/**
 * Draw one trash item, indicated by ID
 */
void draw_trash(int i){
	if (num_trash > 0 && num_trash <= TRASH_MAX){
		int left = round(trashX[i]) - TRASH_WIDTH / 2;
		int top = round(trashY[i]) - TRASH_HEIGHT / 2;
		draw_sprite(left, top, TRASH_WIDTH, TRASH_HEIGHT, trash, true);
	}
}

/**
 * Draw one slime item, indicated by ID
 */
void draw_slime(int s){
	if (num_slime > 0 && num_slime <= SLIME_MAX){
		int left = round(slimeX[s]) - SLIME_DIM / 2;
		int top = round(slimeY[s]) - SLIME_DIM / 2;
		draw_sprite(left, top, SLIME_DIM, SLIME_DIM, slime, true);
	}
}

/**
 * Draws one dust, indicated by ID
 */
 void draw_dust(int d){
	 if (num_dust > 0 && num_dust <= DUST_MAX){
		 draw_sprite(dustX[d], dustY[d], DUST_DIM, DUST_DIM, dust, false);
	 }
 }
 
/**
 * Draw all trash items
 */
void draw_all_trash(){
	for (int i = 0; i < num_trash; i++){
		draw_trash(i);
	}
}

/**
 * Draw all slime items
 */
void draw_all_slime(){
	for (int s = 0; s < num_slime; s++){
		draw_slime(s);
	}
}

/**
 * Draws all dust
 */
void draw_all_dust(){
	for (int d = 0; d < num_dust; d++){
		draw_dust(d);
	}
}

/**
 * Draw all rubbish
 */
void draw_all_rubbish(){
	draw_all_trash();
	draw_all_slime();
	draw_all_dust();
}

/**
 * Generate a random location for a trash item to be drawn at within terminal
 */
void trash_coords(int i){
	int w, h;
	get_screen_size(&w, &h);
	//coords to keep within border
	trashX[i] = 1 + TRASH_WIDTH / 2 + rand() % (w - 1 - TRASH_WIDTH);
	trashY[i] = 5 + TRASH_HEIGHT / 2 + rand() % (h - 4 - TRASH_HEIGHT/2 - TRASH_HEIGHT);
}

/**
 * Generate a random location for a single slime to be drawn at within terminal
 */
void slime_coords(int s){
	int w, h;
	get_screen_size(&w, &h);
	slimeX[s] = 1 + SLIME_DIM / 2 + rand() % (w - 1 - SLIME_DIM);
	slimeY[s] = 5 + SLIME_DIM / 2 + rand() % (h - 4 - SLIME_DIM / 2 - SLIME_DIM);
}

/**
 * Generate a random location for a single dust to be drawn at within terminal
 */
void dust_coords(int d){
	int w, h;
	get_screen_size(&w, &h);
	dustX[d] = (1 + DUST_DIM) + rand() % (w - 2 - DUST_DIM);
	dustY[d] = (4 + DUST_DIM) + rand() % (h - 7 - DUST_DIM);
}
/**
 * Set up all trash items, ensuring they don't overlap other trash,
 * the base, vacuum or the terminal
 */
void setup_trash(){
	int trash_made = 0;
	for (int i = 0; i < num_trash; i++){
		trash_coords(i);
		//generate coords
		trash_made++;
		//until supplied coords don't cause a collision, 
		while (check_trash_setup_collisions(i, trash_made) == true){
			//if (trash-trash collision is true)
				//regenerate trash coords
			//continue regenerating trash coords
			trash_coords(i);
		}

	}
}

/**
 * Set up all slime items, ensuring they don't overlap with other trash,
 * the base, vacuum or the terminal
 * If collision would occur from produced coords, regenerate coords.
 */
void setup_slime(){
	int slime_made = 0; 
	for (int s = 0; s < num_slime; s++){
		slime_coords(s);
		slime_made++;
		//check if new slime collides with other objects
		while (check_slime_setup_collisions(s, slime_made) == true){
			//if true, regenerate coords
			slime_coords(s);
		}
	}
}

/**
 * Setup all dust items if they don't overlap with another object
 */
void setup_dust(){
	int dust_made = 0;
	//only want dust to be draw if generated coords are already opaque
	for (int d = 0; d < num_dust; d++){
		dust_coords(d);
		dust_made++;
		//while there is not a collision between dust and an opaque space
		while (check_dust_setup_collisions(d, dust_made) == true){
			//regenerate coords 
			dust_coords(d);
		}
		
		
	}
} 

/**
 * Allow user to create a new trash item
 */
void create_trash(){
	int trash_id = num_trash++;
	int new_trashX = get_int("Trash X coord?");
	int new_trashY = get_int("Trash Y coord?"); 
	
	//confirm if creating new trash exceeds trash max
	if ( trash_id <= TRASH_MAX ) {
        trashX[trash_id] = new_trashX;
        trashY[trash_id] = new_trashY;
		draw_trash(trash_id);
    }
}

/**
 * Allow user to create a new slime 
 */
 void create_slime(){
	int slime_id = num_slime++;
	 //prompt for coords
	int new_slimeX = get_int("Slime X coord?");
	int new_slimeY = get_int("Slime Y coord?");
	
	//if additional created slime doesn't exceed limit
	if (slime_id <= SLIME_MAX){
		//draw the new slime
		slimeX[slime_id] = new_slimeX;
		slimeY[slime_id] = new_slimeY;
		draw_slime(slime_id);
	}
}

/**
 * Allow user to create a new piece of dust
 */
void create_dust(){
	int dust_id = num_dust++;
	//prompt for coords
	int new_dustX = get_int("Dust X coord?");
	int new_dustY = get_int("Dust Y coord?");
	
	//if additional created dust doesn't exceed limit
	if (dust_id <= DUST_MAX){
		//draw the new dust
		dustX[dust_id] = new_dustX;
		dustY[dust_id] = new_dustY;
		draw_dust(dust_id);
	}
}
 
/**
 * Determine how much rubbish to draw and setup for each trash item and  
 */
void setup_rubbish(){
	//prompt for number of rubbish to drop
	num_dust = get_int("How much dust would you like to drop? (Up to 1000)");
	num_slime = get_int("How much slime would you like to drop? (Up to 10)");
	num_trash = get_int("How much trash would you like to drop? (Up to 5)");
	
	//Assign number of trash inputs to display
	set_numtrash_display(num_trash);
	set_numslime_display(num_slime);
	set_numdust_display(num_dust);
	
	//Determine what happens when numbers outside of trash boundaries are inputted
	if (num_trash > TRASH_MAX || num_slime > SLIME_MAX|| num_dust > DUST_MAX){
		num_trash = TRASH_MAX;
		num_slime = SLIME_MAX;
		num_dust = DUST_MAX;
	} else if (num_trash < 0 || num_slime < 0 || num_dust < 0){
		num_trash = 1;
		num_slime = 1;
		num_dust = 1;
	}
	
	//set up each trash item
	setup_trash();
	setup_slime();
	setup_dust();
}

/** 
 * Access coord x of trash based on ID
 */
int get_trashX(int i){
	return trashX[i];
}

/** 
 * Access coord y of trash based on ID
 */
int get_trashY(int i){
	return trashY[i];
}

/**  
 * Access number of declared trash
 */
int get_trash_num(){
	return num_trash;
}

/** 
 * Access height of trash
 */
int get_trash_height(){
	return TRASH_HEIGHT;
}

/** 
 * Access width of trash
 */
int get_trash_width(){
	return TRASH_WIDTH;
}

/** 
 * Access trash bitmap
 */
char * get_trash_bmap(){
	return trash;
}

/** 
 * Set number of visible trash
 */
void set_numtrash_display(int new_numtrash){
	numtrash_display = new_numtrash;
}

/** 
 * Access number of visible trash
 */
int get_numtrash_display(){
	return numtrash_display;
}

/** 
 * Access the x coord of the slime based on ID
 */
int get_slimeX(int i){
	return slimeX[i];
}

/**
 * Access the y coord of the slime based on ID
 */
int get_slimeY(int i){
	return slimeY[i];
}

/**
 * Access size of slime
 */
int get_slime_dim(){
	return SLIME_DIM;
}

/**
 * Access the number of slimes declared
 */
int get_slime_num(){
	return num_slime;
}

/**
 * Access the slime bitmap
 */
char * get_slime_bmap(){
	return slime;
} 

/**
 * Set the number of visible slimes to display
 */
void set_numslime_display(int new_numslime){
	numslime_display = new_numslime;
}

/**
 * Access the number of visible slimes on display
 */
int get_numslime_display(){
	return numslime_display;
}

/** 
 * Access dust X based on ID
 */
int get_dustX(int d){
	return dustX[d];
}

/** 
 * Access dust Y based on ID
 */
int get_dustY(int d){
	return dustY[d];
}

/** 
 * Access dust dimensions
 */
int get_dust_dim(){
	return DUST_DIM;
}

/** 
 * Access number of declared dust 
 */
int get_dust_num(){
	return num_dust;
}

/** 
 * Set number of visible dust 
 */
void set_numdust_display(int new_numdust){
	numdust_display = new_numdust;
}

/** 
 * Access number of visible dust
 */
int get_numdust_display(){
	return numdust_display;
}

/** 
 * Access dust bitmap
 */
char * get_dust_bmap(){
	return dust;
}