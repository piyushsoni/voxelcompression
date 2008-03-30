/*******************************************************************
						drawConcavePoly.c
********************************************************************
    Author: Kyle Olszewski (based on the template by Stan Sclaroff)
      Date: Modified September 20, 2006 (originally written 
			September 9, 2004)
	 Class: CS480, Introduction to Computer Graphics
Assignment: Programming Assignment 1 - OpenGL, 2D Graphics, Polygons
  Due Date: September 25, 2005, 11:59 PM
 Problem #: 3.  Write a function that subdivides a concave polygon 
			into convex polygons and draws them (via openGL).
  Comments: 

  drawConcavePoly is a method which accepts a pointer to a
  polygonType which is assumed to be concave, and subdivides the
  polygon into a number of new convex polygons.  

  The method first iterates through the vertices, taking the
  crossProduct of each edge using the helper function 
  findCrossProduct.  If takes the sum of these values and determines
  if the sum is positive, in which case the vertices are ordered
  counterclockwise, or negative, in which case they are clockwise.
  Using this result, it determines if the concave angles have a 
  negative or a positive cross product.  It then searches for a
  vertex between two edges in a convex angle; it takes that vertex
  and the adjacent vertices and creates a new triangular polygon.

  If no vertices other than the triangles vertices lie in this
  polygon, it is drawn, and all of the vertices except for the central
  vertex of the triangle are placed in a new polygon.  If this
  new polygon is concave, then this method is recursively called upon it;
  otherwise, the method drawConvexPoly is called.

  Should the method fail for some reason to successfuly create
  and draw two new polygons, the method drawConvexPoly will be called
  to draw a non-divided concave polygon.  (This is to ensure that the
  polygon will not simply disappear should there be an error.)
********************************************************************/

#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "const.h"
#include "types.h"
#include "funcs.h"

/* drawConcavePoly is a method which accepts a pointer to a
   polygonType which is assumed to be concave, creates a number of
   new convex polygons which subdivide the concave polygon. */

void drawConcavePoly(polygonType *poly) {

  int numVerts = poly->n_verts; // number of vertices in the polygon
  float polyArea = 0;			// area of the polygon
  int counter = 0;				// counter for main loops
  int counter2 = 0;				// counter for inner loops

  float firstVertX;     // x value of starting point of first line
  float firstVertY;		// y value of starting point of first line

  float secondVertX;	// x value of end point of first line
						// and starting point of second line
  float secondVertY;	// y value of end point of first line
						// and starting point of second line

  float thirdVertX;		// x value of end point of second line
  float thirdVertY;		// y value of end point of second line

  float length1;		// length of first edge
  float length2;		// length of second edge

  int specialVal;		// used to record whether the cross product
						// value of concave angles is positive or
						// negative

  float specialVerts[MAX_NUM_VERTS];  // records whether each vertex
									  // has a positive or negative
									  // cross product

  float crossProduct;				  // cross product of two edges

  int forwardOne;					  // vertex after current vertex
  int backOne;						  // vertex before current vertex

  int noVertsInside = TRUE;			  // used to test if a new polygon
									  // has any vertices inside of it
  int polygonDrawn = FALSE;			  // used to record whether a new
									  // convex polygon has been 
									  // successfully drawn

  polygonType *triangle;			  // pointer for new triangle
  polygonType *newPoly;				  // pointer for new polygon

  // iterate through the vertices, taking the sum of the unit vectors
  // of the cross products of each set of adjacent edges

  while(counter < numVerts) {

    // find the values of each vertex

	firstVertX = poly->verts[counter % numVerts][0];
	firstVertY = poly->verts[counter % numVerts][1];

	secondVertX = poly->verts[(counter + 1) % numVerts][0];
	secondVertY = poly->verts[(counter + 1) % numVerts][1];

	thirdVertX = poly->verts[(counter + 2) % numVerts][0];
	thirdVertY = poly->verts[(counter + 2) % numVerts][1];

	// determine the cross product of the adjacent edges

	crossProduct = findCrossProduct(firstVertX, firstVertY, 
									secondVertX, secondVertY, 
									thirdVertX, thirdVertY);

	// divide the crossProduct by the length of the first line
	// times the length of the second line to obtain a unit vector;
	// add this value to the current cross product sum

	length1 = sqrt(((thirdVertX - secondVertX)*
					(thirdVertX - secondVertX)) + 
				   ((thirdVertY - secondVertY)*
					(thirdVertY - secondVertY)));

	length2 = sqrt(((firstVertX - secondVertX)*
					(firstVertX - secondVertX)) + 
				   ((firstVertY - secondVertY)*
					(firstVertY - secondVertY)));

	polyArea = polyArea + 
			  (crossProduct/(length1 * length2));
	
	// record in the array specialVerts whether the
	// angle of the given vertex is positive, negative,
	// or 0

	if(crossProduct > 0) {

	  specialVerts[(counter + 1) % numVerts] = 1;

	}else{
	
	  if (crossProduct < 0) {

	    specialVerts[(counter + 1) % numVerts] = -1;

	  }else {

		specialVerts[(counter + 1) % numVerts] = 0;

	  }
    }

	counter++;

  }

  // if the sum of the cross product is positive, then the angles
  // which have a negative cross product are concave; if the sum
  // is negative, then the angles which have a positive cross product
  // are concave

  polyArea = polyArea/2;
  
  if (polyArea >= 0) {

	specialVal = -1;

  }
  else {

	specialVal = 1;

  }

  counter = 0;

  // iterate through the vertices

  while(counter < numVerts) {

	// if an angle has a cross product whose sign is opposite
	// of that of the concave angles, then that vertex is convex

    if(specialVerts[counter % numVerts] == -specialVal) {

	  // find the nearest vertices before and after this vertex

	  forwardOne = (counter + 1) % numVerts;

	  backOne = (counter + (numVerts - 1)) % numVerts;

	  // create a triangle from the given vertices

	  triangle = createPoly();
	  addVert(triangle, poly->verts[backOne][0], poly->verts[backOne][1]);
	  addVert(triangle, poly->verts[counter % numVerts][0], poly->verts[counter % numVerts][1]);
	  addVert(triangle, poly->verts[forwardOne][0], poly->verts[forwardOne][1]);

      // determine if any vertices other than those of the triangle
	  // lie on or within its edges

	  noVertsInside = TRUE;

	  counter2 = 0;

	  while(counter2 < numVerts) {
		
	    if(insidePoly(triangle, poly->verts[counter2 % numVerts][0], poly->verts[counter2 % numVerts][1])){
			
		  if(!((counter2 % numVerts == counter % numVerts) || 
			   (counter2 % numVerts == backOne) || 
			   (counter2 % numVerts == forwardOne))) {
					
			noVertsInside = FALSE;
			break;

		  }
		}

		counter2++;

	  }

	  // if no other vertices are within the triangle, create a new
	  // polygon using all of the vertices except for the central vertex
	  // of the triangle

	  counter2 = 0;

	  if(noVertsInside){

	    newPoly = createPoly();

	    while(counter2 < numVerts){

		  if((counter2 % numVerts != counter % numVerts)) {

		    addVert(newPoly, poly->verts[counter2 % numVerts][0], poly->verts[counter2 % numVerts][1]);

		  }

		  counter2++;
			
		}

		// draw the triangle

		drawConvexPoly(triangle);

		// check to see if the new polygon is concave; if it is,
		// recursively call this method

		if(concavePoly(newPoly)) {

		  drawConcavePoly(newPoly);

		}
		else {

		  // otherwise, draw a convex polygon

		  drawConvexPoly(newPoly);

		}

		// assert that new polygons have been drawn
		polygonDrawn = TRUE;
		break;
	  } 

    }

	counter++;
    
  } 

  // if for any reason we fail to find a triangle to divide from the
  // main polygon, we simply draw an undivided concave polygon

  if(!polygonDrawn){

	drawConvexPoly(poly);

  }

}