#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
#include "helpers.h"
#include "vacuum.h"
#include "trash.h"

void check_base_vacuum_collision();
bool opaque(int, int, int, int, int, int, char[]);
bool pixel_collision(int, int, int, int, char[], int, int, int, int, char[]);

void check_vacuum_trash_collision();
void check_vacuum_slime_collision();
void check_vacuum_rubbish_collisions();
void check_vacuum_dust_collision();

bool check_trash_made_collision(int);
bool check_trash_setup_vacuum_collision(int);
bool check_trash_setup_base_collision(int);
bool check_trash_setup_collisions(int, int);

bool check_slime_setup_vacuum_collision(int);
bool slime_setup_base_collision(int);
bool check_slime_setup_collisions(int, int); 

bool check_dust_setup_collisions(int, int);
bool check_dust_setup_base_collisions(int);
bool check_dust_setup_vacuum_collisions(int);