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

extern ByteRaster *read_jpeg_image(const char *filename);
void ReadInFile(int face);           // read jpeg file to buffer
static char *infile[6];              // names of the pictures' file

void display();                     // general display function to draw cube
static int mode = 1;                // 1 = GL mode; 0 = Basic mode
void GLDisplay();                   // draw in GL mode through gl functions
static void key_CB(unsigned char key, int x, int y);     // Called on key press 

float sizeofCube = 1.0f;            // normalized cube size in GL mode
GLuint texture[6];                  // texture ID for different planes
GLfloat rotation_matrix[16];        // rotation matrix in GL mode
int refreshMills = 15;              // refresh interval in milliseconds   
GLfloat angleGL = 0.0f;             // Rotational angle in GL mode
static int rotateAxis2 = 1;         // 1 = Y axis, -1 = diagonal axis
float viewy = 2.0;
float viewz = 0.0;

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
	GLDisplay();
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
  
	glTranslatef(0.0f, 0.0f, 2.0f);  // Move the center of cube the origin before rotation 
	
	gluLookAt(0., viewy, viewz, 0., 0., 0., 0., 0., 1.);   // set up the eye/camera position
	Draw(sizeofCube);
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
		case 's':
			viewy = viewy + 0.1;
			break;
		// quit the program -- 'hard quit' 
		case 'w':
			viewy = viewy - 0.1;
			break;
		case 'q':
			viewz = viewz + 0.1;
			break;
		case 'e':
			viewz = viewz - 0.1;
			break;
		case 'Esc':
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
	glutCreateWindow("Scene");   // Create window
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

