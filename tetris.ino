#include <StandardCplusplus.h>
#include <set>
#include <list>
#include <iterator>
#include <algorithm>

#include "tetris.h"

using namespace std; //bad practice...use std:whatever instead!

Piece * tetris_piece;
int * scored_lines;

int initial_x,initial_y,number_of_scored_lines,moves,lines,total_lines;

char previous_move,chosen_move;

//FLAGS
boolean place_tetris_flag,create_piece_flag,sticky_method;

long previous_millis,default_interval,start_move,chance_interval,current_interval;
unsigned long current_millis;

PointStruct current_unit_pixels_piece[4 * sizeof(PointStruct)];
PointStruct next_unit_pixels_piece[4 * sizeof(PointStruct)];


void tetris_setup()
{

  place_tetris_flag = false;
  create_piece_flag = true;
  sticky_method = false;
  
  
  if (sticky_method){
    scored_lines = (int*) malloc(H * sizeof(int));
    } else {
      scored_lines = (int*) malloc(4 * sizeof(int));
    }

  initial_y = 0;
  initial_x = W/2 - 2;
  number_of_scored_lines = 0;
  moves = 0;
  total_lines = 0;
  previous_millis = 0L;
  default_interval = 9600/H - (difficulty * (8000/H));
  chance_interval = 1000 - difficulty * 650;
  current_interval = default_interval;
  start_move = 0L;
  previous_move = ' ';

}



void tetris_loop()
{


  if (create_piece_flag){
    delete tetris_piece;

    switch (random(7)){
    //switch (4){
      
      /* 
      ORIGINAL ATARI COLORS 
      magenta -> L
      blue -> square
      cyan -> S
      orange -> Z
      red -> rectangle
      yellow -> J
      green -> T
      white -> gaps padding
      */

      case 0:
      tetris_piece = new Rectangle(initial_x,initial_y,red);
      break;

      case 1:
      tetris_piece = new Square(initial_x,initial_y,blue);
      break;

      case 2:
      tetris_piece = new S(initial_x,initial_y+1,cyan);
      break;

      case 3:
      tetris_piece = new Z(initial_x,initial_y,orange);
      break;

      case 4:
      tetris_piece = new L(initial_x,initial_y,magenta);
      break;

      case 5:
      tetris_piece = new J(initial_x,initial_y,yellow);
      break;

      case 6:
      tetris_piece = new T(initial_x,initial_y,green);
      break;

    }

    if (get_validated_pixels_piece(tetris_piece)){
      drawing_procedure(tetris_piece,false);
      create_piece_flag = false;
      } else {
      //YOU LOSE!!!
      int all_lines[W * H];
      for (int i=0;i<H;i++) all_lines[i] = i;
      blink_lines(all_lines,H,3,256);
      game_over();
      while(1){}
      

      
    }
    
  }

  //This would be better if it was implemented by interrupts ,specially the "hard down" move (which is on digital)
  //because interrupts are attached on digital pins by default.It works properly though.

  chosen_move = get_move();
  if (chosen_move == 'U'){
    delay(decrease_delay(chosen_move,180,7));
    //delay(180);
  } else if(chosen_move == 'P'){
    previous_move = 'P';
  }else if (chosen_move == ' '){
    previous_move = ' ';
  }else {
      delay(decrease_delay(chosen_move,130 - difficulty * 70,20));
  }
  


  switch(chosen_move){
    case 'U':
      tetris_piece -> turn_piece(true);
      if (get_validated_pixels_piece(tetris_piece)){
        drawing_procedure(tetris_piece,true);
        } else {
          tetris_piece -> turn_piece(false);
        }
        break;

    case 'L':
      tetris_piece -> set_x(tetris_piece -> get_x() - 1);
      if (get_validated_pixels_piece(tetris_piece)){
        drawing_procedure(tetris_piece,true);
      } else {
          tetris_piece -> set_x(tetris_piece -> get_x() + 1);
      }
        break;

    case 'R':
      tetris_piece -> set_x(tetris_piece -> get_x() + 1);
      if (get_validated_pixels_piece(tetris_piece)){
        drawing_procedure(tetris_piece,true);
        } else {
          tetris_piece -> set_x(tetris_piece -> get_x() - 1);
        }
        break;

    case 'D':
      tetris_piece -> set_y(tetris_piece -> get_y() + 1);
      if (get_validated_pixels_piece(tetris_piece)){
        drawing_procedure(tetris_piece,true);
        } else {
          tetris_piece -> set_y(tetris_piece -> get_y() - 1);
        }    

        break;

    case 'P'://hard down
      while (1){
        tetris_piece -> set_y(tetris_piece -> get_y() + 1);
        if (get_validated_pixels_piece(tetris_piece)){
          drawing_procedure(tetris_piece,true);
        } else {
          tetris_piece -> set_y(tetris_piece -> get_y() - 1);
          break;
        }
          //delay(10);
      }
      place_and_tetris();
      create_piece_flag = true;
      place_tetris_flag = false;
      current_interval = default_interval;
      break;
  }

  current_millis = millis();
  if (!create_piece_flag && current_millis - previous_millis > current_interval) {
  //if (false){
    previous_millis = current_millis;
    tetris_piece -> set_y(tetris_piece -> get_y() + 1);
    if (get_validated_pixels_piece(tetris_piece)){
      drawing_procedure(tetris_piece,true);
      if (place_tetris_flag){
        place_tetris_flag = false;
        //current_interval = default_interval;
      }
    } else {
      tetris_piece -> set_y(tetris_piece -> get_y() - 1);
      if (place_tetris_flag){
        //here we place the piece at the current position 
        place_and_tetris();
        create_piece_flag = true;
        place_tetris_flag = false;
        //current_interval = default_interval;

      } else {
        //current_interval = chance_interval;
        place_tetris_flag = true;
      }  
    }
  }
}




void drawing_procedure(Piece * p,boolean clear_piece)
{
  if (clear_piece){
    draw_piece(0);
  }
  memcpy(current_unit_pixels_piece, next_unit_pixels_piece,4 * sizeof(PointStruct));
  draw_piece(p -> get_color());
}


void place_and_tetris()
{

  update_matrix_state();
  while (check_tetris(&number_of_scored_lines,scored_lines)){
    if (sticky_method){
      sticky_tetris();  
    } else {
      naive_tetris();
    }
    total_lines += number_of_scored_lines;
    if (total_lines % 20 == 0){
      level_up();
    }
    number_of_scored_lines = 0;
  }
}


void sticky_tetris()
{
  //"based on" flood fill algorithm.This allows chain reaction ¬¬.
  set<set<int> > chunks;
  int temp_x,temp_y,current_point;
  boolean  erase_flag;
  blink_lines(scored_lines,number_of_scored_lines,2,40);
  clear_tetris_lines(scored_lines,number_of_scored_lines);
  set<set<int> > all_chunks = get_chunks();
  //Serial.println(free_ram());
  while (!all_chunks.empty()){

    for (set<set<int> >::iterator it = all_chunks.begin(); it != all_chunks.end();) {
      erase_flag = false;
      for (set<int>::iterator it_2 = it -> begin(); it_2 !=it -> end();++it_2){
        if (it -> find((*it_2) + W) == it -> end()){
          if ((*it_2) + W >= W*H || matrix_state[(*it_2) + W] != 0){
            all_chunks.erase(it++);
            erase_flag = true;
            break;
          }
        }
        //Serial.println((*it_2));
        

      }
      //Serial.println("other chunk");
      if (!erase_flag) ++it;
    }
    //delay(20000);

    //update matrix state
    for (set<set<int> >::iterator it = all_chunks.begin(); it != all_chunks.end(); ++it) {
      for (set<int>::reverse_iterator it_2 = it -> rbegin(); it_2 !=it -> rend();++it_2){
        current_point = *it_2;
        temp_x = current_point % W;
        temp_y = (current_point - temp_x) / W;
        temp_x *= unit_pieces;
        temp_y *= unit_pieces;
        matrix_state[current_point + W] = matrix_state[current_point];
        matrix_state[current_point] = 0;
        (*it_2) = (*it_2) + W;

        for (int i = 0; i < unit_pieces; i++){
          for (int j = 0; j < unit_pieces; j++){
            matrix -> drawPixel(temp_x + i,temp_y + j + offset_height,0);
            matrix -> drawPixel(temp_x + i,temp_y + j + unit_pieces + offset_height,matrix_state[(*it_2)]);
          }
        }
      }
    }
  }

  //number_of_scored_lines = 0;
  //delay(50);
    //while(Serial.read() != 'S'){}
}


void naive_tetris()
{
  int above_scored_line,top;
  int counter = 0;
  boolean last_line = false;
  blink_lines(scored_lines,number_of_scored_lines,2,40);
  clear_tetris_lines(scored_lines,number_of_scored_lines);
  for (int index = 0; index < number_of_scored_lines; index++){
    if (index == number_of_scored_lines - 1)
      last_line = true;
    above_scored_line = scored_lines[number_of_scored_lines - 1 - index] - 1;

    for (int y = above_scored_line; ;y--){
      if (!last_line && y == (scored_lines[number_of_scored_lines - 2 - index] - 1))
        break;
      if (above_scored_line == -1){ //It means the first line is a tetris
        top = 0;
        break;
      } else {
        for (int x = 0; x < W; x++){
          matrix_state[(y + (1 + index))*W + x] = matrix_state[y*W + x];
          if (last_line && matrix_state[y*W + x] == 0)
            counter++;
          matrix_state[y*W + x] = 0;

        }

      }

      if (counter == W){
        top = y - 1;
        break;
      } else { 
        counter = 0;
      }
    }
  }

  //print_matrix_state();

  for (int y = scored_lines[number_of_scored_lines - 1];y>=top; y--){
    draw_line(y);
  }
}


boolean check_tetris(int * number_of_scored_lines,int * scored_lines)//int * scored_lines
{

  int counter = 0;
  for (int y = 0;y < H;y++){
    counter = 0;
    for (int x = 0;x < W;x++){
      if (matrix_state[y * W + x] != 0){
        counter++;
      } else {
        break;
      }
    }
    if (counter == W){
      scored_lines[*number_of_scored_lines] = y;
      (*number_of_scored_lines)++;
    }
  }
  //Serial.println(*number_of_scored_lines);
  return *number_of_scored_lines > 0;
}


void clear_tetris_lines(int * scored_lines,int number_of_scored_lines)
{
  for (int index = 0; index < number_of_scored_lines ;index++) {
    matrix -> fillRect(0,scored_lines[index] * unit_pieces + offset_height,W * unit_pieces,unit_pieces,0);
  }
  for (int index = 0; index < number_of_scored_lines; index++){
    for (int x = 0; x < W; x++){
      matrix_state[scored_lines[index] * W + x] = 0;
    }
  }
}

void blink_lines(int * scored_lines,int number_of_scored_lines,int number_of_blinks,int ms)
{
  //lines blinks number_of_blinks times and therefore nunmber_of_blinks*2 iterations
  int iter = 0;
  while (iter < number_of_blinks*2){

    if (iter % 2 == 0){
      for (int index = 0; index < number_of_scored_lines ;index++) {
        matrix -> fillRect(0,scored_lines[index] * unit_pieces + offset_height,W * unit_pieces,unit_pieces,0);
      }
    }else{
      for (int index = 0; index < number_of_scored_lines ;index++){
        draw_line(scored_lines[index]);
      }
    }
    delay(ms);
    iter++;
  }

}


void draw_line(int line)
{
  //print_matrix_state();
  for (int i = 0; i < W ;i++){
    matrix -> fillRect(unit_pieces * i,unit_pieces * line + offset_height,unit_pieces,unit_pieces,matrix_state[W * line + i]);  
  }
}


set<set<int> > get_chunks()
{
  int current_point, counter;
  boolean last_line = false;
  set<int> set_points,temp_chunk;
  set<set<int> > res;

  for (int y = scored_lines[number_of_scored_lines - 1] + 1;; y--){
    counter = 0;
    if (y == scored_lines[0] - 1){
      last_line = true;
    }
    if (y == -1) break;

    for (int x = 0; x < W; x++){
      current_point = y*W + x;
      if (matrix_state[current_point] == 0){
        counter++;
        } else {
          set_points.insert(current_point);

        }
      }

      if (last_line && counter == W) break;
    }

  //Let's get chunksssssss
  //inser,erase and find complexity log in size.
  set<int>::iterator it = set_points.begin();
  while (it != set_points.end()){
    temp_chunk = flood_finding(*it,temp_chunk);
    res.insert(temp_chunk);
    //Serial.println(freeRam());

    temp_chunk.erase(*(it));
    for (set<int>::iterator it_1 = temp_chunk.begin(); it_1 != temp_chunk.end(); ++it_1){
      set_points.erase(*(it_1));
    }
    set_points.erase(it++);
    temp_chunk.clear();
    //Serial.println("I'm here");
  }
  return res;
}


set<set<int> > test_flood()
{
  set<set<int> > res;
  set<int> set_points,temp_chunk;
  for (int y = 0; y < H; y++){
    for (int x = 0; x < W; x++){
      if (matrix_state[y*W + x] != 0){
        set_points.insert(y*W + x);
      }

    }
  }

  set<int>::iterator it = set_points.begin();
  while (it != set_points.end()){
    /*Serial.print("Punto actual -> ");
    Serial.println(*it);*/
    temp_chunk = flood_finding(*it,temp_chunk);
    res.insert(temp_chunk);
    temp_chunk.erase(*(it));
    for (set<int>::iterator it_1 = temp_chunk.begin(); it_1 != temp_chunk.end(); ++it_1){
      set_points.erase(*(it_1));
    }
    set_points.erase(it++);
    /*Serial.print("Tam set_points after -> ");
    Serial.println(set_points.size());
    delay(5000);*/
    temp_chunk.clear();
    //if (set_points.size() == 0) break;
  }

  return res;
}


set<int> flood_finding(int current_point,set<int> chunk){

  list<int> list_chunks;
  chunk.insert(current_point);
  list_chunks.push_back(current_point);
  //Serial.println("current_point");
  //Serial.println(current_point);
  int west,east,south,north;

  for (list<int>::iterator it = list_chunks.begin(); it != list_chunks.end(); ++it){
    //move to the left
    for (west = *it - 1; matrix_state[west] != 0 && west % W != W - 1 && west >= 0; west--){
      chunk.insert(west);
    }
    west++;
    //move to the right
    for (east = *it + 1; matrix_state[east] != 0 && east % W != 0 && east < H*W ;east++){
      chunk.insert(east);
    }
    east--;
    /*Serial.print("West -> ");
    Serial.println(west);
    Serial.print("East -> ");
    Serial.println(east);*/

    for (int index = west; index <= east;index++){
      north = index - W;
      if (north >= 0 && matrix_state[north] != 0 && chunk.find(north) == chunk.end()){
        chunk.insert(north);
        list_chunks.push_back(north);
      }
      south = index + W;
      if (south < W*H && matrix_state[south] != 0 && chunk.find(south) == chunk.end()){
        chunk.insert(south);
        list_chunks.push_back(south);
      }
    }
    list_chunks.pop_front();
  }
  return chunk;

}


void update_matrix_state()
{
  //Serial.println("###################################################################");
  int x,y;
  for (int i = 0;i < 4;i++){
    x  = current_unit_pixels_piece[i].x;
    y  = current_unit_pixels_piece[i].y;
      /*Serial.println("Pares x y");
       Serial.println(x);    Serial.println(y);
       Serial.println("Posicion done se guardara");
       Serial.println(((y * W) / (unit_pieces * unit_pieces)) + (x/unit_pieces));
       Serial.println("Color");    
       Serial.println(tetris_piece -> get_color());*/
       matrix_state[x + y * W] = tetris_piece -> get_color();
     }

   }



boolean get_validated_pixels_piece(Piece * p)
{
  //Note: return false if there was a non-acceptable pixel.
  int width = p -> get_bitmap_width();
  int height = p -> get_bitmap_height();
  const unsigned char * bitmap = p -> get_bitmap();
  uint8_t shift = width - 1;
  int first_i = 0,first_j = 0;
  boolean first_time = true;
  int aux_index = 0;
  uint8_t b;

  //the origin of each piece is the left top corner

  for (uint8_t j=0;j<width;j++) {
    for (uint8_t i=0;i<height;i++) {

      b = bitmap[p -> get_rotation() * height + i];
        if ((b >> shift) & 0x1) {//if we find a one then...
          if (first_time){
            first_time = false;
            first_i = i;
            first_j = j;
          }

          int relocated_x = p -> get_x() + (j - first_j);
          int relocated_y = p -> get_y() + (i - first_i);

          next_unit_pixels_piece[aux_index] = {.x = relocated_x ,.y = relocated_y};
          //Remember it's NOT necessary check every single pixel because our pixel is an "unit_piece" pixel.
          if (!check_unit_piece_pixel(relocated_x,relocated_y)){
            return false;
          }
          aux_index++;

        }


      }
      shift--;
    }
  //now we are sure that coordinates are addmissible and therefore we can draw them.
  return true;
}


boolean check_unit_piece_pixel(int relocated_x,int relocated_y)
{

  if (relocated_x >= 0 && relocated_x < W && relocated_y >= 0 && relocated_y < H && matrix_state[relocated_x + relocated_y * W] == 0)
    return true;
  else
    return false;
}


void draw_piece(int color)
{
  for (int sub_piece = 0;sub_piece < 4;sub_piece++){
    int x = current_unit_pixels_piece[sub_piece].x;
    int y = current_unit_pixels_piece[sub_piece].y;

    for (int i = 0;i < unit_pieces;i++){
      for (int j = 0;j < unit_pieces;j++){
        matrix -> drawPixel(x * unit_pieces + j,y * unit_pieces + i + offset_height,color);
      }
    }

  }

}


int decrease_delay(char chosen_move,int init_delay,int decrease){
  if (previous_move == chosen_move){
    moves++;
    if (init_delay - decrease*moves < 0)
    moves--;
    } else {
      moves = 0;
    }
    previous_move = chosen_move;
    return init_delay - decrease*moves;
}



void level_up()
{
  if (difficulty < 1.0){
    difficulty += 0.05;
    default_interval = 9600/H - (difficulty * (8000/H));
    chance_interval = 1000 - difficulty * 650;
    current_interval = default_interval;
  }
}





