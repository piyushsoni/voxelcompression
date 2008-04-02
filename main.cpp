#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <cassert>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <imdebuggl.h>
#include <math.h>
#include "graphics3d.h"
#include "openglhelper.h"

extern "C"
{

#include "const.h"
#include "types.h"
#include "funcs.h"

}

using namespace std;
using namespace Pi;

//// WINDOW PROPERTIES /////////////////
#define WINDOW_TITLE "OpenGL - Getting Started"
int winWidth  = 512;
int winHeight = 512;
#define VOLUME_DIMENSION 512
#define PEARL_SUBDIVISION_LEVEL 5

enum EventType {ENTRY, EXIT};

struct Node{

	float voxelx;
	float voxely; 
	float voxelz; 
	float voxelxSq;
	float voxelySq;
	float voxelzSq;

	struct Node *next;
	//Pearl *currentPearl, char *touched, int *voxCount

};

bool touched[VOLUME_DIMENSION][VOLUME_DIMENSION][VOLUME_DIMENSION];

class Pearl;
class Event;
class Active;
void readModelFile( char * filename , Pearl **pearlListHead, int *numPearls);
void readLine( FILE * fp, char * str);
int write_rgb_ppm(unsigned char *buff, int width, int height, char *fpath);
void subdivide(Pearl* pearlListHead, int *numPearls, int *numSubPearls);
void createEntrySchedule(Pearl *subPearlListHead, Event **eventListHead);
void addToSchedule(Pearl *currentSubPearl, Event **eventListHead);
void insert(Event *insertEvent, Event **eventListHead);
int SubdivisionDone = 0;
int TotalPearlCount=0;
int numSubPearls=0;

float fmax(float a, float b);

class Pearl{

	public:
		
		Pearl();
		Pearl(int num, Point3D Center, float radius, int numChildren)
		{
			pearlNum=num;
			this->Center=Center.Clone();
			this->radius= radius;
			this->radSquared = radius*radius;
			this->numChildren=numChildren;
			next=NULL;
			prev = NULL;
		}

		Pearl(int num, float x, float y, float z, float radius, int numChildren)
		{
			pearlNum = num;
			this->Center.x=x;
			this->Center.y=y;
			this->Center.z=z;
			this->radius = radius;
			this->radSquared = radius*radius;
			this->numChildren = numChildren;
			next = NULL;
			prev = NULL;
		}
		~Pearl();
		int pearlNum;
		float radius, radSquared;
		Point3D Center; 
		int numChildren;
		Pearl *next;
		Pearl *prev;

};
Pearl* pearlListTail=NULL; 

class Event{

public:

	Event();
	Event(EventType evt, float val, Pearl *pearl);
	~Event();

	EventType evt;
	Pearl *pearl;
	float val;
	Event *next;

};

class Active{

public:

	Active();
	Active(Pearl *pearl);
	~Active();

	float currentRadiusSquared, currentZ;

	Pearl *pearl;
	Active *next;

};

Active::Active(Pearl *pearl){

	this->pearl = pearl;
	this->next = NULL;
	this->currentRadiusSquared = 0.0;
	this->currentZ = -(pearl->radius);

}

Active::Active(){

	this->next = NULL;

}



//// GLOBALS ///////////////////////////
static int oldMouseX,oldMouseY;
static int currentMouseX,currentMouseY;
static bool isLeftMouseActive;
static bool isRightMouseActive;

float zDepth = -5.0f;
float xTranslate = 0.0f;
float yTranslate = 0.0f;
float xRotation  = 0.0f;
float yRotation  = 0.0f;

// Forward Declaration
void exitGL(int returnCode);

GLuint g_fbo;
GLuint g_texture;

GLint triFan;

void CheckFrameBufferStatus()
{
	GLenum eStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch (eStatus)
	{
		// everything is OK
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		break;

		// Unsupported Format
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
		cout << "Error: Unsupported Format" << endl;
		break;

		// Other Error
	default:
		assert(0);
		break;
	}
}

void InitFBO()
{
	// Create FBO
	glGenFramebuffersEXT(1, &g_fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_fbo);

	// Initialize Texture
	glGenTextures(1, &g_texture);
	glBindTexture(GL_TEXTURE_2D, g_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	// Attach Texture to Framebuffer Color Buffer
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texture, 0);

	// check status?
}

void ReleaseFBO()
{
	glDeleteFramebuffersEXT(1, &g_fbo);

}

//// OPENGL EXTENSION CHECKING //////////////////////////////////////////////////
void InitGLExtensions()
{
	// Init GLEW
	if (glewInit() != GLEW_OK)
	{
		cout << "Error: Can't initialize GLEW" << endl;
		exitGL(-1);
	}

	if (!GLEW_EXT_framebuffer_object)
	{
		cout << "Error: EXT_framebuffer_object is not supported." << endl;
		exitGL(-1);
	}
}

//// OPENGL FUNCTIONS ///////////////////////////////////////////////////////////
void exitGL(int returnCode)
{
	ReleaseFBO();
	exit(returnCode);
}

void initGL()
{
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	// Init OpenGL Extensions
	//InitGLExtensions();

	// Init FBO
	//InitFBO();

	////////////
	//GLint triFan;
	triFan = glGenLists(1);

	glNewList(triFan,GL_COMPILE);

	glBegin(GL_TRIANGLE_FAN);

	glVertex3f(0,0,0);

	int i;
	for(i = 0; i <= 360; i += 10){

		glVertex2f(-sin(i*(3.14159/180)),cos(i*(3.14159/180)));

	}

	glEnd();

	glEndList();

}

//// GLUT CALLBACK /////////////////////
void glutDisplay()
{
	static int i = 0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 512, 512, 0);

	glMatrixMode(GL_MODELVIEW);

	// render to texture
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_fbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// place camera/eye
	//glTranslatef(xTranslate,yTranslate,zDepth);
	glTranslatef(256,256,0);
	glRotatef(-yRotation,1.0,0.0,0.0);
	glRotatef(-xRotation,0.0,1.0,0.0);
	//glScalef(256,256,256);
	glCallList(triFan);

	// render to screen
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 512, 512, 0);

	// switch back to modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_texture);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(512.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(512.0f, 512.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 512.0f);

	glEnd();
	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();

	///
	//FILE *pFile; // File pointer
	//TGAHEADER tgaHeader; // TGA file header
	unsigned long lImageSize; // Size in bytes of image
	static GLbyte *pBits = NULL; // Pointer to bits
	GLint iViewport[4]; // Viewport in pixels
	GLenum lastBuffer; // Storage for the current read buffer setting

	// Get the viewport dimensions
	glGetIntegerv(GL_VIEWPORT, iViewport);
	// How big is the image going to be (targas are tightly packed)
	lImageSize = iViewport[2] * 3 * iViewport[3];
	// Allocate block. If this doesn’t work, go home
	//pBits = (GLbyte *)malloc(lImageSize);
	if(pBits == NULL)
		pBits = (GLbyte *)calloc(lImageSize,1);
	if(pBits == NULL)
		return;

	int k,j;
	for(k = 0; k < lImageSize; k++){

		pBits[k] = 7;

	}

	glFinish();

	// Read bits from color buffer
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	// Get the current read buffer setting and save it. Switch to
	// the front buffer and do the read operation. Finally, restore
	// the read buffer state
	glGetIntegerv(GL_READ_BUFFER, (GLint *) &lastBuffer);
	glReadBuffer(GL_FRONT);


	glReadPixels(0, 0, iViewport[2], iViewport[3], GL_BGR,GL_UNSIGNED_BYTE, pBits);

	glReadBuffer(lastBuffer);

	for(k = 0; k < lImageSize; k++){

		if(pBits[k] == 7){

			printf("Blank! %d", k);
			while(1){};
		}

		if(pBits[k]){

			printf("Yes! %d", k);
			while(1){};
		}

	}
	printf("NO! %d", k);
	while(1){};

}

void glutIdle()
{
	glutPostRedisplay();
}

void glutReshape(int width,int height)
{
	winWidth  = width;
	winHeight = height;
	glViewport(0,0,width,height);

	// set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60.0f,(GLfloat)width/(GLfloat)height,0.1f,200.0f);
	gluOrtho2D(0, width, height, 0);

	// switch back to modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void glutKeyboard(unsigned char key,int x,int y)
{
	if (key == 27) exit(0);
}

void glutMouse(int button,int state,int x,int y)
{
	if (state == GLUT_UP)
		switch (button)
	{
		case GLUT_LEFT_BUTTON:
			isLeftMouseActive = false;
			break;
		case GLUT_RIGHT_BUTTON:
			isRightMouseActive = false;
			break;
	}

	if (state == GLUT_DOWN)
	{
		oldMouseX = currentMouseX = x;
		oldMouseY = currentMouseY = y;

		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			isLeftMouseActive = true;
			break;
		case GLUT_RIGHT_BUTTON:
			isRightMouseActive = true;
			break;
		}

	}
}

void glutMouseMotion(int x,int y)
{
	currentMouseX = x;
	currentMouseY = y;

	if (isLeftMouseActive && isRightMouseActive)
	{
		xTranslate += (currentMouseX - oldMouseX)/100.0f;
		yTranslate -= (currentMouseY - oldMouseY)/100.0f;
	}
	else if (isLeftMouseActive)
	{
		xRotation -= (currentMouseX - oldMouseX);
		yRotation -= (currentMouseY - oldMouseY);
	}
	else if (isRightMouseActive)	
		zDepth -= (currentMouseY - oldMouseY)/10.f;	

	oldMouseX = currentMouseX;
	oldMouseY = currentMouseY;
}

// send Voxel

void sendVoxel(float voxelx, float voxely, float voxelz, 
			   float voxelxSq, float voxelySq, float voxelzSq, 
			   Pearl *currentPearl, char *touched, int *voxCount){

	printf("Good one! %d %f %f %f %f %f %f %f %f\n", *voxCount, voxelx, voxely, voxelz, sqrt(voxelxSq + voxelySq + voxelzSq), currentPearl->Center.x, currentPearl->Center.y, currentPearl->Center.z, currentPearl->radius);
	// send voxel and mark as Sent
	//touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(voxelx + currentPearl->Center.x)) + (int) (VOLUME_DIMENSION*floor(voxely + currentPearl->Center.y)) + (int) (floor(voxelz + currentPearl->Center.z))] = 1;
	//(*voxCount)++;

	float newPos;
	float newDist;

	// x up

	newDist = voxelxSq + voxelx + voxelx + 1;
	newPos = voxelx + 1;
	//newDist = voxelxSq - newPos - newPos - 1;

	if(((newDist + voxelySq + voxelzSq) <= currentPearl->radSquared) && 
	  !(touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(newPos + currentPearl->Center.x)) 
	          + (int) (VOLUME_DIMENSION*floor(voxely + currentPearl->Center.y)) 
			  + (int) (floor(voxelz + currentPearl->Center.z))])){

		touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(newPos + currentPearl->Center.x)) 
			  + (int) (VOLUME_DIMENSION*floor(voxely + currentPearl->Center.y)) 
			  + (int) (floor(voxelz + currentPearl->Center.z))] = 1;

		(*voxCount)++;
		sendVoxel( newPos, voxely, voxelz, newDist, voxelySq, voxelzSq, currentPearl, touched, voxCount);

	}

	// x down

	newDist = voxelxSq - voxelx - voxelx + 1;
	newPos = voxelx - 1;
	//newDist = voxelxSq - newPos - newPos - 1;
	//newPos = voxelx - 1;
	//newDist = voxelxSq - newPos - newPos - 1;

	if(((newDist + voxelySq + voxelzSq) <= currentPearl->radSquared) && 
	   !(touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(newPos + currentPearl->Center.x)) 
	           + (int) (VOLUME_DIMENSION*floor(voxely + currentPearl->Center.y)) 
			   + (int) (floor(voxelz + currentPearl->Center.z))])){

		touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(newPos + currentPearl->Center.x)) 
			  + (int) (VOLUME_DIMENSION*floor(voxely + currentPearl->Center.y)) 
			  + (int) (floor(voxelz + currentPearl->Center.z))] = 1;
		(*voxCount)++;
		sendVoxel( newPos, voxely, voxelz, newDist, voxelySq, voxelzSq, currentPearl, touched, voxCount);

	}

	// y up

	newDist = voxelySq + voxely + voxely + 1;
	newPos = voxely + 1;
	//newDist = voxelxSq - newPos - newPos - 1;

	if(((voxelxSq + newDist + voxelzSq) <= currentPearl->radSquared) && 
	   !(touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(voxelx + currentPearl->Center.x)) 
	           + (int) (VOLUME_DIMENSION*floor(newPos + currentPearl->Center.y)) 
			   + (int) (floor(voxelz + currentPearl->Center.z))])){

		touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(voxelx + currentPearl->Center.x)) 
			  + (int) (VOLUME_DIMENSION*floor(newPos + currentPearl->Center.y)) 
			  + (int) (floor(voxelz + currentPearl->Center.z))] = 1;
		(*voxCount)++;
		sendVoxel( voxelx, newPos, voxelz, voxelxSq, newDist, voxelzSq, currentPearl, touched, voxCount);

	}

	// y down

	newDist = voxelySq - voxely - voxely + 1;
	newPos = voxely - 1;
	//newDist = voxelxSq - newPos - newPos - 1;
	//newPos = voxelx - 1;
	//newDist = voxelxSq - newPos - newPos - 1;

	if(((voxelxSq + newDist + voxelzSq) <= currentPearl->radSquared) && 
	   !(touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(voxelx + currentPearl->Center.x)) 
	           + (int) (VOLUME_DIMENSION*floor(newPos + currentPearl->Center.y)) 
			   + (int) (floor(voxelz + currentPearl->Center.z))])){

		touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(voxelx + currentPearl->Center.x)) 
			  + (int) (VOLUME_DIMENSION*floor(newPos + currentPearl->Center.y)) 
			  + (int) (floor(voxelz + currentPearl->Center.z))] = 1;
		(*voxCount)++;
		sendVoxel( voxelx, newPos, voxelz, voxelxSq, newDist, voxelzSq, currentPearl, touched, voxCount);

	}

	// z up

	newDist = voxelzSq + voxelz + voxelz + 1;
	newPos = voxelz + 1;
	//newDist = voxelxSq - newPos - newPos - 1;

	if(((voxelxSq+ voxelySq + newDist) <= currentPearl->radSquared) && 
	   !(touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(voxelx + currentPearl->Center.x)) 
	           + (int) (VOLUME_DIMENSION*floor(voxely + currentPearl->Center.y)) 
			   + (int) (floor(newPos + currentPearl->Center.z))])){

		touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(voxelx + currentPearl->Center.x)) 
			  + (int) (VOLUME_DIMENSION*floor(voxely + currentPearl->Center.y)) 
			  + (int) (floor(newPos + currentPearl->Center.z))] = 1;
		(*voxCount)++;
		sendVoxel( voxelx, voxely, newPos, voxelx, voxelySq, newDist, currentPearl, touched, voxCount);

	}

	// z down

	newDist = voxelzSq - voxelz - voxelz + 1;
	newPos = voxelz - 1;
	//newDist = voxelxSq - newPos - newPos - 1;

	if(((voxelxSq+ voxelySq + newDist) <= currentPearl->radSquared) && 
	   !(touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(voxelx + currentPearl->Center.x)) 
	           + (int) (VOLUME_DIMENSION*floor(voxely + currentPearl->Center.y)) 
			   + (int) (floor(newPos + currentPearl->Center.z))])){

		touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(voxelx + currentPearl->Center.x)) 
			  + (int) (VOLUME_DIMENSION*floor(voxely + currentPearl->Center.y)) 
			  + (int) (floor(newPos + currentPearl->Center.z))] = 1;
		(*voxCount)++;
		sendVoxel( voxelx, voxely, newPos, voxelx, voxelySq, newDist, currentPearl, touched, voxCount);

	}

}

//// MAIN PROGRAM //////////////////////
int main(int argc,char **argv)
{

	////////////////////////////////////////////
/*
	glutInit(&argc,argv);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(winWidth,winHeight);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(WINDOW_TITLE);

	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);
	glutReshapeFunc(glutReshape);
	glutKeyboardFunc(glutKeyboard);
	glutMouseFunc(glutMouse);
	glutMotionFunc(glutMouseMotion);

	initGL();
*/
	////////////////////////////////////////////

	Pearl *pearlListHead = NULL;
	int numPearls = 0;

	//Pearl *subPearlListHead = NULL;
	int numSubPearls = 0;

	///////////////////////////////////////////
	// read input file

	readModelFile("angiography.prl", &pearlListHead, &numPearls);

	subdivide(pearlListHead, &numPearls, &numSubPearls);

	Pearl *currentPearl = pearlListHead;

	//short touched[VOLUME_DIMENSION][VOLUME_DIMENSION][VOLUME_DIMENSION];

	//char *touched = (char *) calloc(VOLUME_DIMENSION*VOLUME_DIMENSION*VOLUME_DIMENSION*sizeof(char),1);
	int voxCount = 0;

	/*
	if(!touched){

		printf("CALLOC FAILED!");
		while(1){};

	}
	*/

	printf("sending...");

	float prevPearlCenter[3];

	//prevPearlCenter[0] = currentPearl->Center.x;
	//prevPearlCenter[1] = currentPearl->Center.y;
	//prevPearlCenter[2] = currentPearl->Center.z;

	int firstPearl = 1;

	float vector[3];

	LARGE_INTEGER ticksPerSecond;
	LARGE_INTEGER tick1;   // A point in time
	LARGE_INTEGER tick2;
	//LARGE_INTEGER difference;
	float difference;
	//LARGE_INTEGER sum;
	float sum = 0;
	//float finishTime;
	LARGE_INTEGER startTime;   // For converting tick into real time
	LARGE_INTEGER finishTime;

	// get the high resolution counter's accuracy
	QueryPerformanceFrequency(&ticksPerSecond);

	/*******/

	QueryPerformanceCounter(&startTime);

	while(currentPearl){
		
		//vector[0] = currentPearl->Center.x - currentPearl->next->Center.x;
		//vector[1] = currentPearl->Center.y - currentPearl->next->Center.y;
		//vector[2] = currentPearl->Center.z - currentPearl->next->Center.z;

		if(currentPearl->next){
		vector[0] = -currentPearl->Center.x + currentPearl->next->Center.x;
		vector[1] = -currentPearl->Center.y + currentPearl->next->Center.y;
		vector[2] = -currentPearl->Center.z + currentPearl->next->Center.z;
		}
		//vector[0] = currentPearl->Center.x - prevPearlCenter[0];
		//vector[1] = currentPearl->Center.y - prevPearlCenter[1];
		//vector[2] = currentPearl->Center.z - prevPearlCenter[2];

		//prevPearlCenter[0] = currentPearl->Center.x;
		//prevPearlCenter[1] = currentPearl->Center.y;
		//prevPearlCenter[2] = currentPearl->Center.z;

		float length = sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);

		vector[0] = vector[0]/length;
		vector[1] = vector[1]/length;
		vector[2] = vector[2]/length;

		float firstVoxel[3];

		//if(firstPearl){

		//	firstVoxel[0] = 0;
		//	firstVoxel[1] = 0;
		//	firstVoxel[2] = 0;
		//	firstPearl = 0;
		//}
		//else{

		firstVoxel[0] = currentPearl->radius*vector[0];
		firstVoxel[1] = currentPearl->radius*vector[1];
		firstVoxel[2] = currentPearl->radius*vector[2];

		//}

		//firstVoxel[0] = currentPearl->Center.x + currentPearl->radius*vector[0];
		//firstVoxel[1] = currentPearl->Center.y + currentPearl->radius*vector[1];
		//firstVoxel[2] = currentPearl->Center.z + currentPearl->radius*vector[2];

		//float firstRadSquared = currentPearl->radius*currentPearl->radius;
		//float secRadSquared = currentPearl->next->radius*currentPearl->next->radius;

		//sendVoxel(firstVoxel[0], firstVoxel[1], firstVoxel[2], , , currentPearl, &touched);

		//touched[(int) (VOLUME_DIMENSION*VOLUME_DIMENSION*floor(firstVoxel[0] + currentPearl->Center.x)) + (int) (VOLUME_DIMENSION*floor(firstVoxel[1] + currentPearl->Center.y)) + (int) (floor(firstVoxel[2] + currentPearl->Center.z))] = 1;
		//voxCount++;

		//sendVoxel(firstVoxel[0], firstVoxel[1], firstVoxel[2], firstVoxel[0]*firstVoxel[0], firstVoxel[1]*firstVoxel[1], firstVoxel[2]*firstVoxel[2], currentPearl, touched, &voxCount);

		struct Node *queueHead = (struct Node *) malloc(sizeof(struct Node));
		struct Node *oldQueueHead = NULL;
		struct Node *queueEnd  = queueHead; 

		queueHead->voxelx = firstVoxel[0];
		queueHead->voxely = firstVoxel[1];
		queueHead->voxelz = firstVoxel[2];

		queueHead->voxelxSq = firstVoxel[0]*firstVoxel[0];
		queueHead->voxelySq = firstVoxel[1]*firstVoxel[1];
		queueHead->voxelzSq = firstVoxel[2]*firstVoxel[2];

		queueHead->next = NULL;

		touched[(int) (floor(queueHead->voxelx + currentPearl->Center.x))]
			   [(int) (floor(queueHead->voxely + currentPearl->Center.y))]
			   [(int) (floor(queueHead->voxelz + currentPearl->Center.z))] = 1;

		voxCount++;

		while(queueHead){
			// send

			// print?
			
			//if(!touched[(int) (floor(queueHead->voxelx + currentPearl->Center.x))]
			//	   [(int) (floor(queueHead->voxely + currentPearl->Center.y))]
			//	   [(int) (floor(queueHead->voxelz + currentPearl->Center.z))]){

			//touched[(int) (floor(queueHead->voxelx + currentPearl->Center.x))]
			//	   [(int) (floor(queueHead->voxely + currentPearl->Center.y))]
			//	   [(int) (floor(queueHead->voxelz + currentPearl->Center.z))] = 1;
			//voxCount++;

			float newPos;
			float newDist;
			float upPos;
			float upDist;
			float downPos;
			float downDist;

			// up

			upDist = queueHead->voxelxSq + queueHead->voxelx + queueHead->voxelx + 1;
			upPos = queueHead->voxelx + 1;

			if(((upDist + queueHead->voxelySq + queueHead->voxelzSq) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(upPos             + currentPearl->Center.x))] 
					 [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					 [(int) (floor(queueHead->voxelz + currentPearl->Center.z))])){

				struct Node *queueNew = (struct Node *) malloc(sizeof(struct Node));
				*queueNew = *queueHead;

				queueNew->voxelx = upPos;
				queueNew->voxelxSq = upDist;
				queueNew->next = NULL;

				// add to queue
				queueEnd->next = queueNew;
				queueEnd = queueNew;

				touched[(int) (floor(upPos             + currentPearl->Center.x))] 
					   [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					   [(int) (floor(queueHead->voxelz + currentPearl->Center.z))] = 1;
				voxCount++;
			}

			// down

			downDist = queueHead->voxelxSq - queueHead->voxelx - queueHead->voxelx + 1;
			downPos = queueHead->voxelx - 1;

			if(((downDist + queueHead->voxelySq + queueHead->voxelzSq) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(downPos           + currentPearl->Center.x))] 
					 [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					 [(int) (floor(queueHead->voxelz + currentPearl->Center.z))])){

				struct Node *queueNew = (struct Node *) malloc(sizeof(struct Node));
				*queueNew = *queueHead;

				queueNew->voxelx = downPos;
				queueNew->voxelxSq = downDist;
				queueNew->next = NULL;

				// add to queue
				queueEnd->next = queueNew;
				queueEnd = queueNew;

				touched[(int) (floor(downPos             + currentPearl->Center.x))] 
					   [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					   [(int) (floor(queueHead->voxelz + currentPearl->Center.z))] = 1;
				voxCount++;
			}

					 
			// north +z

			newDist = queueHead->voxelzSq + queueHead->voxelz + queueHead->voxelz + 1;
			newPos = queueHead->voxelz + 1;

			while(((queueHead->voxelxSq + queueHead->voxelySq + newDist) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(queueHead->voxelx + currentPearl->Center.x))] 
					 [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					 [(int) (floor(newPos			 + currentPearl->Center.z))])){

				touched[(int) (floor(queueHead->voxelx + currentPearl->Center.x))] 
					   [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					   [(int) (floor(newPos			   + currentPearl->Center.z))] = 1;
				voxCount++;

				// add to queue
				
				if(((upDist + queueHead->voxelySq + newDist) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(upPos       + currentPearl->Center.x))] 
					 [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					 [(int) (floor(newPos + currentPearl->Center.z))])){

				struct Node *queueNew = (struct Node *) malloc(sizeof(struct Node));
				*queueNew = *queueHead;

				queueNew->voxelx = upPos;
				queueNew->voxelxSq = upDist;
				queueNew->voxelz = newPos;
				queueNew->voxelzSq = newDist;
				queueNew->next = NULL;

				// add to queue
				queueEnd->next = queueNew;
				queueEnd = queueNew;

				touched[(int) (floor(upPos       + currentPearl->Center.x))] 
					   [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					   [(int) (floor(newPos + currentPearl->Center.z))] = 1;
				voxCount++;
			}

			// down

			if(((downDist + queueHead->voxelySq + newDist) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(downPos           + currentPearl->Center.x))] 
					 [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					 [(int) (floor(newPos + currentPearl->Center.z))])){

				struct Node *queueNew = (struct Node *) malloc(sizeof(struct Node));
				*queueNew = *queueHead;

				queueNew->voxelx = downPos;
				queueNew->voxelxSq = downDist;
				queueNew->voxelz = newPos;
				queueNew->voxelzSq = newDist;
				queueNew->next = NULL;

				// add to queue
				queueEnd->next = queueNew;
				queueEnd = queueNew;

				touched[(int) (floor(downPos           + currentPearl->Center.x))] 
					   [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					   [(int) (floor(newPos			   + currentPearl->Center.z))] = 1;
			    voxCount++;
			}
				
				
				newDist += (newPos + newPos + 1);
				newPos  += 1;
			}

			// south -z

			newDist = queueHead->voxelzSq - queueHead->voxelz - queueHead->voxelz + 1;
			newPos = queueHead->voxelz - 1;

			while(((queueHead->voxelxSq + queueHead->voxelySq + newDist) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(queueHead->voxelx + currentPearl->Center.x))] 
					 [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					 [(int) (floor(newPos			 + currentPearl->Center.z))])){

				touched[(int) (floor(queueHead->voxelx + currentPearl->Center.x))] 
					   [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					   [(int) (floor(newPos			   + currentPearl->Center.z))] = 1;
				voxCount++;
				
				// add to queue
				
				if(((upDist + queueHead->voxelySq + newDist) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(upPos       + currentPearl->Center.x))] 
					 [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					 [(int) (floor(newPos + currentPearl->Center.z))])){

				struct Node *queueNew = (struct Node *) malloc(sizeof(struct Node));
				*queueNew = *queueHead;

				queueNew->voxelx = upPos;
				queueNew->voxelxSq = upDist;
				queueNew->voxelz = newPos;
				queueNew->voxelzSq = newDist;
				queueNew->next = NULL;

				// add to queue
				queueEnd->next = queueNew;
				queueEnd = queueNew;

				touched[(int) (floor(upPos       + currentPearl->Center.x))] 
					   [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					   [(int) (floor(newPos + currentPearl->Center.z))] = 1;
				voxCount++;
			}

			// down

			if(((downDist + queueHead->voxelySq + newDist) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(downPos           + currentPearl->Center.x))] 
					 [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					 [(int) (floor(newPos + currentPearl->Center.z))])){

				struct Node *queueNew = (struct Node *) malloc(sizeof(struct Node));
				*queueNew = *queueHead;

				queueNew->voxelx = downPos;
				queueNew->voxelxSq = downDist;
				queueNew->voxelz = newPos;
				queueNew->voxelzSq = newDist;
				queueNew->next = NULL;

				// add to queue
				queueEnd->next = queueNew;
				queueEnd = queueNew;

				touched[(int) (floor(downPos           + currentPearl->Center.x))] 
					 [(int) (floor(queueHead->voxely + currentPearl->Center.y))] 
					 [(int) (floor(newPos + currentPearl->Center.z))] = 1;
				voxCount++;
			}
			

				newDist += (-newPos - newPos + 1);
				newPos  -= 1;

			}

			// east +y

			newDist = queueHead->voxelySq + queueHead->voxely + queueHead->voxely + 1;
			newPos = queueHead->voxely + 1;

			while(((queueHead->voxelxSq + newDist + queueHead->voxelzSq) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(queueHead->voxelx + currentPearl->Center.x))] 
					 [(int) (floor(newPos		     + currentPearl->Center.y))] 
					 [(int) (floor(queueHead->voxelz + currentPearl->Center.z))])){

				touched[(int) (floor(queueHead->voxelx + currentPearl->Center.x))] 
					   [(int) (floor(newPos			   + currentPearl->Center.y))] 
					   [(int) (floor(queueHead->voxelz + currentPearl->Center.z))] = 1;
				voxCount++;

				// add to queue
				
				if(((upDist + newDist + queueHead->voxelzSq) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(upPos       + currentPearl->Center.x))] 
					 [(int) (floor(newPos + currentPearl->Center.y))] 
					 [(int) (floor(queueHead->voxelz + currentPearl->Center.z))])){

				struct Node *queueNew = (struct Node *) malloc(sizeof(struct Node));
				*queueNew = *queueHead;

				queueNew->voxelx = upPos;
				queueNew->voxelxSq = upDist;
				queueNew->voxely = newPos;
				queueNew->voxelySq = newDist;
				queueNew->next = NULL;

				// add to queue
				queueEnd->next = queueNew;
				queueEnd = queueNew;

				touched[(int) (floor(upPos       + currentPearl->Center.x))] 
					 [(int) (floor(newPos + currentPearl->Center.y))] 
					 [(int) (floor(queueHead->voxelz + currentPearl->Center.z))] = 1;
			     voxCount++;
			}

			// down

			if(((downDist + newDist + queueHead->voxelzSq) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(downPos       + currentPearl->Center.x))] 
					 [(int) (floor(newPos + currentPearl->Center.y))] 
					 [(int) (floor(queueHead->voxelz + currentPearl->Center.z))])){

				struct Node *queueNew = (struct Node *) malloc(sizeof(struct Node));
				*queueNew = *queueHead;

				queueNew->voxelx = downPos;
				queueNew->voxelxSq = downDist;
				queueNew->voxely = newPos;
				queueNew->voxelySq = newDist;
				queueNew->next = NULL;

				// add to queue
				queueEnd->next = queueNew;
				queueEnd = queueNew;

				touched[(int) (floor(downPos       + currentPearl->Center.x))] 
					 [(int) (floor(newPos + currentPearl->Center.y))] 
					 [(int) (floor(queueHead->voxelz + currentPearl->Center.z))] = 1;
				voxCount++;

			}

				newDist += (newPos + newPos + 1);
				newPos  += 1;
			}

			// east -y

			newDist = queueHead->voxelySq - queueHead->voxely - queueHead->voxely + 1;
			newPos = queueHead->voxely - 1;

			while(((queueHead->voxelxSq + newDist + queueHead->voxelzSq) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(queueHead->voxelx + currentPearl->Center.x))] 
					 [(int) (floor(newPos		     + currentPearl->Center.y))] 
					 [(int) (floor(queueHead->voxelz + currentPearl->Center.z))])){

				touched[(int) (floor(queueHead->voxelx + currentPearl->Center.x))] 
					   [(int) (floor(newPos			   + currentPearl->Center.y))] 
					   [(int) (floor(queueHead->voxelz + currentPearl->Center.z))] = 1;
				voxCount++;

				// add to queue
				
				if(((upDist + newDist + queueHead->voxelzSq) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(upPos       + currentPearl->Center.x))] 
					 [(int) (floor(newPos + currentPearl->Center.y))] 
					 [(int) (floor(queueHead->voxelz + currentPearl->Center.z))])){

				struct Node *queueNew = (struct Node *) malloc(sizeof(struct Node));
				*queueNew = *queueHead;

				queueNew->voxelx = upPos;
				queueNew->voxelxSq = upDist;
				queueNew->voxely = newPos;
				queueNew->voxelySq = newDist;
				queueNew->next = NULL;

				// add to queue
				queueEnd->next = queueNew;
				queueEnd = queueNew;

				touched[(int) (floor(upPos       + currentPearl->Center.x))] 
					 [(int) (floor(newPos + currentPearl->Center.y))] 
					 [(int) (floor(queueHead->voxelz + currentPearl->Center.z))] = 1;
				voxCount++;
			}

			// down

			if(((downDist + newDist + queueHead->voxelzSq) <= currentPearl->radSquared) && 
			!(touched[(int) (floor(downPos       + currentPearl->Center.x))] 
					 [(int) (floor(newPos + currentPearl->Center.y))] 
					 [(int) (floor(queueHead->voxelz + currentPearl->Center.z))])){

				struct Node *queueNew = (struct Node *) malloc(sizeof(struct Node));
				*queueNew = *queueHead;

				queueNew->voxelx = downPos;
				queueNew->voxelxSq = downDist;
				queueNew->voxely = newPos;
				queueNew->voxelySq = newDist;
				queueNew->next = NULL;

				// add to queue
				queueEnd->next = queueNew;
				queueEnd = queueNew;

				touched[(int) (floor(downPos       + currentPearl->Center.x))] 
					 [(int) (floor(newPos + currentPearl->Center.y))] 
					 [(int) (floor(queueHead->voxelz + currentPearl->Center.z))] = 1;
				voxCount++;

			}
				newDist += (-newPos -newPos + 1);
				newPos  -= 1;
			}
			//}			
			oldQueueHead = queueHead;
			queueHead = queueHead->next;
			free(oldQueueHead);

		}

		currentPearl = currentPearl->next;
		//break;

	}

	/***/

	QueryPerformanceCounter(&finishTime);

	printf("Total Useful Time: %f\n\n", ((finishTime.QuadPart - startTime.QuadPart)/((float) ticksPerSecond.QuadPart)) - sum);

	printf("Average Useful Time: %f\n\n",(((finishTime.QuadPart - startTime.QuadPart)/((float) ticksPerSecond.QuadPart)) - sum)/VOLUME_DIMENSION);	

	/***/

	// get last pearl!
	printf("Voxel Count: %d", voxCount);
	while(1){};
	return 0;
	/************************************/
/*
	// Event *eventListHead = NULL;
	Event *eventListHead = (Event *) new Event(); // dummy head

	createEntrySchedule(pearlListHead, &eventListHead);

	///////////////////////////////////
	// render!

	//////////////////////////////////

	Pearl *currentPearl = pearlListHead;

	int count = 0;

	while(currentPearl){

		printf("Pearl %d: %d %f %f %f %f %d\n\n", count, currentPearl->pearlNum, currentPearl->Center.x, currentPearl->Center.y, currentPearl->Center.z, currentPearl->radius, currentPearl->numChildren);
		currentPearl = currentPearl->next;
		count++;

	}

	printf("Num Pearls: %d\n\n", numPearls);

	////////////////////////////////////

	Pearl *currentSubPearl = subPearlListHead;

	count = 0;



	printf("Num Sub Pearls: %d\n\n", numSubPearls);

	//////////////////////////////////////////////

	Event *currentEvent = eventListHead->next;

	count = 0;
	int enter = 0;
	int exit = 0;

	while(currentEvent){

		if(currentEvent->evt == ENTRY){
			printf("Event %d: ENTER %f PEARL: %d\n\n", count, currentEvent->val, currentEvent->pearl->pearlNum);
			enter++;
		}

		if(currentEvent->evt == EXIT){
			printf("Event %d: EXIT  %f PEARL: %d\n\n", count, currentEvent->val, currentEvent->pearl->pearlNum);
			exit++;
		}

		currentEvent = currentEvent->next;
		count++;

	}

	printf("Num Events: %d Enter: %d Exit: %d\n\n", count,enter,exit);

	////////////////////////////////////////////
	glViewport(0,0,512,512);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 512, 512, 0);

	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glLoadIdentity();

	////////////////////////////////////////////

	Active *activeListHead = (Active *) new Active();
	Active *currentActive = NULL;

	// current event list head
	currentEvent = eventListHead->next;
	enter = 0;
	exit = 0;

	int voxCount = 0;

	int i;
	for(i=0; i < VOLUME_DIMENSION; i++){

		printf("Level: %d\n", i);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// check to see if any new events
		while(currentEvent && currentEvent->val <= (float) i){

			if(currentEvent->evt == ENTRY){
				//printf("Enter: %d\n\n", currentEvent->pearl->pearlNum);
				enter++;
				// add active
				Active *newActive = (Active *) new Active(currentEvent->pearl);
				newActive->next = activeListHead->next;
				activeListHead->next = newActive;

			}
			else{
				//printf("Exit: %d\n\n", currentEvent->pearl->pearlNum);
				exit++;
				// remove inactive
				Active *prevActive = activeListHead;
				Active *searchActive = activeListHead->next;

				while(searchActive->pearl != currentEvent->pearl){
					prevActive = prevActive->next;
					searchActive = searchActive->next;
				}

				// free?
				prevActive->next = searchActive->next;

			}

			currentEvent = currentEvent->next;

		}

		// for all active pearls
		currentActive = activeListHead->next;

		if(!(i%10) && currentActive){

			//printf("Now %d \n\n", i);

		}

		while(currentActive){

			// find current radius value?
			if(!(i%10)){

				//printf("%d Current: %f Actual: %f \n\n", currentActive->pearl->pearlNum, sqrt(currentActive->currentRadiusSquared), sqrt((currentActive->pearl->radius)*(currentActive->pearl->radius) - (currentActive->currentZ)*(currentActive->currentZ)));

			}
			// draw pearl
			////////////
			// render to texture
			glPushMatrix();
			glTranslatef(currentActive->pearl->Center.x,currentActive->pearl->Center.y,0);
			glScalef(sqrt(currentActive->currentRadiusSquared),sqrt(currentActive->currentRadiusSquared),sqrt(currentActive->currentRadiusSquared));
			glCallList(triFan);
			glPopMatrix();

			////////////
			// update radius
			currentActive->currentRadiusSquared -= (currentActive->currentZ + currentActive->currentZ + 1);
			currentActive->currentZ += 1;

			currentActive = currentActive->next;
		}

		///////////////////////////////////////

		//glReadBuffer(GL_FRONT);
		long k, j, bitsize, width;

		GLint		viewport[4];		
		static void		*bits = NULL;			
		GLubyte	*rgb,			
			temp;			


		glGetIntegerv(GL_VIEWPORT, viewport);

		width   = viewport[2] * 3;		
		width   = (width + 3) & ~3;		
		bitsize = width * viewport[3];	

		if(!bits){
			if ((bits = calloc(bitsize, 1)) == NULL)
			{

				printf("Bad Calloc!");
			};
		}

		glFinish();				
		glPixelStorei(GL_PACK_ALIGNMENT, 4);	
		glPixelStorei(GL_PACK_ROW_LENGTH, 0);
		glPixelStorei(GL_PACK_SKIP_ROWS, 0);
		glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

		glReadPixels(0, 0, viewport[2], viewport[3], GL_RGB, GL_UNSIGNED_BYTE,
			bits);

		////////////
		//if(i == 70){
		//printf("%d %d\n", viewport[2], viewport[3]);

		int check[32][32];

		int m,n;

		for(m = 0; m < 32; m++){

			for(n = 0; n < 32; n++){

				check[m][n] = 0;

			}

		}

		///////////

		for (k = 0; k < viewport[3]; k++)
			for (j = 0, rgb = ((GLubyte *)bits) + k * width;
				j < viewport[2];
				j ++, rgb += 3)
			{

				if(rgb[0]){
					voxCount++;
					// store this pixel
					check[k/16][j/16] = 1;
					// printf("Yeah! %d %d %d %d", i, k, j, (rgb)[0]);
					// while(1){};

				}
				else{


				}

			};
		
		if(i > 60 && i < 70){
		printf("%d\n", i);
		for(m = 0; m < 32; m++){

			for(n = 0; n < 32; n++){

				printf("%d ", check[m][n]);

			}

			printf("\n");

		}
		printf("\n\n\n");
		}

		///////////////////////////////////////

	}

	printf("Total Number of Voxels Grabbed: %d\n\n", voxCount);

	printf("Entries: %d, Exits: %d", enter, exit);
*/
	/* register callbacks for window redisplay 
	and reshape event callbacks */
	//glutDisplayFunc(display);
	//glutReshapeFunc(reshape); 

	/* setup mouse handler callbacks */
	//glutMotionFunc(mouseMotion);
	//glutMouseFunc(mouseButton);

	/* setup keyboard handler callback */
	//glutKeyboardFunc(keyboard);

	//initGL();
	//glutMainLoop();
	//exitGL(0);

	//printf("Hey!");

	//while(1){};

	//return 0;
}

float fmax(float a, float b){

	if (a > b){

		return a;

	}

	return b;

}


void createEntrySchedule(Pearl *subPearlListHead, Event **eventListHead){

	Pearl *currentSubPearl = subPearlListHead;

	while(currentSubPearl){

		addToSchedule(currentSubPearl, eventListHead);
		currentSubPearl = currentSubPearl->next;

	}

}

void addToSchedule(Pearl *currentSubPearl, Event **eventListHead){

	float enterPoint = currentSubPearl->Center.z - currentSubPearl->radius;
	float exitPoint = currentSubPearl->Center.z + currentSubPearl->radius;

	// make entry event

	EventType newEventType = ENTRY;

	Event *newEvent = (Event *) new Event(newEventType,enterPoint, currentSubPearl);

	// add to list

	insert(newEvent,eventListHead);

	// make exit event

	newEventType = EXIT;

	newEvent = (Event *) new Event(newEventType,exitPoint, currentSubPearl);

	// add to list

	insert(newEvent,eventListHead);

}

void insert(Event *insertEvent, Event **eventListHead){

	/*
	if(!*eventListHead){

	*eventListHead = insertEvent;
	return;

	}
	*/

	Event *previous = *eventListHead;
	Event *next = (*eventListHead)->next;

	while(next){

		if(insertEvent->val < next->val){

			break;
		}

		previous = previous->next;
		next = next->next;
	}

	previous->next = insertEvent;
	insertEvent->next = next;
}



void readModelFile( char * filename , Pearl **pearlListHead , int *numPearls){

	float pearlCoords[3], radius;
	int numChildren;

	Pearl *currentPearl = NULL;

	FILE * fp;
	//char str[100], str1[1000];
	fp = fopen(filename, "r");
	if( fp == NULL ){
		fprintf(stderr,"Unable to open input file %s",filename);
		exit(1);
	}

	/*******************/
	fscanf(fp,"%f %f %f %f %d ", &pearlCoords[0], &pearlCoords[1], &pearlCoords[2], &radius, &numChildren);
	
	if( feof(fp) ){
		fclose(fp);
		return;
	}
		
	Pearl *newPearl = (Pearl *) new Pearl(*numPearls, pearlCoords[0], pearlCoords[1], pearlCoords[2], radius, numChildren);

	(*numPearls)++;

	currentPearl = (*pearlListHead) = newPearl;
		
	if(!numChildren){
		fclose(fp);
		TotalPearlCount=*numPearls;
		return;
	}
	/*******************/

	while(1){
		
		fscanf(fp,"%f %f %f %f %d ", &pearlCoords[0], &pearlCoords[1], &pearlCoords[2], &radius, &numChildren);
		if( feof(fp) )
			break;
		
		Pearl *newPearl = (Pearl *) new Pearl(*numPearls, pearlCoords[0], pearlCoords[1], pearlCoords[2], radius, numChildren);

		(*numPearls)++;

		currentPearl->next = newPearl;
		newPearl->prev=currentPearl;

		currentPearl = currentPearl->next;
		
		if(!numChildren){
			break;
		}

		//fp = fp;

	}
	TotalPearlCount=*numPearls; 
	pearlListTail = currentPearl;
	fclose(fp);

}


//void subdivide(Pearl* pearlListHead, int *numPearls, int *numSubPearls)
//{	
//	while(1)
//	{
//		if(SubdivisionDone>PEARL_SUBDIVISION_LEVEL) return;
//		++SubdivisionDone;
//		//sphere(-width+SubdivisionDone*50,-height+50,-depth+50,10);
//		Pearl* temp = pearlListHead;
//		//Pearl* subHead = NULL;
//		Pearl *create = NULL;
//		Point3D a,b,c,d;
//		//Create two dummy  points in the start and in the end, using linear prediction. 
//		Vector3D BA = temp->next->Center.VectorTo(temp->Center);
//		Vector3D CB = temp->next->next->Center.VectorTo(temp->next->Center);
//		
//		Point3D start = temp->Center.Clone().TranslateBy(BA);
//		//Point3D start = temp->Center.Clone().TranslateBy(BA.ScaleBy(2).Subtract(CB));
//
//		Point3D end = pearlListTail->Center.Clone().TranslateBy(pearlListTail->prev->Center.VectorTo(pearlListTail->Center));
//		b = temp->Center, c= temp->next->Center, d = temp->next->next->Center;
//		Point3D inserted = start.MidPointWith(d).TranslateBy(start.MidPointWith(d).VectorTo(b.MidPointWith(c)).ScaleBy(9.0/8));
//		
//		//glColor3f(128,128,0);
//		//start.DrawSphere(150);
//		//start.Print("Start"); 
//		temp=temp->next->next;  
//		
//		create = new Pearl(TotalPearlCount++, inserted, (temp->prev->radius+temp->prev->prev->radius)/2,1);
//		(*numSubPearls)++;
//		temp->prev->prev->next=create;
//		create->next=temp->prev;
//		create->prev=temp->prev->prev;
//		
//		temp=temp->next;
//
//		//temp will always point to the 4th point in the 4 point subdivision :)
//		//This is done to prevent two runs on the list - the insertion can be done in one traversal only. 
//		//The problem with simple insertion is - we can't consider new inserted points to generate others. 
//		while(temp)
//		{
//
//
//			a = temp->prev->prev->prev->Center, b= temp->prev->prev->Center, c = temp->prev->Center, d = temp->Center;
//
//			//Correct the wrong pointer of last iteration: 
//			temp->prev->prev->prev=create;
//
//			inserted = a.MidPointWith(d).TranslateBy(a.MidPointWith(d).VectorTo(b.MidPointWith(c)).ScaleBy(9.0/8));
//			create = new Pearl(TotalPearlCount++, inserted, (temp->prev->radius+temp->prev->prev->radius)/2,1);
//			(*numSubPearls)++;
//			if(create==NULL) {cout<<"Error! Can not allocate memory!!"; exit(1);}
//
//			temp->prev->prev->next=create;
//			create->next=temp->prev;
//			create->prev=temp->prev->prev;
//			
//
//			temp=temp->next;
//			
//		}
//		
//		pearlListTail->prev->prev=create;
//		inserted = b.MidPointWith(end).TranslateBy(b.MidPointWith(end).VectorTo(c.MidPointWith(d)).ScaleBy(9.0/8));
//		create = new Pearl(TotalPearlCount++, inserted, (pearlListTail->radius+pearlListTail->prev->radius)/2,1);
//		(*numSubPearls)++;
//		//insert last point
//		create->next=pearlListTail; 
//		create->prev=pearlListTail->prev;
//		pearlListTail->prev->next=create; 
//		pearlListTail->prev=create;
//
//		//drawPearls();
//	}
//}
//
//
//
//
//
//


void subdivide(Pearl* pearlListHead, int *numPearls, int *numSubPearls)
{	
	float tempRad;
	Pearl* temp = NULL; 
	//Pearl* subHead = NULL;
	Pearl *create = NULL;
	Point3D a,b,c,d, start, end, inserted;	
	Vector3D BA, CB;

	while(SubdivisionDone++<PEARL_SUBDIVISION_LEVEL)
	{
		temp = pearlListHead;
		//Create two dummy  points in the start and in the end, using linear prediction. 
		BA= temp->next->Center.VectorTo(temp->Center);
		CB = temp->next->next->Center.VectorTo(temp->next->Center);
		
		start = temp->Center.Clone().TranslateBy(BA);
		//Point3D start = temp->Center.Clone().TranslateBy(BA.ScaleBy(2).Subtract(CB));

		end = pearlListTail->Center.Clone().TranslateBy(pearlListTail->prev->Center.VectorTo(pearlListTail->Center));
		b = temp->Center, c= temp->next->Center, d = temp->next->next->Center;
		inserted = start.MidPointWith(d).TranslateBy(start.MidPointWith(d).VectorTo(b.MidPointWith(c)).ScaleBy(9.0/8));
		
		//glColor3f(128,128,0);
		//start.DrawSphere(150);
		//start.Print("Start"); 
		temp=temp->next->next;  
		//tempRad = (temp->prev->radius+temp->prev->prev->radius)/2; 
		tempRad = 9*(temp->prev->radius+temp->prev->prev->radius)/16 - (temp->radius+temp->prev->prev->radius)/16;
		create = new Pearl(TotalPearlCount++, inserted, tempRad ,1);
		
		//if(maxX<inserted.x+tempRad) maxX=inserted.x+tempRad;
		//else if(minX>inserted.x-tempRad) minX=inserted.x-tempRad;
		//if(maxY<inserted.y+tempRad) maxY=inserted.y+tempRad;
		//else if(minY>inserted.y-tempRad) minY=inserted.y-tempRad;
		//if(maxZ<inserted.z+tempRad) maxZ=inserted.z+tempRad;
		//else if(minZ>inserted.z-tempRad) minZ=inserted.z-tempRad;
		
		(*numSubPearls)++;
		temp->prev->prev->next=create;
		create->next=temp->prev;
		create->prev=temp->prev->prev;
		
		temp=temp->next;

		//temp will always point to the 4th point in the 4 point subdivision :)
		//This is done to prevent two runs on the list - the insertion can be done in one traversal only. 
		//The problem with simple insertion is - we can't consider new inserted points to generate others. 
		while(temp)
		{


			a = temp->prev->prev->prev->Center, b= temp->prev->prev->Center, c = temp->prev->Center, d = temp->Center;

			//Correct the wrong pointer of last iteration: 
			temp->prev->prev->prev=create;

			inserted = a.MidPointWith(d).TranslateBy(a.MidPointWith(d).VectorTo(b.MidPointWith(c)).ScaleBy(9.0/8));
			
			//tempRad = (temp->prev->radius+temp->prev->prev->radius)/2;
			tempRad = 9*(temp->prev->radius+temp->prev->prev->radius)/16 - (temp->radius+temp->prev->prev->prev->radius)/16;
			create = new Pearl(TotalPearlCount++, inserted, tempRad,1);

			//if(maxX<inserted.x+tempRad) maxX=inserted.x+tempRad;
			//else if(minX>inserted.x-tempRad) minX=inserted.x-tempRad;
			//if(maxY<inserted.y+tempRad) maxY=inserted.y+tempRad;
			//else if(minY>inserted.y-tempRad) minY=inserted.y-tempRad;
			//if(maxZ<inserted.z+tempRad) maxZ=inserted.z+tempRad;
			//else if(minZ>inserted.z-tempRad) minZ=inserted.z-tempRad;

			(*numSubPearls)++;
			if(create==NULL) {cout<<"Error! Can not allocate memory!!"; exit(1);}

			temp->prev->prev->next=create;
			create->next=temp->prev;
			create->prev=temp->prev->prev;
			

			temp=temp->next;
			
		}
		
		pearlListTail->prev->prev=create;
		inserted = b.MidPointWith(end).TranslateBy(b.MidPointWith(end).VectorTo(c.MidPointWith(d)).ScaleBy(9.0/8));
		create = new Pearl(TotalPearlCount++, inserted, (pearlListTail->radius+pearlListTail->prev->radius)/2,1);
		(*numSubPearls)++;
		//insert last point
		create->next=pearlListTail; 
		create->prev=pearlListTail->prev;
		pearlListTail->prev->next=create; 
		pearlListTail->prev=create;

		//drawPearls();
	}
}





void readLine( FILE * fp, char * str){
	char a;
	int i=0;
	a = fgetc(fp);
	while( a != '\n' && a != '\r' ){
		str[i++] = a;
		a = fgetc(fp);
	}	
	str[i] = 0;
}


Event::Event(EventType evt, float val, Pearl *pearl){

	this->evt = evt;
	this->val = val;
	this->next = NULL;
	this->pearl = pearl;

}

Event::Event(){

	this->next = NULL;

}

int write_rgb_ppm(unsigned char *buff, int width, int height, char *fpath)
{

	FILE *f;

	/* open the file for writing (overwrite any previous version) */
	if((f = fopen(fpath,"w")) == 0)
	{
		fprintf(stderr,"Cannot open image output file (name = %s)\n",fpath);
		return 0;
	}

	/* the ASCII header of the PPM file */

	fprintf(f,"P6\n");  /* magic number for PPM files */
	fprintf(f,"%i %i\n",width,height); /* dimensions */
	fprintf(f,"255\r");  /* max intensity value */

	/* write the pixels, 3 bytes/pixel */

	fwrite(buff,3,width*height,f);

	fclose(f);

	return 1;
}

/*
struct hitType Cylinder::intersect(struct rayType ray){

hitType hit;
hit.u = INFINITE;
double u1,u2,u3, u4;

double xCoeff;
double yCoeff;
double constant;

double newPoint[4];
double newVect[4];
double solPoint[4];
double coeffMat[4][4];
double length1;

newPoint[0] = ray.point[0] - x;
newPoint[1] = ray.point[1] - y;
newPoint[2] = ray.point[2] - z;
newPoint[3] = 1;

double basisMat[4][4];

basisMat[0][0] = (double) nx;
basisMat[0][1] = (double) ny;
basisMat[0][2] = (double) nz;

if(nx != 0){

basisMat[1][0] = (double) basisMat[0][1];
basisMat[1][1] = (double) -basisMat[0][0];
basisMat[1][2] = 0;

}
else {

basisMat[1][0] = 0;
basisMat[1][1] = (double) -basisMat[0][2];
basisMat[1][2] = (double) basisMat[0][1];

}

crossProduct(basisMat[1], basisMat[0]);

basisMat[2][0] = (double) sol1[0];
basisMat[2][1] = (double) sol1[1];
basisMat[2][2] = (double) sol1[2];

basisMat[3][0] = (double) 0;
basisMat[3][1] = (double) 0;
basisMat[3][2] = (double) 0;

basisMat[0][3] = (double) 0;
basisMat[1][3] = (double) 0;
basisMat[2][3] = (double) 0;

basisMat[3][3] = 1;

length1 = sqrt(basisMat[0][0] * basisMat[0][0] + basisMat[0][1] * basisMat[0][1] + basisMat[0][2] * basisMat[0][2]);

basisMat[0][0] = basisMat[0][0]/length1;
basisMat[0][1] = basisMat[0][1]/length1;
basisMat[0][2] = basisMat[0][2]/length1;

length1 = sqrt(basisMat[1][0] * basisMat[1][0] + basisMat[1][1] * basisMat[1][1] + basisMat[1][2] * basisMat[1][2]);

basisMat[1][0] = basisMat[1][0]/length1;
basisMat[1][1] = basisMat[1][1]/length1;
basisMat[1][2] = basisMat[1][2]/length1;

length1 = sqrt(basisMat[2][0] * basisMat[2][0] + basisMat[2][1] * basisMat[2][1] + basisMat[2][2] * basisMat[2][2]);

basisMat[2][0] = basisMat[2][0]/length1;
basisMat[2][1] = basisMat[2][1]/length1;
basisMat[2][2] = basisMat[2][2]/length1;

matVectMult(basisMat,newPoint);

newPoint[0] = sol2[0];
newPoint[1] = sol2[1];
newPoint[2] = sol2[2];
newPoint[3] = sol2[3];

matVectMult(basisMat,ray.vector);

newVect[0] = sol2[0];
newVect[1] = sol2[1];
newVect[2] = sol2[2];
newVect[3] = sol2[3];

coeffMat[0][0] = 0;
coeffMat[0][1] = 0;
coeffMat[0][2] = 0;
coeffMat[0][3] = 0;
coeffMat[1][0] = 0;
coeffMat[1][1] = 1;
coeffMat[1][2] = 0;
coeffMat[1][3] = 0;
coeffMat[2][0] = 0;
coeffMat[2][1] = 0;
coeffMat[2][2] = 1;
coeffMat[2][3] = 0;
coeffMat[3][0] = 0;
coeffMat[3][1] = 0;
coeffMat[3][2] = 0;
coeffMat[3][3] = -(radius * radius);

matVectMult(coeffMat, newVect);

xCoeff = dotProduct(newVect,sol2);

matVectMult(coeffMat, newPoint);

yCoeff = 2 * (dotProduct(newVect,sol2));

matVectMult(coeffMat, newPoint);
constant = dotProduct(newPoint,sol2);

if((yCoeff*yCoeff - 4*xCoeff*constant) < 0){

}
else {

if((yCoeff*yCoeff - 4*xCoeff*constant) == 0){

u1 = -yCoeff/(2*xCoeff);
u2 = INFINITE;

}
else{

u1 = (-yCoeff + sqrt((yCoeff*yCoeff - 4*xCoeff*constant)))/(2*xCoeff);
u2 = (-yCoeff - sqrt((yCoeff*yCoeff - 4*xCoeff*constant)))/(2*xCoeff);

}

if(u1 > 0 && u1 < hit.u && abs(u1*newVect[0] + newPoint[0]) <= length/2){

hit.u = u1;

}
if(u2 > 0 && u2 < hit.u && abs(u2*newVect[0] + newPoint[0]) <= length/2){

hit.u = u2;

}

////////////////////

//hit.u = 1;//?
//U

solPoint[0] = (newPoint[0] + hit.u * newVect[0]);
solPoint[1] = (newPoint[1] + hit.u * newVect[1]);
solPoint[2] = (newPoint[2] + hit.u * newVect[2]);
solPoint[3] = 1;

matVectMult(coeffMat,solPoint);

hit.surfNorm[0] = sol2[0];
hit.surfNorm[1] = sol2[1];
hit.surfNorm[2] = sol2[2];
hit.surfNorm[3] = 0;

transpose(basisMat);//?

matVectMult(solMat,hit.surfNorm);

hit.surfNorm[0] = sol2[0];
hit.surfNorm[1] = sol2[1];
hit.surfNorm[2] = sol2[2];
hit.surfNorm[3] = 0;

matVectMult(solMat,solPoint);

hit.point[0] = sol2[0] + x; //? solPoint?
hit.point[1] = sol2[1] + y;
hit.point[2] = sol2[2] + z;
hit.point[3] = 1;

hit.objID = id;
hit.matID = matId;

///////////////////

}	

double norm[4];
norm[0] = 1;
norm[1] = 0;
norm[2] = 0;
norm[3] = 0;

u3 = (length/2 - dotProduct(newPoint, norm))/(dotProduct(newVect, norm));

solPoint[0] = newPoint[0] + newVect[0]*u3;
solPoint[1] = newPoint[1] + newVect[1]*u3;
solPoint[2] = newPoint[2] + newVect[2]*u3;
solPoint[3] = newPoint[3] + newVect[3]*u3;

if(u3 > 0 && u3 < hit.u){


if(sqrt(solPoint[1]*solPoint[1] + solPoint[2]*solPoint[2]) <= radius){

hit.u = u3;//?

////////////////////

//hit.u = 1;//?
//U

hit.surfNorm[0] = 1;
hit.surfNorm[1] = 0;
hit.surfNorm[2] = 0;
hit.surfNorm[3] = 0;

transpose(basisMat);//?

matVectMult(solMat,hit.surfNorm);

hit.surfNorm[0] = sol2[0];
hit.surfNorm[1] = sol2[1];
hit.surfNorm[2] = sol2[2];
hit.surfNorm[3] = 0;

matVectMult(solMat,solPoint);

hit.point[0] = sol2[0] + x; //? solPoint?
hit.point[1] = sol2[1] + y;
hit.point[2] = sol2[2] + z;
hit.point[3] = 1;

hit.objID = id;
hit.matID = matId;

///////////////////

}

}

norm[0] = -1;
norm[1] = 0;
norm[2] = 0;
norm[3] = 0;

u4 = (length/2 - dotProduct(newPoint, norm))/(dotProduct(newVect, norm));

solPoint[0] = newPoint[0] + newVect[0]*u4;
solPoint[1] = newPoint[1] + newVect[1]*u4;
solPoint[2] = newPoint[2] + newVect[2]*u4;
solPoint[3] = newPoint[3] + newVect[3]*u4;

if(u4 > 0 && u4 < hit.u){


if(sqrt(solPoint[1]*solPoint[1] + solPoint[2]*solPoint[2]) <= radius){

hit.u = u4;//?

////////////////////

//hit.u = 1;//?
//U

hit.surfNorm[0] = -1;
hit.surfNorm[1] = 0;
hit.surfNorm[2] = 0;
hit.surfNorm[3] = 0;

transpose(basisMat);//?

matVectMult(solMat,hit.surfNorm);

hit.surfNorm[0] = sol2[0];
hit.surfNorm[1] = sol2[1];
hit.surfNorm[2] = sol2[2];
hit.surfNorm[3] = 0;

matVectMult(solMat,solPoint);

hit.point[0] = sol2[0] + x; //? solPoint?
hit.point[1] = sol2[1] + y;
hit.point[2] = sol2[2] + z;
hit.point[3] = 1;

hit.objID = id;
hit.matID = matId;

///////////////////

}

}

return hit;

}
*/