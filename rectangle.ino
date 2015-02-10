#include "piece.h"


static const unsigned char array_bitmap_rectangle[] = {

										0b0000,
										0b1111,
										0b0000,
										0b0000,

										0b0010,
										0b0010,
										0b0010,
										0b0010
										
                       					};


Rectangle::Rectangle(int x, int y,int color):Piece(x,y,color)
{
	this -> bitmap = array_bitmap_rectangle;
	this -> bitmap_width = 4;
	this -> bitmap_height = 4;

}


const unsigned char * Rectangle::get_bitmap()
{
	return this -> bitmap;
}

int Rectangle::get_bitmap_width()
{
	return this -> bitmap_width;
}

int Rectangle::get_bitmap_height()
{
	return this -> bitmap_height;
}

void Rectangle::turn_piece(boolean left)
{
	//The number of possible rotations of this particular piece is 2
	//therefore don't matter if "left" flag is high or low 
	//the effect'll be the same.
   	switch(get_rotation()){
        
        case 0:
			set_x(get_x() + 2);
			set_y(get_y() - 1);
			break;

        case 1:
        	set_x(get_x() - 2);
          	set_y(get_y() + 1);
          	break;
    }
	
	set_rotation((get_rotation() + 1) % 2);

}




