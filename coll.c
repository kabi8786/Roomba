#include "vacuum.h"
#include "trash.h"
#include "helpers.h"
#include "cab202_graphics.h"
#include "cab202_timers.h"
#include "coll.h"


bool opaque(int left, int top, int width, int height, int x, int y, char pixels[]){
	return (x >= left && x < left + width && y >= top && 
		y < top + height && pixels[(x - left) + (y - top) * width] != ' ');
	
}

/** 
 * Determines whether the pixel collision between 2 bitmaps occur
 */
bool pixel_collision(int x0, int y0, int w0, int h0, char pixels0[], int x1, int y1, int w1, int h1, char pixels1[]){
	bool collision = false;
	for (int i = 0; i < (y0 + h0); i++){
		for (int j = 0; j < (x0 + w0); j++){
			if (opaque(x0, y0, w0, h0, j, i, pixels0) && opaque(x1, y1, w1, h1, j, i, pixels1)){
				collision = true;
			}
		}
	}
	
	return collision;
}

/**
 * Determines if current trash item overlaps with previously made trash items
 * Errors with running this collision test - after inputting num trash at begininng of
 * simulation, screen doesn't load into main screen. 
 */
/* bool check_trash_made_collision(int trash_made){
	//
	for (int i = trash_made - 1; i <= 0; i--){
		if (pixel_collision(
			get_trashX(i), get_trashY(i), get_trash_width(), get_trash_height(), get_trash_bmap(),
			get_trashX(trash_made), get_trashY(trash_made), get_trash_width(), get_trash_height(), get_trash_bmap()){
				return true;
				return;
			}
	}
	return false;
} */

/**
 * Determines if trash coords overlap with vacuum
 */
bool check_trash_setup_vacuum_collision(int i){
	if (pixel_collision(
		get_vacuum_x(), get_vacuum_y(), get_vacuum_width(), get_vacuum_height(), get_vacuum_bmap(),
		get_trashX(i), get_trashY(i), get_trash_width(), get_trash_height(), get_trash_bmap())){
			return true;
		}
	return false;
}

/**
 * Determines if slime coords overlap vacuum
 */
bool check_slime_setup_vacuum_collision(int s){
	if (pixel_collision(
		get_vacuum_x(), get_vacuum_y(), get_vacuum_width(), get_vacuum_height(), get_vacuum_bmap(),
		get_slimeX(s), get_slimeY(s), get_slime_dim(), get_slime_dim(), get_slime_bmap())){
		return true;
	}
	return false;
} 

/**
 * Determines if dust coords overlap vacuum
 */
bool check_dust_setup_vacuum_collision(int d){
	if (pixel_collision(
		get_vacuum_x(), get_vacuum_y(), get_vacuum_width(), get_vacuum_height(), get_vacuum_bmap(),
		get_dustX(d), get_dustY(d), get_dust_dim(), get_dust_dim(), get_dust_bmap())){
			return true;
		}
	return false;
}
/**
 * Determines if trash coords overlap with base
 */ 
bool check_trash_setup_base_collision(int i){
	if (pixel_collision(
		get_baseX(), get_baseY(), get_base_width(), get_base_height(), get_base_bmap(),
		get_trashX(i), get_trashY(i), get_trash_width(), get_trash_height(), get_trash_bmap())){
			return true;
	}
	return false;
}

/**
 * Determines if slime coords overlap with base
 */ 
bool check_slime_setup_base_collision(int s){
	if (pixel_collision(
		get_baseX(), get_baseY(), get_base_width(), get_base_height(), get_base_bmap(),
		get_slimeX(s), get_slimeY(s), get_slime_dim(), get_slime_dim(), get_slime_bmap())){
			return true;
	}
	return false;
}

/**
 * Determines if dust coords overlap with base
 */ 
bool check_dust_setup_base_collisions(int d){
	if (pixel_collision(
		get_baseX(), get_baseY(), get_base_width(), get_base_height(), get_base_bmap(),
		get_dustX(d), get_dustY(d), get_dust_dim(), get_dust_dim(), get_dust_bmap())){
			return true;
		}
	return false;
}

bool check_trash_setup_collisions(int i, int trash_made){
	//if any collisions occur, return true
	if (check_trash_setup_vacuum_collision(i) ||
		check_trash_setup_base_collision(i)){
				   return true;
			   }
	//if no collisions occur, return false
	for (int trash = 0; trash < trash_made; trash++){
		if (!check_trash_setup_vacuum_collision(trash) &&
			!check_trash_setup_base_collision(trash)){
				return false;
		}
	}
	//return true if any collisions occur
	return true;
}

/** 
 * Determines whether slime collides with other objects
 */
bool check_slime_setup_collisions(int s, int slime_made){
	if (check_slime_setup_vacuum_collision(s) ||
		check_slime_setup_base_collision(s)){
			return true;
		}
	for (int slime = 0; slime < slime_made; slime++){
		if (!check_slime_setup_vacuum_collision(slime) ||
			!check_slime_setup_base_collision(slime)){
				return false;
			}
	}
	return true;
} 

/**
 * Determines whether dust collides with other objects
 */
bool check_dust_setup_collisions(int d, int dust_made){
	if (check_dust_setup_base_collisions(d) ||
		check_dust_setup_vacuum_collision(d)){
			return true;
		}
	for (int dust = 0; dust < dust_made; dust++){
		if(!check_dust_setup_base_collisions(dust) ||
		   !check_dust_setup_vacuum_collision(dust)){
			   return false;
		   }
	}
	return true;
}

/* bool dust_setup_collisions(int d){
	int w, h;
	get_screen_size(&w, &h);
	
	//only want dust to draw when given coords are opaque
	for (int y = 5; y < h - 3; y++){
		for (int x = 1; x < w - 1; x++){
			if (pixel_collision(
				x, y, 1, 1, opaque_space,
				get_dustX(d), get_dustY(d), get_dust_dim(), get_dust_dim(), get_dust_bmap())){
					//if there's a collision between dust and an opaque space
					return true;
				}
		}		
	}
	return false;
} */

/**
 * Determine vacuum-base collision behaviour
 */
void check_base_vacuum_collision(){
	set_vacuum_mode(true);
	if (pixel_collision(
		get_baseX(), get_baseY(), get_base_width(), get_base_height(), get_base_bmap(), 
		get_vacuum_x(), get_vacuum_y() - 1, get_vacuum_width(), get_vacuum_height(), get_vacuum_bmap())){
				if (get_vacuum_mode() == true){
					//bounce off from base
					update_direction();
					return;
				} else {
					//don't bounce from base
					set_vacuum_y(get_baseY() + get_base_height());
					stationary_vacuum();
					set_max_battery(100);
					return;
				}
				
	}
}

/**
 * Determines whether vacuum-trash collisions occurs to erase trash if pump is on
 */
void check_vacuum_trash_collision(){
	if (get_vacuum_mode() == true){
		for (int i = 0; i < get_trash_num(); i++){
			if (pixel_collision(
				get_trashX(i), get_trashY(i), get_trash_width(), get_trash_height(), get_trash_bmap(),
				get_vacuum_x(), get_vacuum_y(), get_vacuum_width(), get_vacuum_height(), get_vacuum_bmap())){
					
						erase_trash(i);
						return;
				}
		}
	}
}

/**
 * Determines whether vacuum-slime collisions occurs to erase slime if pump is on
 */
void check_vacuum_slime_collision(){
	if (get_vacuum_mode() == true){
		for (int s = 0; s < get_slime_num(); s++){
			if (pixel_collision(
				get_slimeX(s), get_slimeY(s), get_slime_dim(), get_slime_dim(), get_slime_bmap(), 
				get_vacuum_x(), get_vacuum_y(), get_vacuum_width(), get_vacuum_height(), get_vacuum_bmap())){
					erase_slime(s);
				return;
				}
		}
	}
} 

/**
 * Determines whether vacuum-dust collisions occurs to erase dust if pump is on
 */
void check_vacuum_dust_collision(){
	if (get_vacuum_mode() == true){
		for (int d = 0; d < get_dust_num(); d++){
			if (pixel_collision(
				get_dustX(d), get_dustY(d), get_dust_dim(), get_dust_dim(), get_dust_bmap(),
				get_vacuum_x(), get_vacuum_y(), get_vacuum_width(), get_vacuum_height(), get_vacuum_bmap())){
					erase_dust(d);
					return;
				}
		}
	}
}

/**
 * Runs all the vacuum-rubbish collisions
 */
void check_vacuum_rubbish_collisions(){
	//check if vacuum collides with any trash 
	check_vacuum_trash_collision();
	//check if vacuum collides with any slime 
	check_vacuum_slime_collision();
	//check if vacuum collides with any dust 
	check_vacuum_dust_collision();
	
}