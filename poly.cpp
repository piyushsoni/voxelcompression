/*******************************************************************
			 Polygon Support Routines
********************************************************************
    Author: Stan Sclaroff
      Date: September 9, 2004
  Comments: Basic subroutines to manage and draw polygons.        
********************************************************************/

#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "const.h"
#include "types.h"
#include "funcs.h"
#include <stdio.h>

/* move selected vertex to a new position */
void moveVert(polygonType *poly, int x, int y)
{
  if(poly && poly->selected_vert >= 0 && poly->selected_vert < poly->n_verts)
    {
      poly->verts[poly->selected_vert][0] = x;
      poly->verts[poly->selected_vert][1] = y;
    }
}

/* add vertex to polygon */
void addVert(polygonType *poly, int x, int y)
{
  if(poly && poly->n_verts < MAX_NUM_VERTS-1)
  {
	poly->verts[poly->n_verts][0] = x;
    poly->verts[poly->n_verts][1] = y;
    ++poly->n_verts;
  }
}

/* select vertex closest to given point */
void selectVert(polygonType *poly, int x, int y)
{
  int i,winner; 
  float dist_squared, winning_dist_squared,dx,dy;
  
  if(poly==0 || poly->n_verts < 0) 
    return;

  dx = x-poly->verts[0][0];
  dy = y-poly->verts[0][1];
  winning_dist_squared = dx*dx+dy*dy;
  winner = 0;

  for(i=1;i<poly->n_verts;++i)
    {
      dx = x-poly->verts[i][0];
      dy = y-poly->verts[i][1];
      dist_squared = dx*dx+dy*dy;
      if(dist_squared < winning_dist_squared)
	{
	  winner = i;
	  winning_dist_squared = dist_squared;
	}
    }
  poly->selected_vert = winner;
}

/* change color */
void changeColor(polygonType *poly, float red, float green, float blue)
{
  if(poly==0)
    return;

  poly->color.r = red;
  poly->color.g = green;
  poly->color.b = blue;
}

/* create a polygon data structure */
polygonType *createPoly(void)
{
  polygonType *poly;
  
  poly = (polygonType *)malloc(sizeof(polygonType));
  poly->n_verts=0;
  poly->concave = FALSE;
  
  poly->selected_vert = -1; 

  /* set initial color = white */
  changeColor(poly,1.0,1.0,1.0);

  return(poly);
}

/* generate necessary OpenGL commands to draw a convex polygon */
void drawConvexPoly(polygonType *poly)
{
  int i;

  if(poly==0 || poly->n_verts < 1)
    return;

  /* push the current color */
  glPushAttrib(GL_CURRENT_BIT);
  
  /* set boundary color */  
  glColor3f(poly->color.r, poly->color.g, poly->color.b);

  /* if only 1 vert, draw a point */
  if(poly->n_verts == 1)
    glBegin(GL_POINTS);
  
  /* if only 2 verts, draw line */
  else if(poly->n_verts == 2)
    glBegin(GL_LINES);

  /* else more than 2 verts, then draw poly */
  else 
    glBegin(GL_POLYGON);

  for(i=0; i<poly->n_verts; ++i)
    glVertex2f(poly->verts[i][0],poly->verts[i][1]);
  glEnd();
   
  /* pop current color */
  glPopAttrib();
}

void printVerts(polygonType *poly)
{
  int i;
  for (i=0; i<poly->n_verts; i++)
    printf("%f, %f \n", poly->verts[i][0], poly->verts[i][1]);
}
