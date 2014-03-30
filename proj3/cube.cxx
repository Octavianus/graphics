/* 
 * CSC533 HomeWork 3
 * Group Member: Xudong Weng, Laiyong Mu
 *
 * Extra features:
 *
 * Operations:
 * 	F1: Open/Close the mouse control
 * 	->: look at right.
 * 	<-: look at left.
 * 	wasd: move to four directions.
 * 	q: look at sky.
 * 	e: look at ground.
 * 	1: exit
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
static ByteRaster *gimage[6];        // pixel buffer for 6 jpeg picture

extern ByteRaster *read_jpeg_image(const char *filename);
void ReadInFile(int face);           // read jpeg file to buffer
static char *infile[6];              // names of the pictures' file

void display();                     // general display function to draw the scene
void GLDisplay();                   // draw in GL mode through gl functions
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

// display will calling GLDisplay
void display(){
	GLDisplay();
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

void look()
{
	rad_xz = float (g_Angle/180.0f);
	g_look[0] = (float)(g_eye[0] +100*cos(rad_xz));
	g_look[1] = (float)(g_eye[1] +100*sin(rad_xz));
}

void Mouse_CB(int x, int y){
	if(mouseMode == GL_TRUE)
		Mouse(x, y);
	else
		return;
}

/* GL mode display function */
void GLDisplay(){
	
	glEnable(GL_LIGHTING);              // Enable lighting
	//GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f}; 
        //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	// set up a direct light source
	glEnable(GL_LIGHT0);
	GLfloat lightpos[] = {0.0, 0.0, 1.0, 0.};
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);    // Clear screen and Z-buffer
	
	glMatrixMode(GL_MODELVIEW);     
    
	glLoadIdentity();                  // Reset transformations
	//glTranslatef(0.0f, 0.0f, -6.5f);   // Move further away from the screen
  
	//glTranslatef(0.0f, 0.0f, 2.0f);  // Move the center of cube the origin before rotation 
	// init 
	rad_xz = float (g_Angle/180.0f);
	g_look[0] = (float)(g_eye[0] +100*cos(rad_xz));
	g_look[1] = (float)(g_eye[1] +100*sin(rad_xz));

	// pin point the mouse direction and lookat direction
	gluLookAt(g_eye[0], g_eye[1], g_eye[2], g_look[0], g_look[1], g_look[2], 0., 0., 1.);   // set up the eye/camera position

	Draw(Psize);
	glLoadIdentity();                // Reset the model-view matrix 
	glFlush();                       //Complete any pending operations 
	glutSwapBuffers();               // Swap the front and back frame buffers (double buffering)
	
	glViewport(0, 0, 600, 600);     // set up the display window coordinates
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
			break;
		// w = forward;
		case 'w':
			g_eye[1]+=(float)sin(rad_xz)*speed;	
			g_eye[0]+=(float)cos(rad_xz)*speed;
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
		//exit
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
		// change the angle of view
		case GLUT_KEY_LEFT:
			g_Angle += rotateSpeed;
			look();
			break;
		case GLUT_KEY_RIGHT:
			g_Angle -= rotateSpeed;
			look();
			break;
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
		case GLUT_KEY_UP:
			
			break;
		case GLUT_KEY_DOWN:
			
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
	glutInitWindowSize(600, 600);                 // Set the window's initial width & height
	glutInitWindowPosition(50, 50);               // Position the window's initial top-left corner
	glutCreateWindow("Scene");   // Create window
	ReadInFile(6);                                // Read jpeg images to pixel buffer
	
	glutPassiveMotionFunc(Mouse_CB);

        glutDisplayFunc(display);
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
	int i;
	for(i = 0; i < face; i++)
		gimage[i] = read_jpeg_image(infile[i]);
}

