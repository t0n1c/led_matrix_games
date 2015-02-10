#include "piece.h"


static const unsigned char array_bitmap_J[] = {

										0b0000,
										0b0111,
										0b0001,
										0b0000,

										0b0011,
										0b0010,
										0b0010,
										0b0000,

										0b0100,
										0b0111,
										0b0000, 
										0b0000,

										0b0010,
										0b0010,
										0b0110,
										0b0000,
										
                       					};


J::J(int x, int y,int color):Piece(x,y,color)
{
	this -> bitmap = array_bitmap_J;
	this -> bitmap_width = 4;
	this -> bitmap_height = 4;

}


const unsigned char * J::get_bitmap()
{
	return this -> bitmap;
}

int J::get_bitmap_width()
{
	return this -> bitmap_width;
}

int J::get_bitmap_height()
{
	return this -> bitmap_height;
}

void J::turn_piece(boolean left)
{

	switch(get_rotation()){
			case 0:
				if (left){
					set_x(get_x() + 1);
					set_y(get_y() - 1);
				}else{
					set_y(get_y() + 1);
				}
				
				break;
			case 1:
				if(!left){
					set_y(get_y() + 1);
				}
				set_x(get_x() - 1);
				break;

			case 2:
				if(left){
					set_y(get_y() + 2);     
				}else{
					set_x(get_x() + 1);
				}
				
				break;
			case 3:
				if(left){
					set_y(get_y() - 1);
				}else{
					set_y(get_y() - 2);
				}
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










