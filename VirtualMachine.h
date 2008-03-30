
//For OpenGL, C++
#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H
#include<math.h>
#include<iostream>
#include<GL/glut.h>


#define sq(x) ((x)*(x))

#define PI (3.14159f)
int SPHERE_DETAIL =30;

void point(float x, float y)
{
	glBegin(GL_POINTS);

	glVertex2f(x,y);
	glEnd();
}

void point(float x, float y, float z)
{
	glBegin(GL_POINTS);

	glVertex3f(x,y,z);
	glEnd();
}

void line(float x1, float y1, float x2, float y2)
{
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void line(float x1, float y1, float z1, float x2, float y2, float z2)
{
	glBegin(GL_LINES);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glEnd();
}

void triangle(float x1,float y1,float x2,float y2,float x3,float y3)
{
	glBegin(GL_TRIANGLES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glEnd();
}

void triangle(float x1,float y1,float z1, float x2,float y2, float z2, float x3,float y3, float z3)
{
	glBegin(GL_TRIANGLES);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glVertex3f(x3, y3, z3);
	glEnd();
}


void ellipse(float x, float y, float u, float v)
{
	//To do: write code
	
}

void arc(float x,float y,float diaX,float diaY,float angle1,float angle2)
{
	//To do: write code
}

void setSphereDetail(int detail)
{
	SPHERE_DETAIL = detail	; 
}


void sphere(float x, float y, float z, float r)
{
	glPushMatrix(); 
	glTranslatef(x,y,z);
	glutSolidSphere(r,SPHERE_DETAIL,30);
	glPopMatrix(); 
}

#endif