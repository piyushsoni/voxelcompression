/*******************************************************************
		Callback Subroutines for CS480 Example Program
********************************************************************
    Author: Stan Sclaroff (modified by Kyle Olszewski)
      Date: Modified September 20, 2006 (originally written September
			9, 2004) 
	 Class: CS480, Introduction to Computer Graphics
Assignment: Programming Assignment 1 - OpenGL, 2D Graphics, Polygons
  Due Date: September 25, 2005, 11:59 PM
 Problem #: Extra Credit: Detect when a polygon is self-intersecting
			and display a message in the GLUT window.
  Comments: Subroutines to support GLUT library callbacks: 
            menu support, redisplay/reshape, mouse actions.
            Used for drawing/modifying circular polygon.

			New modifications include the creation and display
			of a message buffer for a message informing the 
			user that a polygon is self-intersecting.

			Also included is the helper function checkPolygonIntersect,
			used to determine if a polygon is self-intersecting.  The
			method accepts a pointer to a polygonType and returns TRUE
			(the integer 1) if the polygon is self-intersecting and
			FALSE (the integer 0) if it is not.

			The method iterates throught the vertices, checking the edge
			created by the current vertex and the one ahead of it.
			It uses the method intersectionCheck with the current
			edge and each non-adjacent edge to determine if there is an
			intersection.
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glaux.h>
#include <assert.h>
#include "const.h"
#include "types.h"
#include "funcs.h"

/* static global variables for storing state information */
static int currentButton; /* currently pressed mouse button */  
static int fill = FALSE; /* area fill on or off, initially=off */
static polygonType *poly; /* polygon primitive */
/* Msg to be rendered by opengl */
static char concaveMsgBuff[300], inOutMsgBuff[300], intersectMsgBuff[300]; 
static int showIO = FALSE; /* toggle flag for inside/outside test pattern */

/* local functions */
void generateTestCase();


/* draw a string at a particular location (x,y) */
void drawString(int x, int y, char *string, void *font)
{
  int len, i;

  glRasterPos2f(x, y);
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(font, string[i]);
  }
}


/* window size change */
void reshape(int width, int height)
{
  /* change viewport dimensions */
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, width, height, 0);
  glMatrixMode(GL_MODELVIEW);
}

/*
void InitFBO(){

	GLuint fbo;
glGenFramebuffersEXT(1, &fbo);

//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);


}
*/

/* redisplaying graphics */
void display(void)
{

  int x, y, maxRow, maxCol;

  glPointSize(3);

  /* If requested, enable fill */
  if(fill==TRUE)
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  else 
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  
  /* clear the display */
  glClear(GL_COLOR_BUFFER_BIT);
  
      
  if(concavePoly(poly)==FALSE)
    {
      drawConvexPoly(poly);
	  strcpy(concaveMsgBuff, " Polygon is convex");
    }
  else
    {
      drawConcavePoly(poly);  
	  strcat(concaveMsgBuff, " Polygon is concave");
    }

  /* check if polygon intersects */
  if(checkPolygonIntersect(poly)==TRUE) 
	{
	  strcat(intersectMsgBuff, " Self-intersecting polgyon");
	}

  drawString(20,glutGet(GLUT_WINDOW_HEIGHT)-20,
	  concaveMsgBuff,GLUT_BITMAP_TIMES_ROMAN_24);
  drawString(20,glutGet(GLUT_WINDOW_HEIGHT)-40,
	  inOutMsgBuff,GLUT_BITMAP_TIMES_ROMAN_24);
  drawString(20,glutGet(GLUT_WINDOW_HEIGHT)-60,
	  intersectMsgBuff,GLUT_BITMAP_TIMES_ROMAN_24);

  if (showIO == TRUE)
  {
    glPointSize(3);
	maxRow = glutGet(GLUT_WINDOW_HEIGHT);
	maxCol = glutGet(GLUT_WINDOW_WIDTH);
	glBegin(GL_POINTS);
	for (y=0; y<maxRow; y++)
      for (x=0; x<maxCol; x++)
	  {
        if(insidePoly(poly,x,y) == TRUE)
		   glColor3f(1.0, 0.5, 0.5);
		else
		   glColor3f(0.5, 0.5, 1.0);
        glVertex2f(x,y);

	  }
	glEnd();

  }

  /* swap the double buffers*/
  glutSwapBuffers();  


  concaveMsgBuff[0] = '\0'; /* Clear the msg buffer */
  inOutMsgBuff[0] = '\0'; /* Clear the msg buffer */
  intersectMsgBuff[0] = '\0'; /* Clear the msg buffer */
}


/* 
mouse press/release handler
  
   left button: add vertex 
   middle button: inside/outside test
   right button: move vertex
*/

void mouseButton(int button, int state, int x, int y)
{
  currentButton = button;

  if (button == GLUT_LEFT_BUTTON){  
    switch (state) {      
    case GLUT_DOWN:
      addVert(poly,x,y);
      break;
    }
  }    
  else if (button == GLUT_MIDDLE_BUTTON || 
           button == (GLUT_RIGHT_BUTTON & GLUT_LEFT_BUTTON)){ 
    switch (state) {      
    case GLUT_DOWN:
      /* conduct and inside/outside test for the point */
      if(insidePoly(poly,x,y) == TRUE)
		   strcpy(inOutMsgBuff,"Point is INSIDE polygon");
      else
		   strcpy(inOutMsgBuff,"Point is OUTSIDE polygon");
      break;
    }
  }

  else if (button == GLUT_RIGHT_BUTTON){ 
    switch (state) {      
    case GLUT_DOWN:
    case GLUT_UP:
      /* select closest vertex and move it */
      selectVert(poly,x,y);
      moveVert(poly,x,y);
      break;
    }
  }

  /* update display */
  display();  
}


/* mouse motion callback */
void mouseMotion(int x, int y)
{
  switch(currentButton)
    {
    case(GLUT_RIGHT_BUTTON):
      moveVert(poly,x,y);
      break;
    }
  /* redisplay */
  display();
}

/* handles input from the keyboard */
void keyboard(unsigned char key, int x, int y)
{

  switch(key){

  case 'c':
  case 'C': 
    /* c or C key = clear polygon */
    poly->n_verts = 0;
    glutPostRedisplay();
    break;

  case 'q':
  case 'Q':
  case 27:  
    /* Esc, q, or Q key = Quit */
    exit(0);
    break;

  case 't':
  case 'T': 
    /* loop through test cases */
	generateTestCase();
    glutPostRedisplay();
    break;

  case 'f':
  case 'F': 
	if(fill == TRUE)
	  fill = FALSE; 
	else fill = TRUE;
	glutPostRedisplay();
	break;

  case 's':
  case 'S':
	  if (showIO == TRUE)
		  showIO = FALSE;
	  else showIO = TRUE;
	  glutPostRedisplay();
	  break;

  case 'p':
  case 'P':
	printVerts(poly);
	break;

  default:
    break;

  }
}


void initPoly()
{
  if(poly == 0)
   poly = createPoly();
}

void generateTestCase()
{
  int numTestCase = 7;
  static int testCase = 0;

  switch (testCase)
  {
  case 0:
	poly->n_verts=0;
	addVert(poly, 50,200);
	addVert(poly,150, 50);
	addVert(poly,250,200);
	addVert(poly,150,125);
	break;

  case 1 :
    poly->n_verts = 0;
	addVert(poly, 50, 50);
	addVert(poly,250, 50);
	addVert(poly,250,200);
	addVert(poly,150,125);
	addVert(poly, 50,200);
	break;

  case 2: 
	poly->n_verts=0;
	addVert(poly, 50, 200);
	addVert(poly, 50, 50);
	addVert(poly,200, 50);
	addVert(poly,200, 100);
	addVert(poly,100, 100);
	addVert(poly,100, 200);
	break;

  case 3:
	poly->n_verts=0;
	addVert(poly, 50,  50);
	addVert(poly,200,  50);
	addVert(poly,200, 200);
	addVert(poly,150, 200);
	addVert(poly,150, 100);
	addVert(poly, 50, 100);
	break;

  case 4:
	poly->n_verts=0;
	addVert(poly, 50, 100);
	addVert(poly,100, 100);
	addVert(poly,100,  50);
	addVert(poly,150,  50);
	addVert(poly,150, 100);
	addVert(poly,200, 100);
	addVert(poly,200, 150);
	addVert(poly,150, 150);
	addVert(poly,150, 200);
	addVert(poly,100, 200);
	addVert(poly,100, 150);
	addVert(poly, 50, 150);
	break;

  case 5:
	poly->n_verts = 0;
	addVert(poly,50,250);
	addVert(poly,50,150);
	addVert(poly,100,150);
	addVert(poly,150,50);
	addVert(poly,200,200);
	addVert(poly,250,50);
	addVert(poly,275,150);
	addVert(poly,275,250);

  case 6 :
	poly->n_verts=0;
	addVert(poly,50,50);
	addVert(poly,50,200);
	addVert(poly,75,200);
	addVert(poly,75,100); // try 75, 125
	addVert(poly,100,100);
	addVert(poly,100,200);
	addVert(poly,125,200);
	addVert(poly,125,100);  // try 125, 125
	addVert(poly,150,100);
	addVert(poly,150,200);
	addVert(poly,175,200);
	addVert(poly,175,100);
	addVert(poly,275,100);
	addVert(poly,275,5);
	addVert(poly,250,5);
	addVert(poly,250,50);
	addVert(poly,225,50);
	addVert(poly,225,5);
	addVert(poly,200,5);
	addVert(poly,200,50);
	addVert(poly,175,50);
	addVert(poly,175,5);
	addVert(poly,150,5);
	addVert(poly,150,50);
	break;


	break;

  default:
	poly->n_verts=0;
	break;

  }

  testCase = (testCase + 1 ) % numTestCase;
}

/* checkPolygonIntersect accepts a pointer to a polygonType and returns 
   TRUE (the integer 1) if the polygon is self-intersecting and
   FALSE (the integer 0) if it is not. */

int checkPolygonIntersect(polygonType *poly) {

  int numVerts = poly->n_verts; // number of vertices in the polygon
  int counter = 0;				// counter for main loop
  int counter2 = 0;				// counter for inner loop

  // iterate through the vertices
  while(counter < numVerts) {

	  // for each vertex, check all other vertices
	  while(counter2 < numVerts) {

		  // if the selected vertex is not the current vertex in the outer loop,
		  // or one of the adjacent vertices, check to see if the edges
		  // represented by the current vertex and the following vertex in each loop
		  // intersect

		  if((counter2 % numVerts != (counter + 1) % numVerts) &&
			 (counter2 % numVerts != counter % numVerts) &&
			 (counter2 % numVerts != (counter - 1 + numVerts) % numVerts)) {

			 if(intersectionCheck(poly->verts[counter2 % numVerts][0],
								  poly->verts[counter2 % numVerts][1],
								  poly->verts[(counter2 + 1) % numVerts][0],
								  poly->verts[(counter2 + 1) % numVerts][1],
								  poly->verts[counter % numVerts][0],
								  poly->verts[counter % numVerts][1],
								  poly->verts[(counter + 1) % numVerts][0],
								  poly->verts[(counter + 1) % numVerts][1])) {

			    // if there is, then the polygon is self-intersecting
				return TRUE;

			 }

		  }

		counter2++;

	  }

	  counter++;
	  // re-initialize the inner counter
	  counter2 = 0;

  }

  // if there were no intersections, the polygon is not self-intersecting 
  return FALSE;

}