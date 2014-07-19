#include <iostream>
#include "glut.h"
#include "Cube.h"
#include <gl/gl.h>
#include <gl/glu.h>
using namespace std;


void init()
{
	cout<<"init()"<<endl;
	glClearColor(0.0,0.0,0.0,0.0);
	glShadeModel(GL_FLAT);
}

void display()
{
	cout<<"display()"<<endl;
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f (0.0, 0.0, 1.0);
	glBegin(GL_POLYGON);
	//glVertex3f (0.25, 0.25, 0.0);
	//glVertex3f (0.75, 0.25, 0.0);
	//glVertex3f (0.75, 0.75, 0.0);
	//glVertex3f (0.25, 0.75, 0.0);
	glVertex3f (25, 25, -0.0);
	glVertex3f (75, 25, -0.0);
	glVertex3f (75, 75, -0.0);
	glVertex3f (25, 75, -0.0);
	glEnd();

	//gluSphere();

	glFlush();
}

void reshape(int w, int h)
{
	cout<<"reshape()"<<endl;

	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100,100,-100,100,-100,100);
	gluLookAt(0,0,200,0,0,0,0,1,0);

}

int main(int argc, char**argv)
{
	//for(int i=1;i<10;i++)
	//{
	//	Cube cube(i);
	//	if(cube.Check())
	//	{
	//		printf("cube is right\n");
	//	}
	//	else
	//	{
	//		printf("cube is not right\n");
	//	}
	//}
	//getchar();

	glutInit(&argc,argv);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800,600);
	glutCreateWindow("Cube");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
