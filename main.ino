#include "tetris.h"

#include "Adafruit_GFX.h"
#include "RGBmatrixPanel.h"

#include <Servo.h> 

#define CLK 11
#define LAT A3
#define OE  9

int selected_game,W,H,unit_pieces,servo_pin,offset_height,offset_width;
RGBmatrixPanel * matrix;
int * matrix_state;
uint16_t magenta,blue,cyan,orange,red,yellow,green,white;

float brightness,sensitivity_joystick,difficulty;

//BoundsStruct p1_joystick_bounds;
//joystick pins
int p1_joystick_x,p1_joystick_y,p1_joystick_button;

Servo my_servo;

boolean matrix_rotation,read_serial_flag,fill_gaps_flag;

void setup()
{
	matrix = new RGBmatrixPanel(A0, A1, A2, CLK, LAT, OE, false);
  matrix -> begin();
  Serial.begin(9600);
	selected_game = 0;  
  brightness = 0.45; // > 0.75 f*ck your eyesight!!!
	//sensitivity_joystick =  1.0; //joystick accuracy sucks this param is useless
	difficulty = 0.5;

	matrix_rotation = true;
	read_serial_flag = false;
	fill_gaps_flag = true;
	
	magenta = color_hsv_360(300, 100, 100 * brightness,true);//L
	blue = color_hsv_360(240, 100, 100 * brightness,true);//square
	cyan = color_hsv_360(180, 100, 100 * brightness,true);//S
	orange = color_hsv_360(37, 100, 100 * brightness,true);//Z
	red = color_hsv_360(0, 100, 100 * brightness,true);//rectangle
	yellow = color_hsv_360(60, 100, 100 * brightness,true);//J
	green = color_hsv_360(120, 100, 100 * brightness,true);//T
	white = color_hsv_360(0,0,100 * brightness,true);//gaps padding



	W = matrix -> width();
	H = matrix -> height();
	unit_pieces = 2;

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

  offset_height = matrix -> height() % unit_pieces;
  offset_width = matrix -> width() % unit_pieces;
 
  /*matrix_state is not neccessary if you know how to deal with the buffermatrix provided by adafruit (you'd save memory space!!)
  matrix_state is more intuitive to deal with. buffermatrix is a low level matrix state,we could build a wrapper though, but 
  you should understand how this matrix works under the hood. Actually is not very hard but is a pain in the as* if you are a hardware agnostic and 
  you just want to code a led matrix and see how cool it is.
  ANYHOW the core that manages this matrix doesn't provide anyway to access to buffermatrix from outside by default. Just a function for dumping
  the matrix content to serial port at one point.*/
  
  matrix_state = (int*) calloc(W * H ,sizeof(int));

	p1_joystick_x = 8;
  p1_joystick_y = 9;
  p1_joystick_button = 21;
  pinMode(p1_joystick_button, INPUT_PULLUP);
  
  /*p1_joystick_bounds.x_right = (int) ((1023 - 502) * sensitivity_joystick);
  p1_joystick_bounds.y_top = (int)- ((539 - 4) * (sensitivity_joystick));
  p1_joystick_bounds.x_left = (int)- ((502 - 4) * sensitivity_joystick);
  p1_joystick_bounds.y_bottom = (int)((1023 - 539) * sensitivity_joystick);*/
  randomSeed(analogRead(A5));
  
  if (selected_game == 0){
		tetris_setup();
	}else if (selected_game == 1){
		stack_setup();
	}
  if (fill_gaps_flag){
    fill_gaps();
  }
}


void loop()
{
	if (selected_game == 0){
		tetris_loop();
	}else if (selected_game == 1){
		stack_loop();
	}


}


int get_move()
{
  if (read_serial_flag){
  //Serial
    return read_serial();

  } else {
  //joystick
    return read_joystick();
  }

}


int read_serial()
{
	if (Serial.available() > 0)
      return Serial.read();
}


int read_joystick()
{
  char chosen_move = '\0';
  long joystick_x = analogRead(p1_joystick_x);//- offset_x;//offset to setup to 0
  // this small pause is needed between reading
  // analog pins, otherwise we get the same value twice
  //Serial.println(joystick_x);
  delay(10);        
  //reads the value of the variable resistor 
  long joystick_y = analogRead(p1_joystick_y);// - offset_y;
  
  
  if (joystick_y >= 1020 && joystick_x < 1023 && joystick_x > 0 ){
    chosen_move = 'D';
  } else if (joystick_y <= 3 && joystick_x < 1023 && joystick_x > 0 ){
    chosen_move = 'U';
  } else if (joystick_x <= 3 && joystick_y < 1023 && joystick_y > 0 ){
    chosen_move = 'L';
  } else if (joystick_x >= 1020 && joystick_y < 1023 && joystick_y > 0 ){
    chosen_move = 'R';
  } else if (digitalRead(p1_joystick_button) == LOW){
    chosen_move = 'P';
    delay(140);
  } else {
    chosen_move = ' ';
  }

  return chosen_move;

	//If dx joystick had good quality would come in handy the next code
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

uint16_t color_hsv_360(long hue, uint8_t sat, uint8_t val, boolean gflag){
  //this function is acting as wrapper in order to work with colors easier. Maybe It would be better if it was placed 
  //on RGBmatrixPanel class. It has been placed here to avoid compability problems with the library provided by 
  //Adafruit.(https://learn.adafruit.com/32x16-32x32-rgb-led-matrix/downloads)

  hue %= 360;             
  if(hue < 0) hue += 360;
  long h = round(hue * (1536/360.0));
  sat %= 101;             
  if(sat < 0) sat += 100;
  long s = round(255 * (sat/100.0));
  val %= 101;             
  if(val < 0) val += 100;
  long v = round(255 * (val/100.0));
  return matrix -> ColorHSV(h,s,v,gflag);


}

int free_ram () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
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
    }
  }
//while (1){}
}

uint16_t select_random_color()
{
  uint16_t colors_list[] = {magenta,blue,cyan,orange,red,yellow,green,white};
  return colors_list[random(8)];
}




