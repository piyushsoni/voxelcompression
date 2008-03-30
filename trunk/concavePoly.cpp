/*******************************************************************
						  concavePoly.c
********************************************************************
    Author: Kyle Olszewski (based on the template by Stan Sclaroff)
      Date: Modified September 20, 2006 (originally written 
			September 9, 2004)
	 Class: CS480, Introduction to Computer Graphics
Assignment: Programming Assignment 1 - OpenGL, 2D Graphics, Polygons
  Due Date: September 25, 2005, 11:59 PM
 Problem #: 2. Write a function that determines whether a polygon is
			concave.
  Comments: 

	concavePoly accepts a pointer to a polygonType, and returns 
	TRUE (the integer 1) if it is concave, FALSE (the integer 0) if 
	otherwise.  It iterates through the vertices finding the
	cross products of each set of adjacent edges.  It records whether
	it has encountered a negative cross product and/or a positive
	cross product.  If it encounters both, then the polygon is
	concave, and it thus returns TRUE; if by the time it has reached
	the last vertex, it has not encountered both, then the polygon
	is convex, so it returns FALSE.
	
	findCrossProduct is a helper function which takes the points 
	on two adjacent lines and returns their cross product.  It uses the 
	method outlined in class of multiplying the x and y values of two 
	vectors to obtain their cross product.
********************************************************************/

#include "const.h"
#include "types.h"
#include "funcs.h"

/* concavePoly.c accepts a pointer to a polygonType, and returns TRUE 
   if it is concave, FALSE if otherwise. */

int concavePoly(polygonType *poly)
{
  
  int numVerts = poly->n_verts; // number of vertices in the polygon

  int posCross = FALSE; // posCross will be TRUE if at least one cross 
						// product is positive
  int negCross = FALSE; // negCross will be TRUE if at least one cross
						// product is negative
  int counter = 0;		// counter for main loop
  
  float firstVertX;     // x value of starting point of first line
  float firstVertY;		// y value of starting point of first line

  float secondVertX;	// x value of end point of first line
						// and starting point of second line
  float secondVertY;	// y value of end point of first line
						// and starting point of second line

  float thirdVertX;		// x value of end point of second line
  float thirdVertY;		// y value of end point of second line

  float crossProduct;	// cross product of the line segments

  // iterate through the vertices in the polygon

  while(counter < numVerts){
  
	  // find the vertex values of the selected line segments

	  firstVertX = poly->verts[counter % numVerts][0];
	  firstVertY = poly->verts[counter % numVerts][1];

	  secondVertX = poly->verts[(counter + 1) % numVerts][0];
	  secondVertY = poly->verts[(counter + 1) % numVerts][1];

	  thirdVertX = poly->verts[(counter + 2) % numVerts][0];
	  thirdVertY = poly->verts[(counter + 2) % numVerts][1];

	  // find the cross product

	  crossProduct = findCrossProduct(firstVertX, firstVertY, 
									  secondVertX, secondVertY, 
									  thirdVertX, thirdVertY);

	  if(crossProduct > 0){
  
		  posCross = TRUE;

	  }
	  else if(crossProduct < 0){

		  negCross = TRUE;

	  }

      // if there is are at least one positive and negative cross 
	  // product, the polygon is concave

	  if(posCross && negCross){

	   	  return TRUE;

	  }

	  counter++;

  }

  // if there are not at least one positive and negative cross product,
  // then the polygon is convex

  return FALSE;
}

/* findCrossProduct takes the points on two adjacent lines and returns 
   their cross product. */

float findCrossProduct(float firstVertX, float firstVertY, 
					   float secondVertX, float secondVertY, 
					   float thirdVertX, float thirdVertY) {

  // find the vector values of each line segment by finding
  // the difference of the vertex values

  // first edge x vector
  float firstEdgeX = secondVertX - firstVertX;
  // first edge y vector
  float firstEdgeY = secondVertY - firstVertY;

  // second edge x vector
  float secondEdgeX = thirdVertX - secondVertX;
  // second edge y vector
  float secondEdgeY = thirdVertY - secondVertY;

  // use the formula X1*Y2 - Y1*X2 to find the cross product 
  // of the two vectors
  float crossProduct = (firstEdgeX * secondEdgeY) - 
					   (firstEdgeY * secondEdgeX);	

  return crossProduct;

}