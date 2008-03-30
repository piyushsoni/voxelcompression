/*******************************************************************
						   insidePoly.c
********************************************************************
    Author: Kyle Olszewski (based on the template by Stan Sclaroff)
      Date: Modified September 20, 2006 (originally written 
			September 9, 2004)
	 Class: CS480, Introduction to Computer Graphics
Assignment: Programming Assignment 1 - OpenGL, 2D Graphics, Polygons
  Due Date: September 25, 2005, 11:59 PM
 Problem #: 1. Write a function that uses the even/odd parity rule to
			determine if a given point is inside or outside the 
			polygon.
  Comments: 

		insidePoly accepts a pointer to a polygonType and two integers
	representing the x and y values of a point in the graphics
	window.  It returns TRUE (the integer 1) if the point is 
	"inside" the polygon, FALSE (the integer 0) if otherwise. A point 
	on the edge of a polygon is considered as "inside."  If uses the
	even/odd parity rule discussed in class to determine if a point
	is inside or outside the polygon.
	
		This method generates a random point outside the boundaries of 
	the graphics window.  It iterates through the vertices, first 
	checking to see if the given test point is the current vertex (and thus, 
	must be "inside" the polygon); if so, it returns TRUE.  Otherwise, 
	the helper method intersectionCheck (described below) is used on the 
	line segment from the current vertex to the next one and the line 
	segment from the test point to the point outside the window.
	intersectionCheck returns TRUE if there is an intersection and records 
	the x and y values of the intersection, and returns FALSE otherwise.  
	If there is an intersection, the method then finds the slope and 
	y-intercept of the current line segment, then uses it to determine if 
	the given test point is on the line (and thus, by our definition, 
	"inside" the polygon; points on the edges between vertices need special
	handling because the line through them may intersect the polygon an
	even number of times); if it is, it returns TRUE.  Otherwise, if
	there is an intersection but the point is not on the line, the 
	intersection counter is incremented;  if there is no intersection, the
	counter is not incremented.

		Once the method has iterated through all of the vertices, it checks
	to see if the intersect counter's value is even or odd.  If it is even,
	the point is "outside" the polygon, so it returns FALSE.  If it is odd,
	the point is "inside" the polygon, so it returns TRUE.
	
		intersectionCheck is a helper function used to determine if two line 
	segments intersect.  It accepts eight float values representing the start
	and end points of each line segment, and returns TRUE if there is an
	intersection, FALSE otherwise.  It records the value of the intersection 
	in the static variables xIntersect and yIntersect, if there is one.
	
		The method uses the x and y values of the start and end points of
	the line segments to find the equations Ax + By = C and Dx + Ey = F 
	which represent the lines.  It then uses the determinant of the 
	matrix to determine if there is a solution or not.  If the determinant
	is 0, then there is no solution, so it returns false.  If the determinant
	is not 0, then there is a solution, so it uses the value of the
	coefficients and the determinant to find the intersection.  It
	then checks to ensure that the intersection is within the boundaries
	of each line segment; if it is, it returns TRUE; otherwise, it returns
	FALSE.
********************************************************************/

#include <stdlib.h>
#include <GL/glut.h>
#include "const.h"
#include "types.h"
#include "funcs.h"

static float xIntersect = 0; // variable to store the x value of an
							 // intersection
static float yIntersect = 0; // variable to store the y value of and
							 // intersection

/*  insidePoly accepts a pointer to a polygonType and two integers
	representing the x and y values of a point in the graphics
	window.  It returns TRUE (the integer 1) if the point is 
	"inside" the polygon, FALSE (the integer 0) if otherwise. A point 
	on the edge of a polygon is considered as "inside." */

int insidePoly(polygonType *poly, int x, int y)
{
 
  int numVerts = poly->n_verts; // number of vertices in the polygon

  // determine a random point outside of the window

  //random exterior x value
  float outsideVertX = glutGet(GLUT_WINDOW_WIDTH) + rand();
  // random exterior y value
  float outsideVertY = glutGet(GLUT_WINDOW_HEIGHT) + rand();
  
  int counter = 0;			// counter for the main loop
  int intersectCount = 0;	// counter for the intersection

  float firstVertX;  // x value of first vertex in edge
  float firstVertY;  // y value of first vertex in edge

  float secondVertX; // x value of second vertex in edge
  float secondVertY; // y value of second vertex in edge

  // variables used for testing to detemine if a point is on a line segment

  float rise;				// used to store the increase in the x value of 
							//	a line segment
  float run;				// used to store the increase in the y value of
							// a line segment
  float lineSlope;			// the slope of a line segment on a polygon
  float testSlope;			// the slope of the line segment from
							// the test point to the exterior point
  float yIntercept;			// the y-intercept of the line segment
  int verticalLine = FALSE; // used to record whether a line segment is 
							// vertical

  // iterate through the vertices in the polygon

  while(counter < numVerts) {

    firstVertX = poly->verts[counter % numVerts][0];
	firstVertY = poly->verts[counter % numVerts][1];
	secondVertX = poly->verts[(counter + 1) % numVerts][0];
	secondVertY = poly->verts[(counter + 1) % numVerts][1];

	// if the point is one of the vertices of the polygon, automatically 
	// return TRUE

	if((firstVertX <= (float) x + EPSILON) && 
	  ((float) x - EPSILON <= firstVertX) &&

	   (firstVertY <= (float) y + EPSILON) && 
	   ((float) y - EPSILON <= firstVertY)){

	  return TRUE;

	}

   // find the slope of the line segment, if it is not vertical
   run = secondVertX
	   - firstVertX;

   if (!(0 - EPSILON <= run && run <= 0 + EPSILON)) {

	  // calculate the rise and slope of the line, if it is
	  // not vertical
	  rise = secondVertY
	       - firstVertY;
	  lineSlope = rise/run;
	  verticalLine = FALSE;

	  // edges which are parallel to the test line are difficult to 
	  // deal with, since the test line may go straight through the edge;
	  // thus, if the slope of the edge and the test line are identical,
	  // we recursively call this method, thus generating a new
	  // random test line

	  testSlope = (outsideVertY - (float) y)/(outsideVertX - (float) x);
	  if(testSlope - EPSILON <= lineSlope && lineSlope <= testSlope + EPSILON){

		return insidePoly(poly, x, y);

	  }

	}
	else {

	  verticalLine = TRUE;

	}
			
	// use the helper function intersectionCheck to determine if there is an 
	// intersection between the line segment from the test point to the point 
	// outside of the window, and the one from the current vertex to the next one

	if(intersectionCheck((float) x, (float) y, 
						  outsideVertX, outsideVertY, 
						  firstVertX, firstVertY,
						  secondVertX, secondVertY)) {



	  // instances in which the test line intersects a polygon at one of the 
	  // vertices (when the test point in question is not that particular vertex)
	  // are difficult to handle, since they technically intersect two adjacent
	  // line segments.  Thus, if the intersection point is identical to the
	  // current vertex in question (and the intersection point is not that 
	  // vertex, in which case it would have returned TRUE above), then we
	  // recursively call this method with the test method, thereby generating
	  // a new random test line

	  if((firstVertX <= xIntersect + EPSILON) && 
	  (xIntersect - EPSILON <= firstVertX) &&

	   (firstVertY <= yIntersect + EPSILON) && 
	   (yIntersect - EPSILON <= firstVertY)){

	    return insidePoly(poly, x, y);

	  } 

      // determine if the given point is on the current line segment; if it is,
	  // return TRUE

	  if(verticalLine == TRUE) {

	    // if the point is on the vertical line segment, return TRUE
		if(xIntersect - EPSILON <= (float) x && (float) x <= xIntersect + EPSILON){

		  return TRUE;

		}
	  }
	  else {

		// calculate the y-intercept of the line
		yIntercept = firstVertY - 
				    (lineSlope * firstVertX);
		  
		// return true if the given point is on the line segment
		if((float) y - EPSILON <= lineSlope * xIntersect + yIntercept &&
			lineSlope * xIntersect + yIntercept <= (float) y + EPSILON) {
	
		  return TRUE;

		}
	  }

      // if it is not on the line segment, then simply 
	  // increment the intersect counter

	  intersectCount++;
			  
    }

    counter++;
 
  }

  // return 0 (FALSE) if there is an even number of intersections, 
  // 1 (TRUE) otherwise

  return(intersectCount % 2);

}


/* intersectionCheck takes the start and end points of two line segments 
[(x1, x2) and (x3,x4)] and returns TRUE if they intersect, FALSE if otherwise */

int intersectionCheck(float x1, float y1, 
					  float x2, float y2,
					  float x3 ,float y3,
					  float x4 ,float y4) {
 
  // represent the equations for the two lines in the form Ax + By = C and
  //													   Dx + Ey = F

  float A = y2 - y1;				// x coefficient for the first equation
  float B = x1 - x2;				// y coefficient for the first equation
  float C = (A * x1) + (B * y1);	// C value for the first equation
  float D = y4 - y3;				// x coefficient for the second equation
  float E = x3 - x4;				// y coefficient for the second equation
  float F = (D * x3) + (E * y3);	// F value for the second equation

  float xMin;	// smallest x value on a line segment
  float xMax;	// largest x value on a line segment
  float yMin;	// smallest y value on a line segment 
  float yMax;	// largest y value on a line segment

  // use the determinant of the matrix [ A B ] to determine if the matrix is singular;
  //								   [ D E ]
  // i.e, there is no solution (unless the lines are identical)

  float  matrixDeterminant = (A * E) - (D * B);
  int onFirst;
  int onSecond;

  if(!(0 - EPSILON <= matrixDeterminant && matrixDeterminant <= 0 + EPSILON)) {

    // The the determinant != 0, the matrix is non-singular, so there is a solution (the lines intersect, but not 
    // necessarily on the line segments)

	// calculate the intersection point
    xIntersect = ((E * C) - (B * F))/matrixDeterminant;
    yIntersect = ((A * F) - (D * C))/matrixDeterminant;

	// determine if the intersection point is on each line, by checking to 
	// see if its x and y values are between the minimum and maximum x and 
	// y values of each line segment

    xMin = min(x1,x2);
    xMax = max(x1,x2);   

    yMin = min(y1,y2);
    yMax = max(y1,y2);

    onFirst = ((xMin - EPSILON <= xIntersect) && 
			   (xIntersect <= xMax + EPSILON)) && 
			  ((yMin - EPSILON <= yIntersect) && 
			   (yIntersect <= yMax + EPSILON));

    xMin = min(x3,x4);
    xMax = max(x3,x4);   

    yMin = min(y3,y4);
    yMax = max(y3,y4);
  
    onSecond = ((xMin - EPSILON <= xIntersect) && 
		        (xIntersect <= xMax + EPSILON)) && 
			   ((yMin - EPSILON <= yIntersect) && 
			    (yIntersect <= yMax + EPSILON));

	// if it is on both lines, then return TRUE

    if(onFirst && onSecond){

	  return TRUE;

    }
    else{

      return FALSE;

    }

  }
  else {

    // If the determinant is 0, the matrix is singular, meaning that there is no solution (the lines are parallel)

    return FALSE;

  }

}