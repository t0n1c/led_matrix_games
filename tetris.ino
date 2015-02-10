
#include <StandardCplusplus.h>
#include <set>
#include <list>
#include <iterator>
#include <algorithm>

#include <Servo.h> 

#include "Adafruit_GFX.h"
#include "RGBmatrixPanel.h"
#include "piece.h"

#define CLK 11
#define LAT A3
#define OE  9

using namespace std; //bad practice! use std:whatever instead!



RGBmatrixPanel * matrix;
Piece * tetris_piece;
int * matrix_state, * scored_lines;

int W,H,initial_x,initial_y,unit_pieces,offset_height,number_of_scored_lines,moves,servo_pin,offset_width;
uint16_t magenta,blue,cyan,orange,red,yellow,green,white;
float brightness,sensitivity_joystick,difficulty;
char previous_move;
Servo my_servo;
//FLAGS
boolean matrix_rotation,place_tetris_flag,create_piece_flag,read_serial_flag,fill_gaps_flag,
sticky_method;

long previous_millis,default_interval,start_move,chance_interval,current_interval;
unsigned long current_millis;

PointStruct current_unit_pixels_piece [4 * sizeof(PointStruct)];
PointStruct next_unit_pixels_piece[4 * sizeof(PointStruct)];

BoundsStruct p1_joystick_bounds,p2_joystick_bounds;
int p1_joystick_pin_x,p1_joystick_pin_y,p2_joystick_pin_x,p2_joystick_pin_y,joystick_button;


//This prototypes are due to I included a port of uClibc++ for Arduino.
//I had to use with a list of lists of different sizes adding and removing elements
//so I didn't want to deal with memory management.

set<set<int> > get_chunks();
set<int> flood_finding(int current_point,set<int> temp_chunk);
set<set<int> > test_flood();

//TODO hacer generica la funcion game_over y variables meaningfull
//los sets ocupan demasiada memoria teninedop en cuenta que arduino tiene 8 KB 
//por tanto si el matrix_state tiene muchos elemtos ala hora de sacar los "sticky chunks"
//el arduino crashea podemos replantearlo con arrays dinamicos from the scratch pero para 
//hacerlo eficiente podría ser tedioso.

void setup()
{

  /*
   ##############################
   #                            #
   #       INITIALIZATION       #
   #                            #
   ##############################
   
   */

  matrix = new RGBmatrixPanel(A0, A1, A2, CLK, LAT, OE, false);
  matrix -> begin();
  Serial.begin(9600);

  matrix_rotation = true;
  place_tetris_flag = false;
  create_piece_flag = true;
  read_serial_flag = false;
  sticky_method = true;
  fill_gaps_flag = false;

  brightness = 0.5;//> 0.8 f*ck your eyesight!!!!!
  //sensitivity_joystick =  1.0;//joystick accuracy sucks this param is useless
  difficulty = 0.5;

  /* ORIGINAL ATARI COLORS */
  magenta = matrix -> ColorHSV360(300, 100, 100 * brightness,true);//L
  blue = matrix -> ColorHSV360(240, 100, 100 * brightness,true);//square
  cyan = matrix -> ColorHSV360(180, 100, 100 * brightness,true);//S
  orange = matrix -> ColorHSV360(37, 100, 100 * brightness,true);//Z
  red = matrix -> ColorHSV360(0, 100, 100 * brightness,true);//rectangle
  yellow = matrix -> ColorHSV360(60, 100, 100 * brightness,true);//J
  green = matrix -> ColorHSV360(120, 100, 100 * brightness,true);//T
  white = matrix -> ColorHSV360(0,0,100 * brightness,true);//gaps padding

  W = matrix -> width();
  H = matrix -> height();
  unit_pieces = 1;

  servo_pin = 37;
  my_servo.write(100);
  my_servo.attach(servo_pin);
  delay(500);
  
  if (matrix_rotation){
    matrix -> setRotation(3);
    H = matrix -> height();
    W = matrix -> width();
    //my_servo.write(180);
    for (int i = 101; i <= 180 ; i++){
      my_servo.write(i);
      delay(15);
    }
    delay(500);
  }
  my_servo.detach();

  //These measures (H and W) dont need to be the same that real dimension of our matrix
  //H and W are multiples of unit_pieces. Such multiples will be the greatest multiple 
  //of "unit_pieces" less than or equal to H and W respectively. 

  W = (W - W % unit_pieces) / unit_pieces;
  H = (H - H % unit_pieces) / unit_pieces;

  matrix_state = (int*) calloc(W * H ,sizeof(int));
  if (sticky_method){
    scored_lines = (int*) malloc(H * sizeof(int));
  } else {
    scored_lines = (int*) malloc(4 * sizeof(int));
  }
  

  offset_height = matrix -> height() % unit_pieces;
  offset_width = matrix -> width() % unit_pieces;
  initial_y = 0;
  initial_x = W/2 - 2;
  //initial_x = W/2;
  number_of_scored_lines = 0;
  moves = 0;

  previous_millis = 0L;
  default_interval = 600 - difficulty * 600;
  chance_interval = 1000 - difficulty * 700;
  current_interval = default_interval;
  start_move = 0L;
  previous_move = ' ';

  p1_joystick_pin_x = 8;
  p1_joystick_pin_y = 9;
  p2_joystick_pin_x = 10;
  p2_joystick_pin_y = 11;
  joystick_button = 47;
  pinMode(joystick_button, INPUT_PULLUP);
  
  /*p1_joystick_bounds.x_right = (int) ((1023 - 502) * sensitivity_joystick);
  p1_joystick_bounds.y_top = (int)- ((539 - 4) * (sensitivity_joystick));
  p1_joystick_bounds.x_left = (int)- ((502 - 4) * sensitivity_joystick);
  p1_joystick_bounds.y_bottom = (int)((1023 - 539) * sensitivity_joystick);*/
  
  if (fill_gaps_flag){
    fill_gaps();
  }

  randomSeed(analogRead(A5));
  random(7);random(7);
}


void loop()
{

  if (create_piece_flag){
    delete tetris_piece;

    switch (random(7)){
    //switch (0){
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



//This would be better if it was implemented by interupts because you don't know exactly
//when a move'll be made.It works properly though.


  switch(get_move(read_serial_flag)){
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

    case 'H'://hard down
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
      current_millis = 0;
      place_and_tetris();
      create_piece_flag = true;
      place_tetris_flag = false;
      current_interval = default_interval;
      break;

    }

    current_millis = millis();
    if (current_millis - previous_millis > current_interval) {
    //  if (false){
      previous_millis = current_millis;
      tetris_piece -> set_y(tetris_piece -> get_y() + 1);
      if (get_validated_pixels_piece(tetris_piece)){
        drawing_procedure(tetris_piece,true);
        if (place_tetris_flag){
          place_tetris_flag = false;
          current_interval = default_interval;
        }
      } else {
        tetris_piece -> set_y(tetris_piece -> get_y() - 1);
        if (place_tetris_flag){
          //here we place the piece at the current position 
          place_and_tetris();
          create_piece_flag = true;
          place_tetris_flag = false;
          current_interval = default_interval;

        } else {
          current_interval = chance_interval;
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
  int above_scored_line,top;
  int counter = 0;
  boolean last_line = false;
  update_matrix_state();

  if (sticky_method){
    //"based on" flood fill algorithm.This allows chain reaction ¬¬.
    set<set<int> > chunks;
    int temp_x,temp_y,current_point;
    boolean  erase_flag;

    while (check_tetris(&number_of_scored_lines,scored_lines)){
      blink_lines(scored_lines,number_of_scored_lines,2,40);
      clear_tetris_lines(scored_lines,number_of_scored_lines);
      set<set<int> > all_chunks = get_chunks();
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
            
          }
          if (!erase_flag) ++it;
        }

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
                matrix -> drawPixel(temp_x + i,temp_y + j,0);
                matrix -> drawPixel(temp_x + i,temp_y + j + unit_pieces,matrix_state[(*it_2)]);
              }
            }
          }
        }
      }

      number_of_scored_lines = 0;
      //delay(50);
    }

  } else {
    //naive method
    if (check_tetris(&number_of_scored_lines,scored_lines)){
      blink_lines(scored_lines,number_of_scored_lines,2,40);
      clear_tetris_lines(scored_lines,number_of_scored_lines);
      for (int index = 0; index < number_of_scored_lines; index++){
        if (index == number_of_scored_lines - 1)
          last_line = true;
        above_scored_line = scored_lines[number_of_scored_lines - 1 - index] - 1;

        for (int y = above_scored_line; ;y--){
          if (!last_line && y == (scored_lines[number_of_scored_lines - 2 - index] - 1))
            break;
          if (above_scored_line == -1){//It means the first line is a tetris
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
  number_of_scored_lines = 0;
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
      }
      else{
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
    }
    else{
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
    matrix -> fillRect(unit_pieces*i,unit_pieces*line + offset_height,unit_pieces,unit_pieces,matrix_state[W * line + i]);  
  }
}




set<set<int> > get_chunks()
{
  int above_line, current_point, counter;
  boolean last_line = false;
  above_line = scored_lines[number_of_scored_lines - 1] - 1 ;
  set<int> set_points,temp_chunk;
  set<set<int> > res;
  for (int y = above_line + 1;; y--){
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

  //the origin of each piece is the left top corner of the bitmap 

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


void game_over()
{
  int text_x1 = 33;
  int text_x2 = -25;
  char str_1[] = "GAME";
  char str_2[] = "OVER";
  int text_min = sizeof(str_1) * -12;
  int init_servo_degrees = 179;
  int current_servo_degrees = init_servo_degrees;
  
  if (matrix_rotation) {
    matrix -> setRotation(0);
    H = matrix -> height();
    W = matrix -> width();
    my_servo.write(180);
    my_servo.attach(servo_pin);
    delay(500);
  }
  matrix -> setTextWrap(false); // Allow text to run off right edge
  matrix -> setTextSize(1);
  if (matrix_rotation){
    while ( current_servo_degrees >= 100 || text_x1 != 4){
      if (current_servo_degrees % 3 == 0 && text_x1 != 4){
        text_x1--;
        text_x2++;
        print_text(str_1,str_2,text_x1,text_x2);
      }
      if (current_servo_degrees >= 100){
        my_servo.write(current_servo_degrees);
        delay(15);
        current_servo_degrees--;
      }
    }
  } else {
    while (text_x1 != 4){
      text_x1--;
      text_x2++;
      print_text(str_1,str_2,text_x1,text_x2);
    }
  }
  delay(500);
  my_servo.detach();
}


void print_text(char str_1[],char str_2[], int text_x1, int text_x2){

  matrix -> fillScreen(0);
  matrix -> setTextColor(white);
  matrix -> setCursor(text_x1, 0);
  matrix -> print(str_1);
  matrix -> setCursor(text_x2, 8);
  matrix -> print(str_2);
  delay(90);
}



int get_move(boolean read_serial_flag)
{
  if (read_serial_flag){
    //Serial

    if (Serial.available() > 0) {
      return Serial.read();
    }
  }
  else{
    //joystick

    return read_joystick();
  }

}


int read_joystick()
{
  char chosen_move = '\0';
  long joystick_x = analogRead(p1_joystick_pin_x);//- offset_x;//offset to setup to 0
  // this small pause is needed between reading
  // analog pins, otherwise we get the same value twice
  //Serial.println(joystick_x);
  delay(10);        
  //reads the value of the variable resistor 
  long joystick_y = analogRead(p1_joystick_pin_y);// - offset_y;
  
  
  if (joystick_y >= 1020 && joystick_x < 1023 && joystick_x > 0 ){
    chosen_move = 'D';
  } else if (joystick_y <= 3 && joystick_x < 1023 && joystick_x > 0 ){
    chosen_move = 'U';
  } else if (joystick_x <= 3 && joystick_y < 1023 && joystick_y > 0 ){
    chosen_move = 'L';
  } else if (joystick_x >= 1020 && joystick_y < 1023 && joystick_y > 0 ){
    chosen_move = 'R';
  } else if (digitalRead(joystick_button) == LOW){
    //hard down
    previous_move = 'H';
    delay(140);
    return 'H';
  } else {
    previous_move = ' ';
  }

  if (chosen_move == 'U'){
    delay(decrease_delay(chosen_move,180 - difficulty * 90,7));
  } else if (chosen_move != 'H'){
    delay(decrease_delay(chosen_move,110 - difficulty * 70,20));
  }
  
  return chosen_move;


  
  //If joystick had good quality would come in handy the next code
  /*
  float hip = sqrt(joystick_x * joystick_x + joystick_y * joystick_y);
  float sine = joystick_y / hip; 
  float cosine = joystick_x / hip;
  if (joystick_x > joystick_bounds -> x_right && sine < 0.7071 && sine > -0.7071 && cosine > 0.7071 ){
    //RIGHT
    delay(50);

  }
  else if (joystick_y < joystick_bounds -> y_top && sine < -0.7071 && cosine < 0.7071 && cosine > -0.7071){
    //UP
    delay(1);
    return 'U';
  }
  else if (joystick_x < joystick_bounds -> x_left && sine < 0.7071 && sine > -0.7071 && cosine < -0.7071 ){
    //LEFT
    delay(50);
    return 'L';
  }
  else if (joystick_y > joystick_bounds -> y_bottom && sine > 0.7071 && cosine < 0.7071 && cosine > -0.7071 ){
    //DOWN
    delay(50);
    return 'D';
  }*/

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


void print_matrix_state()
{
  for (int i = 0; i < H;i++){
    for (int j = 0; j < W;j++){

      Serial.print("Position -> (x,y): ");
      Serial.print(i);
      Serial.print(",");
      Serial.println(j);
      Serial.print("Value -> ");
      Serial.println(matrix_state[i*W + j]);
    //delay(50);
    }
  }
//while (1){}
}


void fill_gaps()
{
  for (int i = 0;i < matrix -> width();i++){
    for (int j = 0; j < offset_height;j++){
      matrix -> drawPixel(i,j,white);
    }
  }

  for (int j = 0; j < matrix -> height(); j++){
    for (int i = matrix -> width() - offset_width; i < matrix -> width(); i++){
      matrix -> drawPixel(i,j,white);
    }
  }
}


int free_ram () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}




