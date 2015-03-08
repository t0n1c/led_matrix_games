
#ifndef TETRIS_H_  
#define TETRIS_H_

#include <StandardCplusplus.h>
#include <set>
#include "piece.h"

typedef struct{
   int x;
   int y;
}PointStruct;


/*typedef struct{
   int x_right;
   int x_left;
   int y_top;
   int y_bottom;
}BoundsStruct;
*/

using namespace std;


void tetris_setup();
void tetris_loop();
void drawing_procedure(Piece * p,boolean clear_piece);
void place_and_tetris();
void naive_tetris();
void sticky_tetris();
void level_up();
boolean check_tetris(int * number_of_scored_lines,int * scored_lines);
void clear_tetris_lines(int * scored_lines,int number_of_scored_lines);
void blink_lines(int * scored_lines,int number_of_scored_lines,int number_of_blinks,int ms);
void draw_line(int line);
set<set<int> > get_chunks();
set<set<int> > test_flood();
set<int> flood_finding(int current_point,set<int> chunk);
void update_matrix_state();
boolean get_validated_pixels_piece(Piece * p);
boolean check_unit_piece_pixel(int relocated_x,int relocated_y);
void draw_piece(int color);
int decrease_delay(char chosen_move,int init_delay,int decrease);

//This prototypes are due to I included a port of uClibc++ for Arduino.
//I had to use with a list of lists of different sizes adding and removing elements
//so I didn't want to deal with memory management.

set<set<int> > get_chunks();
set<int> flood_finding(int current_point,set<int> temp_chunk);
set<set<int> > test_flood();

#endif





