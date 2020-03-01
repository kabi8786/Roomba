#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
#include "helpers.h"

double get_vacuum_x();
void set_vacuum_x(double);

double get_vacuum_y();
void set_vacuum_y(double);

int get_vacuum_angle();
void set_vacuum_angle(int);

int get_battery();
void set_battery(int);

void set_max_battery(int);

int get_load();
void set_load(int);

int get_vacuum_height();
int get_vacuum_width();
char* get_vacuum_bmap();

double get_baseX();
double get_baseY();
int get_base_height();
int get_base_width();
char * get_base_bmap();

bool get_vacuum_mode();
void set_vacuum_mode(bool);

void draw_vacuum();
void draw_base();

void setup_vacuum();
void update_battery(double, double);
void update_direction();
void update_vacuum(bool);
void stationary_vacuum();

void move_vacuum_to_base();

void do_battery_cheat();
bool is_vacuum_ctrl(int);
void move_vacuum(int);
void do_vacuum_cheat();




