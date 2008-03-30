/*******************************************************************
       Example Main Program for CS480 Programming Assignment 1
********************************************************************
    Author: Stan Sclaroff
            Boston University Computer Science Dept.
            September 9, 2004
********************************************************************
Description: This is a template program for the polygon tool.  It supports
             drawing a polygon vertices, moving vertices, reporting whether
             the polygon is concave or convex, and testing points are
             inside/outside the current polygon.
	     
	     LEFTMOUSE: add polygon vertex 
	     RIGHTMOUSE: move closest vertex 
	     MIDDLEMOUSE: click to see if point is inside or outside poly 
	     
	     The following keys control the program:

             Q,q: quit
             T,t: cycle through test cases
	     S,s: toggle inside/outside pattern
             P,p: print polygon vertex coordinates (for debugging)
             F,f: toggle polygon fill off/on
             C,c: clear polygon (set vertex count=0)

********************************************************************
Comments: This program uses the GLUT library. This library has it's
	    own event loop handler which passes control back to your
	    program's callback routines.  Links to WWW pages for GLUT
	    and OpenGL are provided on the course web page.
********************************************************************/
/*
#include <stdlib.h>
//#include <GL/gl.h>
//#include <GL/glut.h>
//#include <GL/glext.h>
#include <GL/glaux.h>
#include "const.h"
#include "types.h"
#include "funcs.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#include <GL/glaux.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv)
{

  GLint windW=DEFAULT_WINDOW_WIDTH, windH=DEFAULT_WINDOW_HEIGHT;

  glutInit(&argc, argv);
  initPoly();

  // display modes: 24 BIT, double buffer mode
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(windW,windH);
  glutCreateWindow("CS480/CS680 Skeleton Polygon Tool");

  // clear the display
  glClear(GL_COLOR_BUFFER_BIT);

  //set default attributes 
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glLineWidth(DEFAULT_LINE_WIDTH);

  //  register callbacks for window redisplay 
  // and reshape event callbacks 
  glutDisplayFunc(display);
  glutReshapeFunc(reshape); 

  // setup mouse handler callbacks
  glutMotionFunc(mouseMotion);
  glutMouseFunc(mouseButton);

  // setup keyboard handler callback
  glutKeyboardFunc(keyboard);

  // turn over control to GLUT
  glutMainLoop();

  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  return 0;             
}
*/