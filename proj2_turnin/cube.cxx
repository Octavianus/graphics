/* 
 * CSC533 HomeWork 2
 * Group Member: Xudong Weng, Laiyong Mu
 
 * Program starts with GL mode. 
 * GL mode is brighter one, and Basic mode is darker one.
 * Extra features:
 * 	1. implemented the cube rotation around its diagonal in both mode
 * 	2. implemented the lighting effect in both mode.
 *
 * Operations:
 * 	"b" switch to basic mode.
 * 	"g" switch to GL mode.
 * 	"q" quit the program.
 *
 * Accompanied programs [raster.cxx, raster.h, raster-jpeg.cxx] 
 * are from "Paintdemo" provided by the instructor
 */

 /* Assumptions in Basic mode:
 (1) Assuming the size of each plane of the cube is 600 x 600 pixels
 (2) Assuming the center of the Cube is at (0, 0, -600)
 (3) Assuming the view point is at (0, 0, 600)
 (4) Assuming the display window's z axis value = 0
 (4) The cube first rotate around Y-axis for one circle and then rotate around diagonal axis for one cycle
 (5) simulated light source is direct light (vector (0, 0, -1)
*/

#include <stdio.h>
#include <math.h>
#include <GL/glu.h>       
#include <GL/glut.h> 
#include <math.h>     
#include "raster.h"

/* Function Prototypes and Global Variables */
static ByteRaster *gimage[6];        // pixel buffer for 6 jpeg picture

#include "basicprojection.h"         // functions of rotation and projection in basic mode (pixel by pixel)
extern void BasicProjection();

extern ByteRaster *read_jpeg_image(const char *filename);
void ReadInFile(int face);           // read jpeg file to buffer
static char *infile[6];              // names of the pictures' file

void display();                     // general display function to draw cube
static int mode = 1;                // 1 = GL mode; 0 = Basic mode
void BasicDisplay();                // draw in basic mode - manually calculated pixel by pixel
void GLDisplay();                   // draw in GL mode through gl functions
static void key_CB(unsigned char key, int x, int y);     // Called on key press 

float sizeofCube = 1.0f;            // normalized cube size in GL mode
GLuint texture[6];                  // texture ID for different planes
GLfloat rotation_matrix[16];        // rotation matrix in GL mode
int refreshMills = 15;              // refresh interval in milliseconds   
GLfloat angleGL = 0.0f;             // Rotational angle in GL mode
static int rotateAxis2 = 1;         // 1 = Y axis, -1 = diagonal axis

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

/* display() Callback functions for both modes */
void display(){

	if (mode == 1)
		GLDisplay();
	
	if (mode == 0)
		BasicDisplay();
}

// draw the cube
void Draw(float sizeofCube)
{
	
    // apply textures to the cube
	// Top face of the cube
    LoadGLTextures(4);
    glBegin(GL_QUADS);
    		// set the normal of this face
		glNormal3d(0, 0, 1);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeofCube, -sizeofCube,  sizeofCube);
		glTexCoord2f(sizeofCube, 0.0f); glVertex3f( sizeofCube, -sizeofCube,  sizeofCube);
		glTexCoord2f(sizeofCube, sizeofCube); glVertex3f( sizeofCube,  sizeofCube,  sizeofCube);
		glTexCoord2f(0.0f, sizeofCube); glVertex3f(-sizeofCube,  sizeofCube,  sizeofCube);  
    glEnd();

    // Bottom Face of the cube
    LoadGLTextures(1);
    glBegin(GL_QUADS);
    		// set the normal of this face
		glNormal3d(0, 0, -1);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(sizeofCube, -sizeofCube, -sizeofCube);
		glTexCoord2f(sizeofCube, 0.0f); glVertex3f(-sizeofCube, -sizeofCube, -sizeofCube);
		glTexCoord2f(sizeofCube, sizeofCube); glVertex3f(-sizeofCube,  sizeofCube, -sizeofCube);
		glTexCoord2f(0.0f, sizeofCube); glVertex3f( sizeofCube, sizeofCube, -sizeofCube);  
    glEnd();
    // Front Face of the cube
    LoadGLTextures(0);
    glBegin(GL_QUADS);

    		// set the normal of this face
		glNormal3d(0, 1, 0);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeofCube, sizeofCube, sizeofCube);
		glTexCoord2f(sizeofCube, 0.0f); glVertex3f(sizeofCube,  sizeofCube,  sizeofCube);
		glTexCoord2f(sizeofCube, sizeofCube); glVertex3f( sizeofCube,  sizeofCube,  -sizeofCube);
		glTexCoord2f(0.0f, sizeofCube); glVertex3f(-sizeofCube,  sizeofCube, -sizeofCube);  
    glEnd();
    // Back Face of the cube
    LoadGLTextures(2);
    glBegin(GL_QUADS);
    		// set the normal of this face
		glNormal3d(0, -1, 0);
		glTexCoord2f(sizeofCube, sizeofCube); glVertex3f(sizeofCube, -sizeofCube, sizeofCube);
		glTexCoord2f(0.0f, sizeofCube); glVertex3f(-sizeofCube, -sizeofCube, sizeofCube);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeofCube, -sizeofCube, -sizeofCube);
		glTexCoord2f(sizeofCube, 0.0f); glVertex3f(sizeofCube, -sizeofCube, -sizeofCube);  
    glEnd();
    // Right Face of the cube
    LoadGLTextures(5);
    glBegin(GL_QUADS);
    		// set the normal of this face
		glNormal3d(1, 0, 0);
		glTexCoord2f(sizeofCube, 0.0f); glVertex3f(sizeofCube, -sizeofCube, -sizeofCube);
		glTexCoord2f(sizeofCube, sizeofCube); glVertex3f( sizeofCube,  sizeofCube, -sizeofCube);
		glTexCoord2f(0.0f, sizeofCube); glVertex3f( sizeofCube,  sizeofCube,  sizeofCube);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( sizeofCube, -sizeofCube,  sizeofCube);  
    glEnd();  
    // Left Face of the cube
    LoadGLTextures(3);
    glBegin(GL_QUADS);
    		// set the normal of this face
		glNormal3d(-1, 0, 0);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeofCube, -sizeofCube, -sizeofCube);
		glTexCoord2f(sizeofCube, 0.0f); glVertex3f(-sizeofCube, -sizeofCube,  sizeofCube);
		glTexCoord2f(sizeofCube, sizeofCube); glVertex3f(-sizeofCube,  sizeofCube,  sizeofCube);
		glTexCoord2f(0.0f, sizeofCube); glVertex3f(-sizeofCube,  sizeofCube, -sizeofCube);  
    glEnd();

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
	glTranslatef(0.0f, 0.0f, -6.5f);   // Move further away from the screen
  
    // Rotate around Y axis (rotateAxis2 = 1) and diagonal axis (rotateAxis2 = -1)
	if (rotateAxis2 == 1)
		glRotatef(angleGL, 0.0f, 1.0f, 0.0f);  
	if (rotateAxis2 == -1)
		glRotatef(angleGL, 1.0f, 1.0f, 1.0f);
		
	glTranslatef(0.0f, 0.0f, 2.0f);  // Move the center of cube the origin before rotation 
	
	gluLookAt(2., 0., 0., 0., 0., 0., 0., 0., 1.);   // set up the eye/camera position
	Draw(sizeofCube);
	glLoadIdentity();                // Reset the model-view matrix 
	glFlush();                       //Complete any pending operations 
	glutSwapBuffers();               // Swap the front and back frame buffers (double buffering)
	
	// update rotating angle in 2 degree steps
	angleGL += 2.0f;
	// reset rotating angle and axis after each cycle
	if (angleGL == 360.0f){
		angleGL = 0.0f;
		rotateAxis2 = -rotateAxis2;
	}
	
	glViewport(0, 0, 600, 600);     // set up the display window coordinates
    glMatrixMode(GL_PROJECTION);    // To operate on the Projection matrix
	glLoadIdentity();               // Reset matrix
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f); // Enable perspective projection with fovy, aspect, zNear and zFar
}

/* Basic mode display function */
void BasicDisplay(){
	

	glDisable(GL_TEXTURE_2D);                      // Enable Texture Mapping
	glDisable(GL_LIGHTING);    // disable the lighting effect in GL mode
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 
		
    BasicProjection();         // calculate the color of each pixel after rotation
	  
	// render a raster image into a viewport 
	int i, j;
	for (i = 0; i < 600; i++) {
		for (j = 0; j < 600; j++) {
			// the R,G,B (byte) color of pixel(i,j) 
			glColor3d (imagBuffer[i][j][0]/255.,
					   imagBuffer[i][j][1]/255.,
					   imagBuffer[i][j][2]/255.);
			
			glBegin(GL_POINTS);
				glVertex2i (i,j);
			glEnd();          
		}
	}
	
	glLoadIdentity();
	glFlush();         
    glutSwapBuffers(); 
	glMatrixMode(GL_PROJECTION);

	gluOrtho2D(0, 600, 600, 0);  //how object is mapped to window
}

/* Function is called on a key press */
static void key_CB(unsigned char key, int x, int y) 
{
    switch (key)
	{
		// switch to basic mode
		case 'b':
			mode = 0;
			break;
		// switch to GL mode		
		case 'g':
			mode = 1;
			break;
		// quit the program -- 'hard quit' 
		case 'q':
			exit(0);
			break;
		default: 
			break;
	}
	glutPostRedisplay (); // state has changed: tell GLUT to redraw
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
	glutCreateWindow("Rotating Cube - Two modes");   // Create window
	ReadInFile(6);                                // Read jpeg images to pixel buffer
	
    glutDisplayFunc(display);
	glutKeyboardFunc(key_CB);
	initGL();                       // OpenGL initialization
	glutTimerFunc(0, timer, 0);     // First timer call immediately 
    glutMainLoop();
 
    return 0;                       
}

// Read in all the jpg file to image array
void ReadInFile(int face){
	infile[4] = "m01.jpg";
	infile[5] = "m02.jpg";
	infile[1] = "m03.jpg";
	infile[3] = "m04.jpg";
	infile[0] = "m06.jpg";
	infile[2] = "m05.jpg";
	int i;
	for(i = 0; i < face; i++)
		gimage[i] = read_jpeg_image(infile[i]);
}

