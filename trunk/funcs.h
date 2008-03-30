/*******************************************************************
	      Function Prototypes for CS480 Project
********************************************************************
    Author: Stan Sclaroff (modified by Kyle Olszewski)
      Date: Modified September 20, 2006 (originally written September
			9, 2004) 
	 Class: CS480, Introduction to Computer Graphics
Assignment: Programming Assignment 1 - OpenGL, 2D Graphics, Polygons
  Due Date: September 25, 2005, 11:59 PM
 Problem #: All (1-3, Extra Credit).
  Comments: Modified version of funcs.h includes the helper functions
  findCrossProduct, checkIntersection, and checkPolygonIntersection,
  used in the modified versions of concavePoly.c, insidePoly.c, 
  drawConcavePoly.c, and callbacks.c
********************************************************************/

/* callbacks.c */
void selectOption(int option);
void selectColor(int color);
void reshape(int width, int height);
void display(void);
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void initPoly();
/* checkPolygonIntersect accepts a pointer to a polygonType and returns 
   TRUE (the integer 1) if the polygon is self-intersecting and
   FALSE (the integer 0) if it is not. */
int checkPolygonIntersect(polygonType *poly);

/* poly.c */
void moveVert(polygonType *poly, int x, int y);
void addVert(polygonType *poly, int x, int y);
void selectVert(polygonType *poly, int x, int y);
void changeColor(polygonType *poly, float red, float green, float blue);
polygonType *createPoly(void);
void drawConvexPoly(polygonType *poly);
void printVerts(polygonType *poly);

/* concavePoly.c */
int concavePoly(polygonType *poly);
/* findCrossProduct takes the points on two adjacent lines and returns 
   their cross product. */
float findCrossProduct(float firstVertX, float firstVertY, 
					   float secondVertX, float secondVertY, 
					   float thirdVertX, float thirdVertY);

/* drawConcavePoly.c */
void drawConcavePoly(polygonType *poly);

/* insidePoly.c */
int insidePoly(polygonType *poly, int x, int y);
/* intersectionCheck takes the start and end points of two line segments 
   [(x1, x2) and (x3,x4)] and returns TRUE if they intersect, FALSE if 
   otherwise. */
int intersectionCheck(float x1, float y1, 
					  float x2, float y2, 
					  float x3, float y3, 
					  float x4, float y4);