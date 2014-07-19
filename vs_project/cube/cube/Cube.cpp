#include <iostream>
#include <cassert>
#include "Cube.h"

#define LOCATION_ROTATE_0(a,i,j)		((a) + n_*(i)		+ (j))
#define LOCATION_ROTATE_90(a,i,j)		((a) + n_*(j)		+ n_-1-(i))
#define LOCATION_ROTATE_180(a,i,j)		((a) + n_*(n_-1-i)	+ n_-1-(j))
#define LOCATION_ROTATE_270(a,i,j)		((a) + n_*(n_-1-j)	+ (i))

const int Cube::ROTATE_ARRAY[3][4] = 
{
	{1,5,4,2},
	{0,2,3,5},
	{0,4,3,1}
};

Cube::Cube(int n):n_(n)
{
	int n_square=n*n;
	for(int i=0;i<SURFACE_NUM;i++)
	{
		surface_[i]=new char[n_square];
	}
	for(int i=0;i<SURFACE_NUM;i++)
	{
		for(int j=0;j<n_square;j++)
		{
			surface_[i][j]=i;
		}
	}
}



Cube::~Cube(void)
{
	for(int i=0;i<SURFACE_NUM;i++)
	{
		if(surface_[i]!=NULL)
		{
			delete surface_[i];
			surface_[i]=NULL;
		}
	}
}

bool Cube::Check()
{
	int n_square=n_*n_;
	for(int i=0;i<SURFACE_NUM;i++)
	{
		assert(surface_[i]!=NULL);
		char c=surface_[i][0];
		for(int j=1;j<n_square;j++)
		{
			if(surface_[i][j]!=c)
			{
				return false;
			}
		}
	}
	return true;
}



void Cube::Rotate(RotateAxis a,int n,int step)
{
	assert(n>=0 && n<n_);
	//if(step>0)
	{
		step=step%ROTATE_CYCLE;
	}

	switch(a)
	{
	case FRONT_BACK:
		if(n==0)
		{
			RotateSurface(FRONT,step);
		}
		else if(n==n_-1)
		{
			RotateSurface(BACK,-step);
		}

		break;
	case RIGHT_LEFT:
		if(n==0)
		{
			RotateSurface(LEFT,step);
		}
		else if(n==n_-1)
		{
			RotateSurface(RIGHT,-step);
		}
		break;
	case TOP_BOTTOM:
		if(n==0)
		{
			RotateSurface(TOP,step);
		}
		else if(n==n_-1)
		{
			RotateSurface(BOTTOM,-step);
		}
		break;
	default:
		assert(0);
	}
}

Cube::StepName Cube::GetStepName(int step)
{
	step = step%ROTATE_CYCLE;
	step = (step+ROTATE_CYCLE)%ROTATE_CYCLE;
	return (StepName)step;
}

void Cube::RotateSideface(RotateAxis a,int n,int step)
{
	StepName step_name = GetStepName(step);

	const int *rotate_array=ROTATE_ARRAY[(int)a];
	
	switch(step_name)
	{
	case ZERO_STEP:
		break;
	case ONE_STEP:
		for(int i=0;i<n_;i++)
		{
			char temp;
			int index;
			int x;
			int y;
			int z;
			//Convert(index,rotate_array[0],x,y,z);
		}
		break;
	case TWO_STEP:
		break;
	case THREE_STEP:
		break;
	default:
		assert(0);
	}
}

void Cube::RotateSurface(Cube::SurfaceName s, int step)
{
	StepName step_name=GetStepName(step);

	char *target_surface=surface_[(int)s];
	assert(target_surface!=NULL);

	switch(step_name)
	{
	case ZERO_STEP:
		break;
	case ONE_STEP:
		for(int i=0;i<(n_>>1);i++)
		{
			for(int j=i;j<n_-2-i;j++)
			{
				char temp;
				temp										=*LOCATION_ROTATE_0(target_surface,i,j);
				*LOCATION_ROTATE_0(target_surface,i,j)		=*LOCATION_ROTATE_270(target_surface,i,j);
				*LOCATION_ROTATE_270(target_surface,i,j)	=*LOCATION_ROTATE_180(target_surface,i,j);
				*LOCATION_ROTATE_180(target_surface,i,j)	=*LOCATION_ROTATE_90(target_surface,i,j);
				*LOCATION_ROTATE_90(target_surface,i,j)		=temp;
			}
		}
		break;
	case TWO_STEP:
		for(int i=0;i<(n_>>1);i++)
		{
			for(int j=0;j<n_;j++)
			{
				char temp;
				temp									=*LOCATION_ROTATE_0(target_surface,i,j);
				*LOCATION_ROTATE_0(target_surface,i,j)	=*LOCATION_ROTATE_180(target_surface,i,j);
				*LOCATION_ROTATE_180(target_surface,i,j)=temp;
			}
		}
		if(n_%2)
		{
			int i=(n_>>1);
			i=i*n_;
			for(int j=0;j<(n_>>1);j++)
			{
				char temp;
				temp = target_surface[i+j];
				target_surface[i+j]=target_surface[i+n_-1-j];
				target_surface[i+n_-1-j]=temp;
			}
		}
		break;
	case THREE_STEP:
		for(int i=0;i<(n_>>1);i++)
		{
			for(int j=i;j<n_-2-i;j++)
			{
				char temp;
				temp										=*LOCATION_ROTATE_0(target_surface,i,j);
				*LOCATION_ROTATE_0(target_surface,i,j)		=*LOCATION_ROTATE_90(target_surface,i,j);
				*LOCATION_ROTATE_90(target_surface,i,j)		=*LOCATION_ROTATE_180(target_surface,i,j);
				*LOCATION_ROTATE_180(target_surface,i,j)	=*LOCATION_ROTATE_270(target_surface,i,j);
				*LOCATION_ROTATE_270(target_surface,i,j)	=temp;
			}
		}
		break;
	default:
		assert(0);
	}
}

//void Cube::Convert(int &i, SurfaceName s,int x, int y,int z)
void Cube::Convert(int &i, RotateAxis axis,SurfaceName surface, int n, int variable)
{
	assert(n>=0 && n<=n-1);
	int x=-1;
	int y=-1;
	int z=-1;
	switch(axis)
	{
	case FRONT_BACK:
		x=n;
		switch(surface)
		{
		case RIGHT:
			i=z;
			break;
		case LEFT:
			break;
		case TOP:
			break;
		case BOTTOM:
			break;
		default:
			assert(0);
		}
		break;
	case RIGHT_LEFT:
		y=n;
		break;
	case TOP_BOTTOM:
		z=n;
		break;
	default:
		assert(0);
	}


	switch(surface)
	{
	case FRONT:
		i = y*n_+z;
		break;
	case BACK:
		i = y*n_+z;
		break;
	case RIGHT:
		i = z*n_+x;
		break;
	case LEFT:
		i = z*n_+x;
		break;
	case TOP:
		i = x*n_+y;
		break;
	case BOTTOM:
		i = x*n_+y;
		break;
	default:
		assert(0);
	}
}

