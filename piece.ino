#include "piece.h"




Piece::Piece(int x, int y, int color)
{
  this -> x = abs(x);
  this -> y = abs(y);
  this -> color = color;
  this -> rotation = 0;
  
}

int Piece::get_color()
{
  return this -> color;
}

int Piece::get_x()
{
  return this -> x;
}

int Piece::get_y()
{
  return this -> y;
}

void Piece::set_x(int x)
{
  this -> x = x;

}

void Piece::set_y(int y)
{
  this -> y = y;
}

int Piece::get_rotation()
{
  return this -> rotation;
}

void Piece::set_rotation(int rotation)
{
  this -> rotation = rotation;
}


void Piece::turn_piece(boolean left)
{
  return;
}

const unsigned char * Piece::get_bitmap()
{
  unsigned char res = ' ';
  return &res;
}

int Piece::get_bitmap_width()
{
  return -1;
}

int Piece::get_bitmap_height()
{
  return -1;
}


