#include "piece.h"


static const unsigned char array_bitmap_T[] = {

										0b0000,
										0b0111,
										0b0010,
										0b0000,

										0b0010,
										0b0011,
										0b0010,
										0b0000,

										0b0010,
										0b0111,
										0b0000,
										0b0000,

										0b0010,
										0b0110,
										0b0010,
										0b0000
										
                       					};


T::T(int x, int y,int color):Piece(x,y,color)
{
	this -> bitmap = array_bitmap_T;
	this -> bitmap_width = 4;
	this -> bitmap_height = 4;

}


const unsigned char * T::get_bitmap()
{
	return this -> bitmap;
}

int T::get_bitmap_width()
{
	return this -> bitmap_width;
}

int T::get_bitmap_height()
{
	return this -> bitmap_height;
}

void T::turn_piece(boolean left)
{
	
	switch(get_rotation()){

		case 0:
			if (left){
				set_x(get_x() + 1);
				set_y(get_y() - 1);
			}
			break;

		case 1:
			set_x(get_x() - 1);
			set_y(get_y() + 1);
			break;

		case 2:
			if (!left){
				set_x(get_x() + 1);
				set_y(get_y() - 1);
			}
			break;
		case 3:
			//Do nothing
			break;

	}

	
	if(left){
		set_rotation((get_rotation() + 1) % 4);
	}else{
		if(get_rotation() == 0){
			set_rotation((get_rotation() + 3) % 4);
		}else{
			set_rotation(abs(get_rotation() - 1) % 4);
		}
		
	}
}










