/* 
 * CSC533 HomeWork 3
 * Group Member: Xudong Weng, Laiyong Mu
 *
 * Extra features:
 *
 * Operations:
 * 	F1: Open/Close the mouse control
 * 	F2: Open/Close the map
 * 	->: look at right.
 * 	<-: look at left.
 * 	up: zoom in for one level.
 * 	down: zoom out for one level.
 * 	wasd: move to four directions.
 * 	r: move upward.
 * 	f: move downward.
 * 	q: look at sky.
 * 	e: look at ground.
 * 	x: reset all parameters.
 * 	1: exit.
 *
 * Accompanied programs [raster.cxx, raster.h, raster-jpeg.cxx] 
 * are from "Paintdemo" provided by the instructor.
 */

#include <stdio.h>
#include <math.h>
#include <GL/glu.h>       
#include <GL/glut.h> 
#include <math.h>     
#include "raster.h"

/* Function Prototypes and Global Variables */
static ByteRaster *gimage[7];        // pixel buffer for 6 jpeg picture

extern ByteRaster *read_jpeg_image(const char *filename);
void ReadInFile(int face);           // read jpeg file to buffer
static char *infile[7];              // names of the pictures' file

void mainDisplay();                  // draw in main window through gl functions
void subDisplay();

static void key_CB(unsigned char key, int x, int y);     // Called on key press 

float Psize = 1.0f;            // normalized size of a picture
GLuint texture[6];                  // texture ID for different planes
GLfloat rotation_matrix[16];        // rotation matrix, may be used in rotation
int refreshMills = 15;              // refresh interval in milliseconds   
GLfloat angleGL = 0.0f;             // Rotational angle
static int rotateAxis2 = 1;         // 1 = Y axis, -1 = diagonal axis

/********* Parameters of the scenm *********
 * the scene is start from (0,0) to (34,25)*
 * viewer start from (8, -3.5)
 * viewer look at the direction of (0,1,0)
 ******************************************/
 // mouse location
int xmouse= 0;
int ymouse= 0;
int xmouseOld= 0;
int ymouseOld= 0;
GLboolean mouseFlag = GL_TRUE;
GLboolean mouseMode = GL_TRUE;
float mouseSpeed = 0.3f;
float mouseControl = 6.0f;

// always look staright forward in y direction even the viewer moves.float g_look[0] = g_eye[0];
float   g_eye[3] = {8.0,-3.5,0.5};
float   g_look[3] = {8.0, -2.5, 0.5};
float	rad_xz;	
// start angle should be relevant to start g_eye 
float	g_Angle = 283.0;	
float	g_elev = 0.0;

// speed of move and rotate
float   speed = 0.5f;
float rotateSpeed = 2.0f;

// distance between block and pic
float disofBlock = 9.0;
float disofPic = 2.0;

// sub map
int winMain;
int winSub;
GLboolean subMap = GL_FALSE;

// zoom in/out, the level of zoon is 1.
int zoom_level_limit = 1;
GLboolean zoomin = GL_TRUE;
GLboolean zoomout = GL_TRUE;
int zoom_level = 0;
float zoomspeed = 10.0f;
// remember the eye location before zoom in and zoom out.
float zoom_eye[3] = {0,0,0};

/* initiation */
void initGL() {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set background color to black and opaque
	glClearDepth(1.0f);                   // Set background depth to farthest
	
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

/* load textures from image buffer for GL mode */
void LoadGLTextures(int k) {
    // Create texture 
    glGenTextures(k+1, &texture[k]);
    glBindTexture(GL_TEXTURE_2D, texture[k]); // binding the 2D texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image,
    // y size from image, border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, gimage[k]->width(), gimage[k]->height(),
            0, GL_RGB, GL_UNSIGNED_BYTE, gimage[k]->data);
    glEnable(GL_TEXTURE_2D);                      // Enable Texture Mapping
    glShadeModel(GL_SMOOTH);                      // Enable Smooth Shading
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);         // Grey Background
    glClearDepth(1.0f);                           // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);                      // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);                       // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // Really Nice Perspective Calculations
} 

// draw a pic in a block.
void DrawOnePic(float x, float y)
{
    LoadGLTextures(0);
    glBegin(GL_QUADS);
    		// set the normal of this face for lighting
		glNormal3d(0, -1, 0);
		// right top
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + Psize, y, Psize);
		// left top
		glTexCoord2f(Psize, 0.0f); glVertex3f(x, y, Psize);
		// left bottom
		glTexCoord2f(Psize, Psize); glVertex3f(x, y, 0.0);
		// right bottom
		glTexCoord2f(0.0f, Psize); glVertex3f(x + Psize, y, 0.0);  
    glEnd();

}

void DrawOneBlockbyImposter()
{
	return;
}

// draw a block of pic based on it's position [x,y].
void DrawOneBlock(float x, float y)
{
	// viewer position and where the viewer looks at
	float dis;
	
	if(0 /* when calling the imposter */){
		DrawOneBlockbyImposter();
	}else{
		int xpic = 4, ypic = 4; 
		float bx = x*disofBlock;
		float by = y*disofBlock;
		int i;
		for(i = 0; i < xpic; i++){
			int j;
			for(j = 0; j < ypic; j++){
				float cx = i*disofPic;
				float cy = j*disofPic;
				DrawOnePic(bx + cx, by + cy);
			}
		}
	}
}

// draw the scene
void Draw(float Psize)
{
	// number of blocks in rows and cols:
	int xblock = 2, yblock = 2;

	// ??? this is for the angle of the posters
	float Rotate;
	
	DrawOnePic(5,5);
	/*
	int i;
	for(i = 0; i < xblock; i++){
		int j;
		for(j = 0; j < yblock; j++){
			DrawOneBlock((float)i, (float)j);
		}
	}
	*/
	
/*
    // apply textures to the cube
	// Top face of the cube
    LoadGLTextures(4);
    glBegin(GL_QUADS);
    		// set the normal of this face
		glNormal3d(0, 0, 1);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-Psize, -Psize,  Psize);
		glTexCoord2f(Psize, 0.0f); glVertex3f( Psize, -Psize,  Psize);
		glTexCoord2f(Psize, Psize); glVertex3f( Psize,  Psize,  Psize);
		glTexCoord2f(0.0f, Psize); glVertex3f(-Psize,  Psize,  Psize);  
    glEnd();

    // Bottom Face of the cube
    LoadGLTextures(1);
    glBegin(GL_QUADS);
    		// set the normal of this face
		glNormal3d(0, 0, -1);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(Psize, -Psize, -Psize);
		glTexCoord2f(Psize, 0.0f); glVertex3f(-Psize, -Psize, -Psize);
		glTexCoord2f(Psize, Psize); glVertex3f(-Psize,  Psize, -Psize);
		glTexCoord2f(0.0f, Psize); glVertex3f( Psize, Psize, -Psize);  
    glEnd();

*/
    // Front Face of the cube
/*  
  // Back Face of the cube
    LoadGLTextures(2);
    glBegin(GL_QUADS);
    		// set the normal of this face
		glNormal3d(0, -1, 0);
		glTexCoord2f(Psize, Psize); glVertex3f(Psize, -Psize, Psize);
		glTexCoord2f(0.0f, Psize); glVertex3f(-Psize, -Psize, Psize);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-Psize, -Psize, -Psize);
		glTexCoord2f(Psize, 0.0f); glVertex3f(Psize, -Psize, -Psize);  
    glEnd();
    // Right Face of the cube
    LoadGLTextures(5);
    glBegin(GL_QUADS);
    		// set the normal of this face
		glNormal3d(1, 0, 0);
		glTexCoord2f(Psize, 0.0f); glVertex3f(Psize, -Psize, -Psize);
		glTexCoord2f(Psize, Psize); glVertex3f( Psize,  Psize, -Psize);
		glTexCoord2f(0.0f, Psize); glVertex3f( Psize,  Psize,  Psize);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( Psize, -Psize,  Psize);  
    glEnd();  
    // Left Face of the cube
    LoadGLTextures(3);
    glBegin(GL_QUADS);
    		// set the normal of this face
		glNormal3d(-1, 0, 0);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-Psize, -Psize, -Psize);
		glTexCoord2f(Psize, 0.0f); glVertex3f(-Psize, -Psize,  Psize);
		glTexCoord2f(Psize, Psize); glVertex3f(-Psize,  Psize,  Psize);
		glTexCoord2f(0.0f, Psize); glVertex3f(-Psize,  Psize, -Psize);  
    glEnd();
*/
}


// track the mouse to decide the view
static void Mouse(int x, int y){		
	
	xmouseOld = xmouse;
	ymouseOld = ymouse;
	
	xmouse = x;
	ymouse = y;
	
	// use the mouseControl to make sure the mouse operation are stable
	float tempx = xmouse - xmouseOld;
	float tempy = ymouse - ymouseOld;

	if(abs(tempx) - mouseControl > 0.0 )
		if(tempx > 0.0)
			tempx = mouseControl;
		else
			tempx = -mouseControl;

	if(abs(tempy) - mouseControl > 0.0 )
		if(tempy > 0.0)
			tempy = mouseControl;
		else
			tempy = -mouseControl;

	g_Angle +=  -(tempx)*mouseSpeed;            
	g_elev  += (tempy)*mouseSpeed;          
	
	xmouseOld=xmouse;
	ymouseOld=ymouse;
	
	// turn into angle
	rad_xz = float (g_Angle/180.0f);	
	
	if (g_elev<-360)	
     		g_elev  =-360;
	if (g_elev> 360)		   
		g_elev  = 360;

	// g_eye[1] =VIEW_HEIGHT;

	g_look[0] = (float)(g_eye[1] +100*cos(rad_xz));
	g_look[1] = (float)(g_eye[0] +100*sin(rad_xz));
	g_look[2] = g_eye[2] - g_elev;	
	
	glutPostRedisplay();
}

// update the looking position 
void zoomIn(){
	zoom_eye[0] = g_eye[0]; 
	zoom_eye[1] = g_eye[1];
	g_eye[1]+=(float)sin(rad_xz)*zoomspeed;	
	g_eye[0]+=(float)cos(rad_xz)*zoomspeed;

}

void zoomOut(){
	zoom_eye[0] = g_eye[0]; 
	zoom_eye[1] = g_eye[1];
	g_eye[1]-=(float)sin(rad_xz)*zoomspeed;	
	g_eye[0]-=(float)cos(rad_xz)*zoomspeed;

}

void zoomBack(){
	g_eye[0] = zoom_eye[0];
	g_eye[1] = zoom_eye[1];
}

// mouse switcher
void Mouse_CB(int x, int y){
	if(mouseMode == GL_TRUE)
		Mouse(x, y);
	else
		return;
}

// sub map display 
void subDisplay(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);    // Clear screen and Z-buffer

	glMatrixMode(GL_MODELVIEW);     
    
	glLoadIdentity();                  // Reset transformations
	LoadGLTextures(6);
	glBegin(GL_QUADS);
	    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
	    // left top
	    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
	    // left bottom
	    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
	    // right bottom
	    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);  
	glEnd();

	/* ??? draw a pixel of the the point in the map
        int i, j;
	for (i = 0; i < gimage[6]->height (); i++)
	{
		for (j = 0; j < gimage[6]->width (); j++)
		{
		}
	}
	*/

	glViewport(0, 0, 170, 125);     // set up the display window coordinates
	                                                                                         /* how object is mapped to window */
	glMatrixMode(GL_PROJECTION);    // To operate on the Projection matrix
	glLoadIdentity();     
	glFlush();
	glutSwapBuffers();

}

// open the sub map
void openSubMap(){
	winSub = glutCreateSubWindow(winMain,1024-190,20,170,125);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutDisplayFunc(subDisplay);
}

// destroy sub map
void destroySubMap(){
	glutDestroyWindow(winSub);
}

void reset()
{
	xmouse= 0;
	ymouse= 0;
	xmouseOld= 0;
	ymouseOld= 0;
	mouseFlag = GL_TRUE;
	mouseMode = GL_TRUE;

	// always look staright forward in y direction even the viewer moves.float g_look[0] = g_eye[0];
	g_eye[0] = 8.0;
	g_eye[1] =-3.5;
	g_eye[2] = 0.5;
	
	g_look[0] = 8.0;
	g_look[1] = -2.5;
	g_look[2] = 0.5;
	// start angle should be relevant to start g_eye 
	g_Angle = 283.0;	
	g_elev = 0.0;

	// zoom in/out, the level of zoon is 1.
	GLboolean zoomin = GL_TRUE;
	GLboolean zoomout = GL_TRUE;
	int zoom_level = 0;
	// remember the eye location before zoom in and zoom out.
	float zoom_eye[3] = {0,0,0};
}

/* Main window display function */
void mainDisplay(){
	
	glEnable(GL_LIGHTING);              // Enable lighting
	GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f}; 
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	// set up a direct light source
	glEnable(GL_LIGHT0);
	GLfloat lightpos[] = {0.0, 0.0, 1.0, 0.};
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);    // Clear screen and Z-buffer
	
	glMatrixMode(GL_MODELVIEW);     
    
	glLoadIdentity();                  // Reset transformations
  
	// init the look direction based on the init eye position
	rad_xz = float (g_Angle/180.0f);
	g_look[0] = (float)(g_eye[0] +100*cos(rad_xz));
	g_look[1] = (float)(g_eye[1] +100*sin(rad_xz));

	// pin point the mouse direction and lookat direction
	gluLookAt(g_eye[0], g_eye[1], g_eye[2], g_look[0], g_look[1], g_look[2], 0., 0., 1.);   // set up the eye/camera position

	Draw(Psize);
	glLoadIdentity();                // Reset the model-view matrix 
	glFlush();                       //Complete any pending operations 
	glutSwapBuffers();               // Swap the front and back frame buffers (double buffering)
	
	glViewport(0, 0, 1024, 768);     // set up the display window coordinates
        glMatrixMode(GL_PROJECTION);    // To operate on the Projection matrix
	glLoadIdentity();               // Reset matrix
        gluPerspective(45.0f, 1.0f, 0.1f, 100.0f); // Enable perspective projection with fovy, aspect, zNear and zFar
}

/* Function is called on a key press */
static void key_CB(unsigned char key, int x, int y) 
{
	
    switch (key)
	{
		// just move for to four directions, 
		// s = backward;
		case 's':
			g_eye[1]-=(float)sin(rad_xz)*speed;	
			g_eye[0]-=(float)cos(rad_xz)*speed;
			if(zoomout == GL_TRUE){
				zoom_eye[1]-=(float)sin(rad_xz)*speed;	
				zoom_eye[0]-=(float)cos(rad_xz)*speed;
			}
			break;
		// w = forward;
		case 'w':
			g_eye[1]+=(float)sin(rad_xz)*speed;	
			g_eye[0]+=(float)cos(rad_xz)*speed;
			if(zoomin == GL_TRUE){
				zoom_eye[1]+=(float)sin(rad_xz)*speed;	
				zoom_eye[0]+=(float)cos(rad_xz)*speed;
			}
			break;
		// a = left
		case 'a':
			g_eye[0]-=(float)sin(rad_xz)*speed;	
			g_eye[1]+=(float)cos(rad_xz)*speed;
			break;
		// d = right
		case 'd':
			g_eye[0]+=(float)sin(rad_xz)*speed;	
			g_eye[1]-=(float)cos(rad_xz)*speed;
			//g_look[0] = g_eye[0];
			break;
		// wa, wd
		// q and e not implemented yet, Don't touch
		case 'q':
			g_look[2] = g_look[2] + rotateSpeed;
			break;
		case 'e':
			g_look[2] = g_look[2] - rotateSpeed;
			break;
		// move upward
		case 'r':
			g_eye[2]+=speed;
			break;
		// move downward
		case 'f':
			g_eye[2]-=speed;
			break;
		// reset all
		case 'x':
			reset();
			break;
		// exit
		case '1':
			exit(0);
			break;
		default: 
			break;
	}
	glutPostRedisplay (); // state has changed: tell GLUT to redraw
}

// use arrow to contral the look
static void SpecialKey_CB(int key, int x, int y)
{
	switch (key)
	{
		// change the angle of view and call look to update
		case GLUT_KEY_LEFT:
			g_Angle += rotateSpeed;
			break;
		case GLUT_KEY_RIGHT:
			g_Angle -= rotateSpeed;
			break;
		// Open or close the mouse control
		case GLUT_KEY_F1:
			if(mouseFlag == GL_TRUE){
				mouseMode = GL_FALSE;
				mouseFlag = GL_FALSE;
			}else{
				mouseMode = GL_TRUE;
				mouseFlag = GL_TRUE;
			}
			break;
		// zome in zome out
		case GLUT_KEY_F2:
			if(subMap == GL_TRUE){
				destroySubMap();
				subMap = GL_FALSE;
			}else{
				openSubMap();
				subMap = GL_TRUE;
			}
			break;
		case GLUT_KEY_UP:
			zoom_level++;	
			if(zoom_level == 0){
				zoomBack();
				zoomin = GL_TRUE;
				zoomout = GL_TRUE;
				break;
			}
			else if(zoom_level >= 1){
				if(zoomin == GL_TRUE)
					zoomIn();
				zoomin = GL_FALSE;
				zoom_level = 1;
			}
			break;
		case GLUT_KEY_DOWN:
			zoom_level--;	
			if(zoom_level == 0){
				zoomBack();
				zoomin = GL_TRUE;
				zoomout = GL_TRUE;
				break;
			}
			else if(zoom_level <= -1){
				if(zoomout == GL_TRUE)
					zoomOut();
				zoomout = GL_FALSE;
				zoom_level = -1;
			}
			break;
		default:
			break;
	}
	glutPostRedisplay ();
}

/* refresh timer */
void timer(int value) {
	glutPostRedisplay();                   // Post re-draw request to activate display function
	glutTimerFunc(refreshMills, timer, 0); // next timer call in xx milliseconds later
}

/* main() function */
int main(int argc, char* argv[]){
   	
	glutInit(&argc,argv);                         //  Initialize GLUT and process user parameters
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  //  Request double buffered true color window with Z-buffer
	glutInitWindowSize(1024, 768);                 // Set the window's initial width & height
	glutInitWindowPosition(100, 100);               // Position the window's initial top-left corner
	winMain = glutCreateWindow("3D Map");   // Create window

	ReadInFile(7); // Read jpeg images to pixel buffer
	
	glutPassiveMotionFunc(Mouse_CB);
        glutDisplayFunc(mainDisplay);
	glutKeyboardFunc(key_CB);
	glutSpecialFunc(SpecialKey_CB);
	initGL();                       // OpenGL initialization
	glutTimerFunc(0, timer, 0);     // First timer call immediately 
	glutMainLoop();
 
        return 0;                       
}

// Read in all the jpg file to image array
void ReadInFile(int face){
	infile[4] = "1.jpg";
	infile[5] = "2.jpg";
	infile[1] = "3.jpg";
	infile[3] = "4.jpg";
	infile[0] = "6.jpg";
	infile[2] = "5.jpg";
	infile[6] = "map.jpg";
	int i;
	for(i = 0; i < face; i++)
		gimage[i] = read_jpeg_image(infile[i]);
}

