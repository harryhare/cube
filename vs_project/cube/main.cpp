#define _USE_MATH_DEFINES
#include <cstdio>
#include <cmath>
#include <cassert>
#include <windows.h>
#include <glut.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "common.h"
#include "Control.h"
#include "Cube.h"
#include "SolveCube3.h"
#include "SolveCube2.h"


#define ENABLE_DEPTH

using namespace std;

Cube *cube_array[8];//2-9
Cube *cube=NULL;
double a=4.0;
double camera_factor=2.0;
int rotation_mode=2;
const int DEFAULT_CUBE = 3;
bool transparent_cube=false;

static int motion_translation_cube3[][3] = //{axis,id,times}
{
	{3,2,-1},{3,2,2},{3,2,1},
	{2,2,-1},{2,2,2},{2,2,1},
	{1,2,-1},{3,2,2},{1,2,1},
	{3,0,1},{3,0,2},{3,0,-1},
	{2,0,1},{2,0,2},{2,0,-1},
	{1,0,1},{3,0,2},{1,0,-1}
};
static int motion_translation_cube2[][3]= //{axis,id,times}
{
	{3,0,1},{3,0,2},{3,0,-1},
	{2,0,1},{2,0,2},{2,0,-1},
	{1,0,1},{3,0,2},{1,0,-1}
};

void GetColor(int id)
{
	switch(id)
	{
	case 1:
		glColor3f(1.0,0,0);
		break;
	case 2:
		glColor3f(1.0,1.0,0);
		break;
	case 3:
		glColor3f(0,0,1.0);
		break;
	case 4:
		glColor3f(1.0,0.5,0);
		break;
	case 5:
		if(transparent_cube==false)
		{
			glColor3f(0.0,0.0,0.0);
		}
		else
		{
			glColor3f(1.0,1.0,1.0);
		}
		
		break;
	case 6:
		glColor3f(0,1.0,0);
		break;
	default:
		glColor3f(0.5,0.5,0.5);

	}
}
void GetColor(float &r, float &g, float &b, int id)
{
	switch(id)
	{
	case 1:
		r = 1;
		g = 0;
		b = 0;
		break;
	case 2:
		r = 1;
		g = 1;
		b = 0;
		break;
	case 3:
		r = 0;
		g = 0;
		b = 1;
		break;
	case 4:
		r = 1;
		g = 0.5;
		b = 0;
		break;
	case 5:
		if(transparent_cube==false)
		{
			r = 0;
			g = 0;
			b = 0;
		}
		else
		{
			r = 1;
			g = 1;
			b = 1;
		}
		break;
	case 6:
		r = 0;
		g = 1;
		b = 0;
		break;
	default:
		r = 0.5;
		g = 0.5;
		b = 0.5;

	}
}





void init(void)
{
	DEBUG_OUT("init called");
	glClearColor(0.0, 0.0, 0.0, 0.0);
	//glShadeModel(GL_FLAT);
#ifdef ENABLE_DEPTH
	glEnable(GL_DEPTH_TEST);
#endif

	//cube->Rotate(Cube::FRONT_BACK,3,1);
	//cube->Rotate(Cube::RIGHT_LEFT,3,1);
	//cube->Rotate(Cube::TOP_BOTTOM,3,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Control::GetInstance().x(),Control::GetInstance().y(),Control::GetInstance().z(),0,0,0,0,1,0);
	glGetDoublev(GL_MODELVIEW_MATRIX,Control::GetInstance().matrix());
}

void draw_cube_by_surface(double a=2.0);
void draw_cube_by_block(double a=2.0);
void draw_cube_by_block_rotate(double a=2.0,Cube::Orientation o=Cube::NONE,int id=0,float angle=0);

double modelview_matrix[16];


bool response=true;
int rotation_id=0;
int times=0;
int rotation_angle=0;
Cube::Orientation rotation_axis=Cube::NONE;

static bool is_select=false;
double x00,y00,z00;


void display(void)
{
	DEBUG_OUT("display called");
	glClear(GL_COLOR_BUFFER_BIT);

	#ifdef ENABLE_DEPTH
		glClear(GL_DEPTH_BUFFER_BIT);
	#endif 

/*
	glPushMatrix();
	glTranslated(-2,2,0);
	draw_cube_by_surface();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-2,2,0);
	//draw_cube_by_block();
	draw_cube_by_block_rotate(a,Cube::RIGHT_LEFT,0,45);
	glPopMatrix();
	*/	

	//cube->Rotate(Cube::RIGHT_LEFT,3,1);
	glPushMatrix();
	//glTranslated(-2,-2,0);
	glGetDoublev(GL_MODELVIEW_MATRIX,modelview_matrix);
	if(response==true)
	{
		draw_cube_by_block(a);
	}
	else
	{
		draw_cube_by_block_rotate(a,rotation_axis,rotation_id,rotation_angle);
	}
	glPopMatrix();

	glFlush();
}

void draw_cube_by_surface(double a)
{
	glColor3f(1.0,1.0,1.0);
	int n=cube->n();
	double edge=a/(20*n);
	static double *position=new double[(n+1)];
	for(int i=0;i<n+1;i++)
	{
		position[i]=a*i/(n)-a/2;
	}
	static double *actual_position_less=new double[n+1];
	for(int i=0;i<n+1;i++)
	{
		actual_position_less[i]=position[i]-edge;
	}
	static double *actual_position_more=new double[n+1];
	for(int i=0;i<n+1;i++)
	{
		actual_position_more[i]=position[i]+edge;
	}



	if(transparent_cube==false)
	{
		glBegin(GL_QUADS);
		glVertex3f(-a/2, -a/2, a/2);
		glVertex3f(-a/2, a/2, a/2);
		glVertex3f(a/2, a/2, a/2);
		glVertex3f(a/2, -a/2, a/2);
		glVertex3f(-a/2, -a/2, -a/2);
		glVertex3f(-a/2, a/2, -a/2);
		glVertex3f(a/2, a/2, -a/2);
		glVertex3f(a/2, -a/2, -a/2);
		glVertex3f(-a/2, a/2, -a/2);
		glVertex3f(-a/2, a/2, a/2);
		glVertex3f(a/2, a/2, a/2);
		glVertex3f(a/2, a/2, -a/2);
		glVertex3f(-a/2, -a/2, -a/2);
		glVertex3f(-a/2, -a/2, a/2);
		glVertex3f(a/2, -a/2, a/2);
		glVertex3f(a/2, -a/2, -a/2);
		glVertex3f(a/2, -a/2, -a/2);
		glVertex3f(a/2, -a/2, a/2);
		glVertex3f(a/2, a/2, a/2);
		glVertex3f(a/2, a/2, -a/2);
		glVertex3f(-a/2, -a/2, -a/2);
		glVertex3f(-a/2, -a/2, a/2);
		glVertex3f(-a/2, a/2, a/2);
		glVertex3f(-a/2, a/2, -a/2);
		glEnd();
	}
	glPolygonOffset(-1.0,1.0);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glColor3f(0.3,0.3,0.3);
	glutWireCube(a);
	glDisable(GL_POLYGON_OFFSET_LINE);


	glEnable(GL_POLYGON_OFFSET_FILL);
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			GetColor(cube->front()[i*n+j]);
			glBegin(GL_POLYGON);
			glVertex3f(actual_position_more[i],		actual_position_more[j],	a/2);
			glVertex3f(actual_position_more[i],		actual_position_less[j+1],	a/2);
			glVertex3f(actual_position_less[i+1],	actual_position_less[j+1],	a/2);
			glVertex3f(actual_position_less[i+1],	actual_position_more[j],	a/2);
			glEnd();
		}
	}

	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			GetColor((cube->back())[i*n+j]);
			glBegin(GL_POLYGON);
			glVertex3f(actual_position_more[i],		actual_position_more[j],	-a/2);
			glVertex3f(actual_position_more[i],		actual_position_less[j+1],	-a/2);
			glVertex3f(actual_position_less[i+1],	actual_position_less[j+1],	-a/2);
			glVertex3f(actual_position_less[i+1],	actual_position_more[j],	-a/2);
			glEnd();
		}
	}

	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{

			GetColor((cube->right())[i*n+j]);
			glBegin(GL_POLYGON);
			glVertex3f(a/2,	actual_position_more[i],		actual_position_more[j]);
			glVertex3f(a/2,	actual_position_more[i],		actual_position_less[j+1]);
			glVertex3f(a/2,	actual_position_less[i+1],		actual_position_less[j+1]);
			glVertex3f(a/2,	actual_position_less[i+1],		actual_position_more[j]);
			glEnd();
		}
	}

	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			GetColor((cube->left())[i*n+j]);
			glBegin(GL_POLYGON);
			glVertex3f(-a/2,	actual_position_more[i],		actual_position_more[j]);
			glVertex3f(-a/2,	actual_position_more[i],		actual_position_less[j+1]);
			glVertex3f(-a/2,	actual_position_less[i+1],		actual_position_less[j+1]);
			glVertex3f(-a/2,	actual_position_less[i+1],		actual_position_more[j]);
			glEnd();
		}
	}

	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			GetColor((cube->top())[i*n+j]);
			glBegin(GL_POLYGON);
			glVertex3f(actual_position_more[j],		a/2,	actual_position_more[i]);
			glVertex3f(actual_position_less[j+1],	a/2,	actual_position_more[i]);
			glVertex3f(actual_position_less[j+1],	a/2,	actual_position_less[i+1]);
			glVertex3f(actual_position_more[j],		a/2,	actual_position_less[i+1]);
			glEnd();
		}
	}
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			GetColor((cube->bottom())[i*n+j]);
			glBegin(GL_POLYGON);
			glVertex3f(actual_position_more[j],		-a/2,	actual_position_more[i]);
			glVertex3f(actual_position_less[j+1],	-a/2,	actual_position_more[i]);
			glVertex3f(actual_position_less[j+1],	-a/2,	actual_position_less[i+1]);
			glVertex3f(actual_position_more[j],		-a/2,	actual_position_less[i+1]);
			glEnd();
		}
	}

	glDisable(GL_POLYGON_OFFSET_FILL);
}

double position(int i, int n, double a)
{
	return a*i/(n)-a/2;
}
double actual_position_more(int i,int n, double a,double edge)
{
	return a*i/(n)-a/2+edge;
}
double actual_position_less(int i,int n, double a,double edge)
{
	return a*i/(n)-a/2-edge;
}

void draw_cube_by_block_rotate(double a,Cube::Orientation o,int id,float angle)
{
	int n=cube->n();
	int n_square=n*n;
	int n_cubic=n_square*n;
	double edge=a/(20*n);

	//static double *position=new double[(n+1)];
	//for(int i=0;i<n+1;i++)
	//{
	//	position[i]=a*i/(n)-a/2;
	//}
	//static double *actual_position_less=new double[n+1];
	//for(int i=0;i<n+1;i++)
	//{
	//	actual_position_less[i]=position[i]-edge;
	//}
	//static double *actual_position_more=new double[n+1];
	//for(int i=0;i<n+1;i++)
	//{
	//	actual_position_more[i]=position[i]+edge;
	//}

	for(int t=0;t<n_cubic;t++)
	{
		int k = t%n;
		int j = (t/n)%n;
		int i = t/n_square;

		//double x1=position[i];
		//double y1=position[j];
		//double z1=position[k];
		//double x2=position[i+1];
		//double y2=position[j+1];
		//double z2=position[k+1];
		double x1=position(i,n,a);
		double y1=position(j,n,a);
		double z1=position(k,n,a);
		double x2=position(i+1,n,a);
		double y2=position(j+1,n,a);
		double z2=position(k+1,n,a);
		glPushMatrix();
		switch(o)
		{
		case Cube::FRONT_BACK:
			if(k==id)
			{
				glRotatef(angle,0,0,1);
			}
			break;
		case Cube::RIGHT_LEFT:
			if(i==id)
			{
				glRotatef(angle,1,0,0);
			}
			break;
		case Cube::TOP_BOTTOM:
			if(j==id)
			{
				glRotatef(angle,0,1,0);
			}
			break;
		default:
			break;
		}


		if(transparent_cube==false)
		{
			glColor3f(1,1,1);
			glBegin(GL_QUADS);

			glVertex3f(x2,y2,z2);
			glVertex3f(x1,y2,z2);
			glVertex3f(x1,y1,z2);
			glVertex3f(x2,y1,z2);

			glVertex3f(x2,y2,z1);
			glVertex3f(x1,y2,z1);
			glVertex3f(x1,y1,z1);
			glVertex3f(x2,y1,z1);

			glVertex3f(x2,y2,z2);
			glVertex3f(x2,y1,z2);
			glVertex3f(x2,y1,z1);
			glVertex3f(x2,y2,z1);

			glVertex3f(x1,y2,z2);
			glVertex3f(x1,y1,z2);
			glVertex3f(x1,y1,z1);
			glVertex3f(x1,y2,z1);

			glVertex3f(x2,y2,z2);
			glVertex3f(x1,y2,z2);
			glVertex3f(x1,y2,z1);
			glVertex3f(x2,y2,z1);

			glVertex3f(x2,y1,z2);
			glVertex3f(x1,y1,z2);
			glVertex3f(x1,y1,z1);
			glVertex3f(x2,y1,z1);

			glEnd();
		}
		//x1=actual_position_more[i];
		//y1=actual_position_more[j];
		//z1=actual_position_more[k];
		//x2=actual_position_less[i+1];
		//y2=actual_position_less[j+1];
		//z2=actual_position_less[k+1];
		x1=actual_position_more(i,n,a,edge);
		y1=actual_position_more(j,n,a,edge);
		z1=actual_position_more(k,n,a,edge);
		x2=actual_position_less(i+1,n,a,edge);
		y2=actual_position_less(j+1,n,a,edge);
		z2=actual_position_less(k+1,n,a,edge);

		glPolygonOffset(-1.0,1.0);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glBegin(GL_QUADS);
		if(cube->blocks()[t].front_!=NULL)
		{
			GetColor(*(cube->blocks()[t].front_));
			glVertex3f(x2,y2,a/2);
			glVertex3f(x1,y2,a/2);
			glVertex3f(x1,y1,a/2);
			glVertex3f(x2,y1,a/2);
		}
		if(cube->blocks()[t].back_!=NULL)
		{
			GetColor(*(cube->blocks()[t].back_));
			glVertex3f(x2,y2,-a/2);
			glVertex3f(x1,y2,-a/2);
			glVertex3f(x1,y1,-a/2);
			glVertex3f(x2,y1,-a/2);
		}
		if(cube->blocks()[t].right_!=NULL)
		{
			GetColor(*(cube->blocks()[t].right_));
			glVertex3f(a/2,y2,z2);
			glVertex3f(a/2,y1,z2);
			glVertex3f(a/2,y1,z1);
			glVertex3f(a/2,y2,z1);
		}
		if(cube->blocks()[t].left_!=NULL)
		{
			GetColor(*(cube->blocks()[t].left_));
			glVertex3f(-a/2,y2,z2);
			glVertex3f(-a/2,y1,z2);
			glVertex3f(-a/2,y1,z1);
			glVertex3f(-a/2,y2,z1);

		}
		if(cube->blocks()[t].top_!=NULL)
		{
			GetColor(*(cube->blocks()[t].top_));
			glVertex3f(x2,a/2,z2);
			glVertex3f(x1,a/2,z2);
			glVertex3f(x1,a/2,z1);
			glVertex3f(x2,a/2,z1);
		}
		if(cube->blocks()[t].bottom_!=NULL)
		{
			GetColor(*(cube->blocks()[t].bottom_));
			glVertex3f(x2,-a/2,z2);
			glVertex3f(x1,-a/2,z2);
			glVertex3f(x1,-a/2,z1);
			glVertex3f(x2,-a/2,z1);

		}

		glEnd();
		glDisable(GL_POLYGON_OFFSET_FILL);

		glPopMatrix();

	}
}


void draw_cube_by_block(double a)
{

	int n=cube->n();
	int n_square=n*n;
	int n_cubic=n_square*n;
	double edge=a/(20*n);

	/*static double *position=new double[(n+1)];
	for(int i=0;i<n+1;i++)
	{
		position[i]=a*i/(n)-a/2;
	}
	static double *actual_position_less=new double[n+1];
	for(int i=0;i<n+1;i++)
	{
		actual_position_less[i]=position[i]-edge;
	}
	static double *actual_position_more=new double[n+1];
	for(int i=0;i<n+1;i++)
	{
		actual_position_more[i]=position[i]+edge;
	}*/

	for(int t=0;t<n_cubic;t++)
	{
		int k = t%n;
		int j = (t/n)%n;
		int i = t/n_square;
		
		//double x1=position[i];
		//double y1=position[j];
		//double z1=position[k];
		//double x2=position[i+1];
		//double y2=position[j+1];
		//double z2=position[k+1];
		double x1=position(i,n,a);
		double y1=position(j,n,a);
		double z1=position(k,n,a);
		double x2=position(i+1,n,a);
		double y2=position(j+1,n,a);
		double z2=position(k+1,n,a);

		glPushMatrix();
		


		if(transparent_cube==false)
		{
			glColor3f(1,1,1);
			glBegin(GL_QUADS);

			glVertex3f(x2,y2,z2);
			glVertex3f(x1,y2,z2);
			glVertex3f(x1,y1,z2);
			glVertex3f(x2,y1,z2);

			glVertex3f(x2,y2,z1);
			glVertex3f(x1,y2,z1);
			glVertex3f(x1,y1,z1);
			glVertex3f(x2,y1,z1);

			glVertex3f(x2,y2,z2);
			glVertex3f(x2,y1,z2);
			glVertex3f(x2,y1,z1);
			glVertex3f(x2,y2,z1);

			glVertex3f(x1,y2,z2);
			glVertex3f(x1,y1,z2);
			glVertex3f(x1,y1,z1);
			glVertex3f(x1,y2,z1);

			glVertex3f(x2,y2,z2);
			glVertex3f(x1,y2,z2);
			glVertex3f(x1,y2,z1);
			glVertex3f(x2,y2,z1);

			glVertex3f(x2,y1,z2);
			glVertex3f(x1,y1,z2);
			glVertex3f(x1,y1,z1);
			glVertex3f(x2,y1,z1);

			glEnd();
		}
		//x1=actual_position_more[i];
		//y1=actual_position_more[j];
		//z1=actual_position_more[k];
		//x2=actual_position_less[i+1];
		//y2=actual_position_less[j+1];
		//z2=actual_position_less[k+1];
		x1=actual_position_more(i,n,a,edge);
		y1=actual_position_more(j,n,a,edge);
		z1=actual_position_more(k,n,a,edge);
		x2=actual_position_less(i+1,n,a,edge);
		y2=actual_position_less(j+1,n,a,edge);
		z2=actual_position_less(k+1,n,a,edge);

		glPolygonOffset(-1.0,1.0);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glBegin(GL_QUADS);
		if(cube->blocks()[t].front_!=NULL)
		{
			GetColor(*(cube->blocks()[t].front_));
			glVertex3f(x2,y2,a/2);
			glVertex3f(x1,y2,a/2);
			glVertex3f(x1,y1,a/2);
			glVertex3f(x2,y1,a/2);
		}
		if(cube->blocks()[t].back_!=NULL)
		{
			GetColor(*(cube->blocks()[t].back_));
			glVertex3f(x2,y2,-a/2);
			glVertex3f(x1,y2,-a/2);
			glVertex3f(x1,y1,-a/2);
			glVertex3f(x2,y1,-a/2);
		}
		if(cube->blocks()[t].right_!=NULL)
		{
			GetColor(*(cube->blocks()[t].right_));
			glVertex3f(a/2,y2,z2);
			glVertex3f(a/2,y1,z2);
			glVertex3f(a/2,y1,z1);
			glVertex3f(a/2,y2,z1);
		}
		if(cube->blocks()[t].left_!=NULL)
		{
			GetColor(*(cube->blocks()[t].left_));
			glVertex3f(-a/2,y2,z2);
			glVertex3f(-a/2,y1,z2);
			glVertex3f(-a/2,y1,z1);
			glVertex3f(-a/2,y2,z1);

		}
		if(cube->blocks()[t].top_!=NULL)
		{
			GetColor(*(cube->blocks()[t].top_));
			glVertex3f(x2,a/2,z2);
			glVertex3f(x1,a/2,z2);
			glVertex3f(x1,a/2,z1);
			glVertex3f(x2,a/2,z1);
		}
		if(cube->blocks()[t].bottom_!=NULL)
		{
			GetColor(*(cube->blocks()[t].bottom_));
			glVertex3f(x2,-a/2,z2);
			glVertex3f(x1,-a/2,z2);
			glVertex3f(x1,-a/2,z1);
			glVertex3f(x2,-a/2,z1);

		}

		glEnd();
		glDisable(GL_POLYGON_OFFSET_FILL);

		glPopMatrix();

	}
}


bool checkpoint(double a,int n,double in,double &out)
{
	out=((in+a/2)*n/a);
	return(out>=0&&out<n);

	/*
	//int n_square=n*n;
	//int n_cubic=n_square*n;

	double edge=a/(20*n);
	double *position=new double[(n+1)];
	for(int i=0;i<n+1;i++)
	{
		position[i]=a*i/(n)-a/2;
	}
	if(position[n]>a/2)
	{
		position[n]=a/2;
	}
	double *actual_position_less=new double[n+1];
	for(int i=0;i<n+1;i++)
	{
		actual_position_less[i]=position[i]-edge;
	}
	double *actual_position_more=new double[n+1];
	for(int i=0;i<n+1;i++)
	{
		actual_position_more[i]=position[i]+edge;
	}
	*/
}
void checkpoint(double a,int n,double in,int &out)
{
	out=(int)floor((in+a/2)*n/a);
}
void checkpoint2(double a,int n,double in,double &out)
{
	out=((in+a/2)*n/a);
}
int check_equal(double a,int n,double x0, double x1)
{
	assert(a!=0);
	assert(n!=0);
	if(fabs((x0-x1)*n/a)<1e-6)
	{
		return true;
	}
	return false;
	
}
double crosspoint(double x0,double x1,double x,double y0,double y1)
{
	assert(x1!=x0);
	return y0+(double)(x-x0)*(y1-y0)/(x1-x0);
}




bool convert(int mouse_x,int mouse_y,double a,int n,double &i,double &j,double &k,int &select_type)
{
	double x,y,z;
	double x0,y0,z0;
	double x1,y1,z1;
	bool is_select;
	//double modelview_matrix[16];
	double projection_matrix[16];
	int viewport[4];
	//glGetDoublev(GL_MODELVIEW_MATRIX,modelview_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,projection_matrix);
	glGetIntegerv(GL_VIEWPORT,viewport);
	gluUnProject((double)mouse_x,(double)(viewport[3]-mouse_y),0.0,modelview_matrix,projection_matrix,viewport,&x0,&y0,&z0);
	gluUnProject((double)mouse_x,(double)(viewport[3]-mouse_y),1.0,modelview_matrix,projection_matrix,viewport,&x1,&y1,&z1);
	if(z0>z1)//front
	{
		z=a/2;
		if(false==check_equal(a,n,z0,z1))
		{
			x=crosspoint(z0,z1,z,x0,x1);
			y=crosspoint(z0,z1,z,y0,y1);
		}
		is_select=checkpoint(a,n,x,i)&&checkpoint(a,n,y,j);
		select_type=is_select*1;
	}
	else if(z0<z1) //back
	{
		z=-a/2;
		if(false==check_equal(a,n,z0,z1))
		{
			x=crosspoint(z0,z1,z,x0,x1);
			y=crosspoint(z0,z1,z,y0,y1);
		}
		is_select=checkpoint(a,n,x,i)&&checkpoint(a,n,y,j);
		select_type=is_select*2;
	}
	if(is_select==false)
	{
		if(x0>x1)//right
		{
			x=a/2;
			if(false==check_equal(a,n,x0,x1))
			{
				z=crosspoint(x0,x1,x,z0,z1);
				y=crosspoint(x0,x1,x,y0,y1);
			}
			is_select=checkpoint(a,n,z,k)&&checkpoint(a,n,y,j);
			select_type=is_select*3;
		}
		else if(x0<x1)//left
		{
			x=-a/2;
			if(false==check_equal(a,n,x0,x1))
			{
				z=crosspoint(x0,x1,x,z0,z1);
				y=crosspoint(x0,x1,x,y0,y1);
			}
			is_select=checkpoint(a,n,z,k)&&checkpoint(a,n,y,j);
			select_type=is_select*4;
		}
	}

	if(is_select==false)
	{
		if(y0>y1)//top
		{
			y=a/2;
			if(false==check_equal(a,n,z0,z1))
			{
				x=crosspoint(y0,y1,y,x0,x1);
				z=crosspoint(y0,y1,y,z0,z1);
			}
			is_select=checkpoint(a,n,x,i)&&checkpoint(a,n,z,k);
			select_type=is_select*5;
		}
		else if(y0<y1)//bottom
		{
			y=-a/2;
			if(false==check_equal(a,n,z0,z1))
			{
				x=crosspoint(y0,y1,y,x0,x1);
				z=crosspoint(y0,y1,y,z0,z1);
			}
			is_select=checkpoint(a,n,x,i)&&checkpoint(a,n,z,k);
			select_type=is_select*6;
		}
	}
	cout<<is_select<<","<<i<<","<<j<<","<<k<<endl;
	return is_select;
}




void rotation1(int mouse_x,int mouse_y,double a,int n,int i,int j,int k,int surface_type)
{
	int i1,j1,k1;
	double x,y,z;
	double x0,y0,z0;
	double x1,y1,z1;
	//double modelview_matrix[16];
	double projection_matrix[16];
	int viewport[4];
	//glGetDoublev(GL_MODELVIEW_MATRIX,modelview_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,projection_matrix);
	glGetIntegerv(GL_VIEWPORT,viewport);
	gluUnProject((double)mouse_x,(double)(viewport[3]-mouse_y),0.0,modelview_matrix,projection_matrix,viewport,&x0,&y0,&z0);
	gluUnProject((double)mouse_x,(double)(viewport[3]-mouse_y),1.0,modelview_matrix,projection_matrix,viewport,&x1,&y1,&z1);

	int t;
	if(surface_type%2==1)
	{
		t=1;
	}
	else
	{
		t=-1;
	}
	int t2=0;
	rotation_axis=Cube::NONE;
	times=0;
	if((surface_type-1)/2==0)
	{
		z=a/2*t;
		if(false==check_equal(a,n,z0,z1))
		{
			x=crosspoint(z0,z1,z,x0,x1);
			y=crosspoint(z0,z1,z,y0,y1);
		}
		checkpoint(a,n,x,i1);
		checkpoint(a,n,y,j1);

		if(i==i1)
		{
			rotation_axis=Cube::RIGHT_LEFT;
			rotation_id=i;
			t2=j1-j;
			if(j1>j)
			{
				times=0-t;
			}
			else if(j1<j)
			{
				times=t;
			}

		}
		else if(j==j1)
		{
			rotation_axis=Cube::TOP_BOTTOM;
			rotation_id=j;
			t2=i-i1;
			if(i1>i)
			{
				times=0-t;
			}
			else if(i1<i)
			{
				times=t;
			}
		}
	}
	else if((surface_type-1)/2==1)
	{
		x=a/2*t;
		if(false==check_equal(a,n,x0,x1))
		{
			z=crosspoint(x0,x1,x,z0,z1);
			y=crosspoint(x0,x1,x,y0,y1);
		}
		checkpoint(a,n,z,k1);
		checkpoint(a,n,y,j1);
		if(j1==j)
		{
			rotation_axis=Cube::TOP_BOTTOM;
			rotation_id=j;
			t2=k1-k;
		}
		else if(k1==k)
		{
			rotation_axis=Cube::FRONT_BACK;
			rotation_id=k;
			t2=j-j1;
		}
	}
	else if((surface_type-1)/2==2)
	{
		y=a/2*t;
		if(false==check_equal(a,n,z0,z1))
		{
			x=crosspoint(y0,y1,y,x0,x1);
			z=crosspoint(y0,y1,y,z0,z1);
		}
		checkpoint(a,n,x,i1);
		checkpoint(a,n,z,k1);
		if(k1==k)
		{
			rotation_axis=Cube::FRONT_BACK;
			rotation_id=k;
			t2=i1-i;
		}
		else if(i1==i)
		{
			rotation_axis=Cube::RIGHT_LEFT;
			rotation_id=i;
			t2=k-k1;
		}
	}

	if(t2>0)
	{
		times=0-t;
	}
	else if(t2<0)
	{
		times=t;
	}
}

void rotation2(int mouse_x,int mouse_y,double a,int n,double i,double j,double k,int surface_type)
{
	double i1,j1,k1;
	double x,y,z;
	double x0,y0,z0;
	double x1,y1,z1;
	//double modelview_matrix[16];
	double projection_matrix[16];
	int viewport[4];
	//glGetDoublev(GL_MODELVIEW_MATRIX,modelview_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,projection_matrix);
	glGetIntegerv(GL_VIEWPORT,viewport);
	gluUnProject((double)mouse_x,(double)(viewport[3]-mouse_y),0.0,modelview_matrix,projection_matrix,viewport,&x0,&y0,&z0);
	gluUnProject((double)mouse_x,(double)(viewport[3]-mouse_y),1.0,modelview_matrix,projection_matrix,viewport,&x1,&y1,&z1);

	int t;
	if(surface_type%2==1)
	{
		t=1;
	}
	else
	{
		t=-1;
	}
	double t2=0;
	rotation_axis=Cube::NONE;
	times=0;
	if((surface_type-1)/2==0)
	{
		z=a/2*t;
		if(false==check_equal(a,n,z0,z1))
		{
			x=crosspoint(z0,z1,z,x0,x1);
			y=crosspoint(z0,z1,z,y0,y1);
		}
		checkpoint2(a,n,x,i1);
		checkpoint2(a,n,y,j1);
		double diff1=fabs(i-i1);
		double diff2=fabs(j-j1);
		if(diff1<diff2)
		{
			rotation_axis=Cube::RIGHT_LEFT;
			rotation_id=i;
			t2=j1-j;


		}
		else if(diff2<diff1)
		{
			rotation_axis=Cube::TOP_BOTTOM;
			rotation_id=j;
			t2=i-i1;

		}
	}
	else if((surface_type-1)/2==1)
	{
		x=a/2*t;
		if(false==check_equal(a,n,x0,x1))
		{
			z=crosspoint(x0,x1,x,z0,z1);
			y=crosspoint(x0,x1,x,y0,y1);
		}
		checkpoint2(a,n,z,k1);
		checkpoint2(a,n,y,j1);
		double diff1=fabs(j1-j);
		double diff2=fabs(k1-k);
		if(diff1<diff2)
		{
			rotation_axis=Cube::TOP_BOTTOM;
			rotation_id=j;
			t2=k1-k;
		}
		else if(diff2<diff1)
		{
			rotation_axis=Cube::FRONT_BACK;
			rotation_id=k;
			t2=j-j1;
		}
	}
	else if((surface_type-1)/2==2)
	{
		y=a/2*t;
		if(false==check_equal(a,n,z0,z1))
		{
			x=crosspoint(y0,y1,y,x0,x1);
			z=crosspoint(y0,y1,y,z0,z1);
		}
		checkpoint2(a,n,x,i1);
		checkpoint2(a,n,z,k1);
		double diff1=fabs(k1-k);
		double diff2=fabs(i1-i);
		if(diff1<diff2)
		{
			rotation_axis=Cube::FRONT_BACK;
			rotation_id=k;
			t2=i1-i;
		}
		else if(diff2<diff1)
		{
			rotation_axis=Cube::RIGHT_LEFT;
			rotation_id=i;
			t2=k-k1;
		}
	}

	if(t2>0.5)
	{
		times=0-t;
	}
	else if(t2<-0.5)
	{
		times=t;
	}
}


void timer(int value)
{
	if(value>=10)
	{
		cube->Rotate(rotation_axis,rotation_id,times);
		response=true;
		glutPostRedisplay();
		return;
	}
	rotation_angle=value*90/10*times;
	glutPostRedisplay();
	glutTimerFunc(10,timer,value+1);
}

const int* solve_array;
int solve_n;
int(* motion_translation_cube)[3];
void timer_solve_cube(int value)
{
	int n=solve_n;
	const int *solve=solve_array;
	if(value%10==0)
	{
		if(value/10>=1)
		{
			cube->Rotate(rotation_axis,rotation_id,times);
		}
		if(value/10==n)
		{
			response=true;
			glutPostRedisplay();
			std::cout<<"solve_done"<<std::endl;
			return;
		}
		if(value/10<n)
		{
			int i=value/10;
			i=solve[i];
			rotation_axis=(Cube::Orientation)motion_translation_cube[i][0];
			rotation_id=motion_translation_cube[i][1];
			times=motion_translation_cube[i][2];
			assert(times==-1 || times==1);
		}
	}
	rotation_angle=value%10*90/10*times;
	glutPostRedisplay();
	glutTimerFunc(20,timer_solve_cube,value+1);
}


void mouse_click(int button, int state, int mouse_x, int mouse_y)
{
	if(response==false)
	{
		return;
	}
	cout<<"x:"<<mouse_x<<",y:"<<mouse_y<<endl;
	static int i,j,k;
	static double di,dj,dk;
	static int surface_type=0;
	int n=cube->n();
	if(state==GLUT_DOWN)
	{
		double projection_matrix[16];
		int viewport[4];
		glGetDoublev(GL_PROJECTION_MATRIX,projection_matrix);
		glGetIntegerv(GL_VIEWPORT,viewport);
		gluUnProject((double)mouse_x,(double)(viewport[3]-mouse_y),0.0,Control::GetInstance().matrix(),projection_matrix,viewport,&x00,&y00,&z00);
		
		is_select=convert(mouse_x,mouse_y,a,n,di,dj,dk,surface_type);
		i=di;
		j=dj;
		k=dk;

	}
	if(state==GLUT_UP)
	{
		if(is_select==true)
		{
			switch(rotation_mode)
			{
			case 1:
				rotation1(mouse_x,mouse_y,a,n,i,j,k,surface_type);
				break;
			case 2:
				rotation2(mouse_x,mouse_y,a,n,di,dj,dk,surface_type);
				break;
			default:
				break;
			}
			if(times!=0)
			{
				response=false;
				glutTimerFunc(10,timer,1);
			}
		}
		if(is_select==false)
		{
			glGetDoublev(GL_MODELVIEW_MATRIX,Control::GetInstance().matrix());
		}
		is_select=false;
		return;
	}

	
}

double length(double x,double y,double z)
{
	return sqrt(x*x+y*y+z*z);
}

void mouse_motion(int mouse_x, int mouse_y)
{
	if(response==false)
	{
		return;
	}
	if(is_select==false)
	{
		double x11,y11,z11;
		double projection_matrix[16];
		int viewport[4];
		glGetDoublev(GL_PROJECTION_MATRIX,projection_matrix);
		glGetIntegerv(GL_VIEWPORT,viewport);
		gluUnProject((double)mouse_x,(double)(viewport[3]-mouse_y),0.0,Control::GetInstance().matrix(),projection_matrix,viewport,&x11,&y11,&z11);
		double nx,ny,nz;	//rotation axis
		double angle;		//rotation angle
		double l1	=	length(x00,y00,z00);
		double l2	=	length(x11,y11,z11);
		nx = y00*z11 - z00*y11;
		ny = z00*x11 - x00*z11;
		nz = x00*y11 - y00*x11;
		angle = acos( (x00*x11+y00*y11+z00*z11) / (l1*l2) );
		glLoadMatrixd(Control::GetInstance().matrix());
		glRotatef(angle*180/M_PI*camera_factor,nx,ny,nz);
		glutPostRedisplay();
	}
}
void keyboard(unsigned char key, int x, int y)
{
	if(response==false)
	{
		return;
	}
	if(key>='2'&&key<='9')
	{
		int temp=key-'0';
		if(cube->n()!=temp)
		{
			cube=cube_array[temp-2];
		}
	}
	if(key=='t')
	{
		transparent_cube=!transparent_cube;
	}
	if(key=='r')
	{
		cube->ResetColor();
	}
	if(key=='R')// default eye position
	{
		if(cube->n()==3)
		{
			SolveCube3::GetInstance()->MakeRandomCube(cube);
		}
		else if(cube->n()==2)
		{
			SolveCube2::GetInstance()->MakeRandomCube(cube);
		}
	}
	if(key=='s'|| key=='S')
	{
		if(cube->n()==3)
		{
			SolveCube3::GetInstance()->GetData(cube);
			SolveCube3::GetInstance()->Solve();
			printf("solve cube ...\n");
			rotation_angle=0;
			rotation_axis=Cube::NONE;
			rotation_id=0;
			solve_n=SolveCube3::GetInstance()->solution_single_length();
			solve_array=SolveCube3::GetInstance()->solution_single_move();
			motion_translation_cube=motion_translation_cube3;
			response=false;
			glutTimerFunc(20,timer_solve_cube,0);
		}
		else if(cube->n()==2)
		{
			SolveCube2::GetInstance()->GetData(cube);
			SolveCube2::GetInstance()->Solve();
			printf("solve cube ...\n");
			rotation_angle=0;
			rotation_axis=Cube::NONE;
			rotation_id=0;
			solve_n=SolveCube2::GetInstance()->solution_single_length();
			solve_array=SolveCube2::GetInstance()->solution_single_move();
			motion_translation_cube=motion_translation_cube2;
			response=false;
			glutTimerFunc(20,timer_solve_cube,0);
		}
	}

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	DEBUG_OUT("reshape called");
	glViewport(0,0,GLsizei(w),GLsizei(h));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-5,5,-5,5,-2,5);
	glMatrixMode(GL_MODELVIEW);
}

void produce_cube()
{
	for(int i=0;i<8;i++)
	{
		cube_array[i]=new Cube(i+2);
	}
	cube=cube_array[DEFAULT_CUBE-2];
}

void destroy_cube()
{
	for(int i=0;i<8;i++)
	{
		delete cube_array[i];
	}
	cube=NULL;
}

int main(int argc, char ** argv)
{
	srand((unsigned int)time(NULL));
	assert(RAND_MAX>10000);
	produce_cube();


	for(int i=0;i<argc;i++)
	{
		cout<<"argv["<<i<<"] = "<< argv[i]<<endl;
	}
	glutInit(&argc,argv);

	#ifdef ENABLE_DEPTH
		glutInitDisplayMode(GLUT_DEPTH|GLUT_RGBA|GLUT_SINGLE);
	#else
		glutInitDisplayMode(GLUT_RGBA|GLUT_SINGLE);
	#endif

	glutInitWindowPosition(100,50);
	glutInitWindowSize(800,600);
	glutCreateWindow("cube");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse_click);
	glutMotionFunc(mouse_motion);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	system("pause");
	return 0;
}