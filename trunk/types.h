/*******************************************************************
	       Basic Type Definitions for CS480 Program 1
********************************************************************
    Author: Stan Sclaroff
      Date: September 9, 2004
  Comments: This is just an example. 
            If you prefer, you are welcome to create your own
            type definitions or C++ class definitions for the
            assignment. 
********************************************************************/


/* color */
typedef struct {
  float r,g,b;
} colorType;

  
/* polygon */
typedef struct {
  int n_verts;
  int concave;
  int selected_vert;
  float verts[MAX_NUM_VERTS][2]; 
  colorType color;
} polygonType;

