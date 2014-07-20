//model

#ifndef CUBE_CUBE_H_
#define CUBE_CUBE_H_

#include <cstdio>

class Block
{
public:
	Block()
	{
		front_	=	NULL;
		back_	=	NULL;
		right_	=	NULL;
		left_	=	NULL;
		top_	=	NULL;
		bottom_ =	NULL;
	}
	Block(int * front, int* back, int* right, int*left, int*top, int*bottom)
	{
		front_	=	front;
		back_	=	back;
		right_	=	right;
		left_	=	left;
		top_	=	top;
		bottom_	=	bottom;
	}
	~Block()
	{
	}
	int* front_;
	int* back_;
	int* right_;
	int* left_;
	int* top_;
	int* bottom_;

};


class Cube
{
public:
	enum Orientation
	{
		NONE=0,
		//may be right hand?
		FRONT_BACK=1,//look from front to back, time wise
		RIGHT_LEFT=2,
		TOP_BOTTOM=3,
	};
	Cube(int n);	//n < (2^31-1)^(1/3)
	~Cube(void);
	void ResetColor();
	void Rotate(Orientation o, int id,int times);//id stand for order of the piece, count accroding the Orientation 
	static void RotateSquare(int* a,int n,int times);
	int n() const
	{
		return n_;
	}
	int* front() 
	{
		return front_;
	}
	int* back() 
	{
		return back_;
	}
	int* right() 
	{
		return right_;
	}
	int* left() 
	{
		return left_;
	}
	int* top() 
	{
		return top_;
	}
	int* bottom() 
	{
		return bottom_;
	}
	const Block *blocks() const
	{
		return blocks_;
	}

private:
	const int n_;

	int* front_;
	int* back_;
	int* right_;
	int* left_;
	int* top_;
	int* bottom_;
	Block *blocks_;


};

#endif