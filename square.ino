#include "piece.h"


static const unsigned char array_bitmap_square[] = {

				                    	0b0000,
			                            0b0110,
			                            0b0110,
			                            0b0000
										
                       					};


Square::Square(int x, int y,int color):Piece(x,y,color)
{
	this -> bitmap = array_bitmap_square;
	this -> bitmap_width = 4;
	this -> bitmap_height = 4;

}


const unsigned char * Square::get_bitmap()
{
	return this -> bitmap;
}

int Square::get_bitmap_width()
{
	return this -> bitmap_width;
}

int Square::get_bitmap_height()
{
	return this -> bitmap_height;
}

void Square::turn_piece(boolean left)
{
//Do nothing...because rotating a square is "useful" ...the result is the same
}











