#ifndef piece_h
#define piece_h
 

class Piece 
{
    private:
        int x,y,color,rotation;
    public:
        Piece(int x, int y, int color);
        int get_x();
        int get_y();
        void set_x(int x);
        void set_y(int y);
        int get_color();
        int get_rotation();
        void set_rotation(int rotation);
        virtual void turn_piece(boolean left);
        virtual const unsigned char * get_bitmap();
        virtual int get_bitmap_width();
        virtual int get_bitmap_height();

};


class Rectangle : public Piece
{
    private:
        const unsigned char * bitmap;
        int bitmap_width,bitmap_height;
    public:
        Rectangle(int x, int y, int color);
        const unsigned char * get_bitmap();
        int get_bitmap_width();
        int get_bitmap_height();
        void turn_piece(boolean left);
};


class Square : public Piece
{
    private:
        const unsigned char * bitmap;
        int bitmap_width,bitmap_height;
    public:
        Square(int x, int y, int color);
        const unsigned char * get_bitmap();
        int get_bitmap_width();
        int get_bitmap_height();
        void turn_piece(boolean left);
};

class S : public Piece
{
    private:
        const unsigned char * bitmap;
        int bitmap_width,bitmap_height;
    public:
        S(int x, int y, int color);
        const unsigned char * get_bitmap();
        int get_bitmap_width();
        int get_bitmap_height();
        void turn_piece(boolean left);
};

class Z : public Piece
{
    private:
        const unsigned char * bitmap;
        int bitmap_width,bitmap_height;
    public:
        Z(int x, int y, int color);
        const unsigned char * get_bitmap();
        int get_bitmap_width();
        int get_bitmap_height();
        void turn_piece(boolean left);    
};


class L : public Piece
{
    private:
        const unsigned char * bitmap;
        int bitmap_width,bitmap_height;
    public:
        L(int x, int y, int color);
        const unsigned char * get_bitmap();
        int get_bitmap_width();
        int get_bitmap_height();
        void turn_piece(boolean left);    
};


class J : public Piece
{
    private:
        const unsigned char * bitmap;
        int bitmap_width,bitmap_height;
    public:
        J(int x, int y, int color);
        const unsigned char * get_bitmap();
        int get_bitmap_width();
        int get_bitmap_height();
        void turn_piece(boolean left);    
};

class T : public Piece
{
    private:
        const unsigned char * bitmap;
        int bitmap_width,bitmap_height;
    public:
        T(int x, int y, int color);
        const unsigned char * get_bitmap();
        int get_bitmap_width();
        int get_bitmap_height();
        void turn_piece(boolean left);    
};


#endif






