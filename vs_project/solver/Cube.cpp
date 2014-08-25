#include "Cube.h"
#include "common.h"
#include <cassert>

Cube::Cube(int n):n_(n)
{
	int n_square = n*n;
	int n_cubic	 = n_square*n;
	front_	=	new int[n_square];
	back_	=	new int[n_square];
	right_	=	new int[n_square];
	left_	=	new int[n_square];
	top_	=	new int[n_square];
	bottom_	=	new int[n_square];
	blocks_	=	new Block[n_cubic];
	
	
	//construct the link between blocks and surfaces

	int step = n_;
	int start = n_-1;
	//front
	for(int i=0;i<n_;i++)
	{
		for(int j=0;j<n_;j++)
		{
			//blocks_[(i*n+j)*n+(n-1)]->front_ = &(front_[i*n+j]);
			blocks_[start].front_ = &(front_[i*n+j]);
			start+=step;
		}
	}
	start = 0;
	//back
	for(int i=0;i<n_;i++)
	{
		for(int j=0;j<n_;j++)
		{
			//blocks_[(i*n+j)*n]->back_ = &(back_[i*n+j]);
			blocks_[start].back_ = &(back_[i*n+j]);
			start+=step;
		}
	}
	//right
	step = 1;
	start = n_cubic-n_square;
	for(int i=0;i<n_;i++)
	{
		for(int j=0;j<n_;j++)
		{
			//blocks_[n_cubic-n_square+i*n+j]->right_=&(right_[i*n+j]);
			blocks_[start].right_=&(right_[i*n+j]);
			start+=step;
		}
	}
	//left
	start = 0;
	for(int i=0;i<n_;i++)
	{
		for(int j=0;j<n_;j++)
		{
			//blocks_[i*n+j]->left_=&(left_[i*n+j]);
			blocks_[start].left_=&(left_[i*n+j]);
			start+=step;
		}
	}
	//top
	for(int i=0;i<n_;i++)
	{
		for(int j=0;j<n_;j++)
		{
			blocks_[j*n_square+n_square-n_+i].top_=&(top_[i*n+j]);
		}
	}
	//bottom
	for(int i=0;i<n_;i++)
	{
		for(int j=0;j<n_;j++)
		{
			blocks_[j*n_square+i].bottom_=&(bottom_[i*n+j]);
		}
	}
	
	ResetColor();
}

Cube::~Cube(void)
{
	SAFE_DELETE_ARRAY(front_);
	SAFE_DELETE_ARRAY(back_);
	SAFE_DELETE_ARRAY(right_);
	SAFE_DELETE_ARRAY(left_);
	SAFE_DELETE_ARRAY(top_);
	SAFE_DELETE_ARRAY(bottom_);
	SAFE_DELETE_ARRAY(blocks_);
}

void Cube::ResetColor()
{
	int n_square = n_*n_;
	for(int i=0;i<n_square;i++)
	{
		front_[i]=1;
	}
	for(int i=0;i<n_square;i++)
	{
		right_[i]=2;
	}
	for(int i=0;i<n_square;i++)
	{
		top_[i]=3;
	}
	for(int i=0;i<n_square;i++)
	{
		back_[i]=4;
	}
	for(int i=0;i<n_square;i++)
	{
		left_[i]=5;
	}
	for(int i=0;i<n_square;i++)
	{
		bottom_[i]=6;
	}
}

void Cube::Rotate(Orientation o, int id,int times)
{
	if(id<0 || id>=n_)
	{
		return;
	}
	while(times<0)
	{
		times+=4;
	}
	times = (times)%4;

	int*a1=NULL;
	int*a2=NULL;
	int*a3=NULL;
	int*a4=NULL;
	int*b1=NULL;
	int*b2=NULL;
	if(o==FRONT_BACK)
	{
		b1=front_;
		b2=back_;
		//top_[id*n_+i] -> right_[(n_-1-i)*n_+id] -> bottom_[id*n_+n_-1-i] -> left_[i*n_+id]
		a1=top_;
		a2=right_;
		a3=bottom_;
		a4=left_;
	}
	if(o==RIGHT_LEFT)
	{
		b1=right_;
		b2=left_;
		//front_[id*n_+i] -> top_[(n-1-i)*n_+id] -> back_[id*n_+n-1-i] -> bottom_[i*n_+id]
		a1 = front_;
		a2 = top_;
		a3 = back_;
		a4 = bottom_;
	}
	if(o==TOP_BOTTOM)
	{
		b1=top_;
		b2=bottom_;
		//right_[id*n_+i] -> front_[(n_-1-i)*n_+id] -> left_[id*n_+n_-1-i] -> back_[i*n_+id]
		a1 = right_;
		a2 = front_;
		a3 = left_;
		a4 = back_;
	}
	if((id==(n_-1)) && (b1!=NULL))
	{
		RotateSquare(b1,n_,times);
	}
	if((id==0) && (b2!=NULL))
	{
		RotateSquare(b2,n_,times);
	}
	if((a1!=NULL) && (a2!=NULL) && (a3!=NULL) && (a4!=NULL))
	{
		int temp_color;
		if(times==1)
		{
			for(int i=0;i<n_;i++)
			{
				temp_color			= a1[id*n_+i];
				a1[id*n_+i]			= a2[(n_-1-i)*n_+id];
				a2[(n_-1-i)*n_+id]	= a3[id*n_+n_-1-i];
				a3[id*n_+n_-1-i]	= a4[i*n_+id];
				a4[i*n_+id]			= temp_color;
			}
		}
		else if(times==3)
		{
			for(int i=0;i<n_;i++)
			{
				temp_color			= a1[id*n_+i];
				a1[id*n_+i]			= a4[i*n_+id];
				a4[i*n_+id]			= a3[id*n_+n_-1-i];
				a3[id*n_+n_-1-i]	= a2[(n_-1-i)*n_+id];
				a2[(n_-1-i)*n_+id]	= temp_color;
			}
		}
		else if(times==2)
		{
			for(int i=0;i<n_;i++)
			{
				temp_color			= a1[id*n_+i];
				a1[id*n_+i]			= a3[id*n_+n_-1-i];
				a3[id*n_+n_-1-i]	= temp_color;
				temp_color			= a2[(n_-1-i)*n_+id];
				a2[(n_-1-i)*n_+id]	= a4[i*n_+id];
				a4[i*n_+id]			= temp_color;
			}
		}
	}
}

/************************************************************************/
/* rotate matrix anticlockwise                                              */
/************************************************************************/
void Cube::RotateSquare(int* a,int n,int times)
{
	/*
	while(times<0)
	{
		times+=4;
	}
	times = (times)%4;
	*/
	assert(times>=0 && times<=3);
	int current = 0;
	if(times==1)
	{
		int * temp = new int[n*n];
		for(int i=0,current=0;i<n;i++)
		{
			for(int j=0;j<n;j++,current++)
			{
				temp[current]=a[j*n+n-1-i];//a[j,n-1-i]
			}
		}

		memcpy(a,temp,n*n*sizeof(int));
		SAFE_DELETE_ARRAY(temp);
	}
	else if(times==3)
	{
		int * temp = new int[n*n];
		for(int i=0,current=0;i<n;i++)
		{
			for(int j=0;j<n;j++,current++)
			{
				temp[current]=a[(n-1-j)*n+i];
			}
		}

		memcpy(a,temp,n*n*sizeof(int));
		SAFE_DELETE_ARRAY(temp);
	}
	else if(times==2)
	{
		int target = 0;
		int temp = 0;
		int middle = (n*n)>>1;
		for(int i=0,current=0;(i<n) && (current<middle);i++)
		{
			for(int j=0; (j<n) && (current<middle) ;j++,current++)
			{
				target=(n-1-i)*n+n-1-j;
				temp		= a[current];
				a[current]	= a[target];
				a[target]	= temp;
			}
		}
	}
	
}