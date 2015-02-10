#include "piece.h"


static const unsigned char array_bitmap_S[] = {

										0b0000,
										0b0011,
										0b0110,
										0b0000,

										0b0010,
										0b0011,
										0b0001,
										0b0000
										
                       					};


S::S(int x, int y,int color):Piece(x,y,color)
{
	this -> bitmap = array_bitmap_S;
	this -> bitmap_width = 4;
	this -> bitmap_height = 4;

}


const unsigned char * S::get_bitmap()
{
	return this -> bitmap;
}

int S::get_bitmap_width()
{
	return this -> bitmap_width;
}

int S::get_bitmap_height()
{
	return this -> bitmap_height;
}

void S::turn_piece(boolean left)
{

	switch(get_rotation()){

		case 0:
			set_x(get_x() + 1);
			set_y(get_y() - 2); 
			break;

		case 1:
			set_x(get_x() - 1);
			set_y(get_y() + 2);     
			break;
	}

	
	set_rotation((get_rotation() + 1) % 2);
}










