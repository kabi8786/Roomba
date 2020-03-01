#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
#include "helpers.h"
#include "vacuum.h"

void draw_trash(int);
void draw_slime(int);
void draw_dust(int);

void draw_all_trash();
void draw_all_slime();
void draw_all_dust();
void draw_all_rubbish();

void trash_coords(int);
void slime_coords(int);
void dust_coords(int);

void erase_trash(int);
void erase_slime(int);
void erase_dust(int);


void trash_collision(int, int);
void setup_trash();
void setup_slime();
void setup_dust();

void setup_rubbish();

void create_trash();
void create_slime();
void create_dust();

int get_trashX(int);
int get_trashY(int);
int get_trash_height();
int get_trash_width();
int get_trash_num();
char * get_trash_bmap();

int get_numtrash_display();
void set_numtrash_display(int);
int get_numslime_display();
void set_numslime_display(int);

int get_slime_num();
int get_slimeX(int);
int get_slimeY(int);
char * get_slime_bmap();
int get_slime_dim();


int get_dust_num();
int get_dustX(int);
int get_dustY(int);
int get_dust_dim();
void set_numdust_display(int);
int get_numdust_display();
char * get_dust_bmap(); 


