/*
 * program start at GL mode.
 *
 * Additional features:
 * 	1. implemented the cube rotation around its diagonal in both mode
 * 	2. implemented the lighting effect in both mode.
 *
 * Operations:
 * 	"b" switch to basic mode.
 * 	"g" switch to GL mode.
 * 	"q" quit the program.
 * 	"s" in GL mode, stop the rotation.
 * 	"r" in GL mode, restart the rotation.
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<math.h>
#include<time.h>
#include<sys/time.h>
#include<GL/glu.h>       
#include<GL/glut.h>  
#include<setjmp.h>

#include"raster.h"
#include"glmode.h"

/* default window size on our display device, in pixels */
static int gwidth  = 600;
static int gheight = 600;
// read in jpg file and texture file
static char *infile[6];
static ByteRaster *gimage[6];
GLuint texture[6];

//rotation parameters, angle
static float rot_1 = -2.0, rot_2 = 0.0;
static float rot_temp_1 = 0.0, rot_temp_2 = 0.0;
float sizeofCube = 1.0f;
// total rotation angle , 360 for a circle. mode is to record the circle.
static float rot_a = 0.0;
static int mode = 0;
static const float delta = 1e-3;

extern void basicWindowDisplay(int *argc, char **argv);
extern void basicImageRead();

void LoadGLTextures(int k)
{
    // Create texture 
    glGenTextures(k+1, &texture[k]);
    glBindTexture(GL_TEXTURE_2D, texture[k]); // binding the 2D texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image,
    // y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, gimage[k]->width(), gimage[k]->height(),
            0, GL_RGB, GL_UNSIGNED_BYTE, gimage[k]->data);
            
    glEnable(GL_TEXTURE_2D);                        // Enable Texture Mapping
    glShadeModel(GL_SMOOTH);                        // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);                   // Black Background
    glClearDepth(1.0f);                         // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);                        // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);                         // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // Really Nice Perspective Calculations
}

void Draw(float sizeofCube) {
    // Top Face of the cube
    // Load the Texture of this face
    LoadGLTextures(0);
    glBegin(GL_QUADS);
	  // set the normal of this face
	  glNormal3d(0, 0, 1);
 	  glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeofCube, -sizeofCube,  sizeofCube);
	  glTexCoord2f(sizeofCube, 0.0f); glVertex3f( sizeofCube, -sizeofCube,  sizeofCube);
   	  glTexCoord2f(sizeofCube, sizeofCube); glVertex3f( sizeofCube,  sizeofCube,  sizeofCube);
	  glTexCoord2f(0.0f, sizeofCube); glVertex3f(-sizeofCube,  sizeofCube,  sizeofCube);  
    glEnd();
    // Bottom Face of the cube
    // Load the Texture of this face
    LoadGLTextures(1);
    glBegin(GL_QUADS);
	  // set the normal of this face
	  glNormal3d(0, 0, -1);
          glTexCoord2f(sizeofCube, 0.0f); glVertex3f(-sizeofCube, -sizeofCube, -sizeofCube);
	  glTexCoord2f(sizeofCube, sizeofCube); glVertex3f(-sizeofCube,  sizeofCube, -sizeofCube);
   	  glTexCoord2f(0.0f, sizeofCube); glVertex3f( sizeofCube,  sizeofCube, -sizeofCube);
	  glTexCoord2f(0.0f, 0.0f); glVertex3f( sizeofCube, -sizeofCube, -sizeofCube);  
    glEnd();
    // Front Face of the cube
    // Load the Texture of this face
    LoadGLTextures(2);
    glBegin(GL_QUADS);
	// set the normal of this face
          glNormal3d(0, 1, 0);
	  glTexCoord2f(0.0f, sizeofCube); glVertex3f(-sizeofCube,  sizeofCube, -sizeofCube);
	  glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeofCube,  sizeofCube,  sizeofCube);
   	  glTexCoord2f(sizeofCube, 0.0f); glVertex3f( sizeofCube,  sizeofCube,  sizeofCube);
	  glTexCoord2f(sizeofCube, sizeofCube); glVertex3f( sizeofCube,  sizeofCube, -sizeofCube);  
    glEnd();
    // Back Face of the cube
    // Load the Texture of this face
    LoadGLTextures(3);
    glBegin(GL_QUADS);
     	  // set the normal of this face
 	  glNormal3d(0, -1, 0);
  	  glTexCoord2f(sizeofCube, sizeofCube); glVertex3f(-sizeofCube, -sizeofCube, -sizeofCube);
	  glTexCoord2f(0.0f, sizeofCube); glVertex3f( sizeofCube, -sizeofCube, -sizeofCube);
   	  glTexCoord2f(0.0f, 0.0f); glVertex3f( sizeofCube, -sizeofCube,  sizeofCube);
	  glTexCoord2f(sizeofCube, 0.0f); glVertex3f(-sizeofCube, -sizeofCube,  sizeofCube);  
    glEnd();
    // Right Face of the cube
    // Load the Texture of this face
    LoadGLTextures(4);
    glBegin(GL_QUADS);
  	  // set the normal of this face
          glNormal3d(1, 0, 0);
	  glTexCoord2f(sizeofCube, 0.0f); glVertex3f( sizeofCube, -sizeofCube, -sizeofCube);
	  glTexCoord2f(sizeofCube, sizeofCube); glVertex3f( sizeofCube,  sizeofCube, -sizeofCube);
   	  glTexCoord2f(0.0f, sizeofCube); glVertex3f( sizeofCube,  sizeofCube,  sizeofCube);
	  glTexCoord2f(0.0f, 0.0f); glVertex3f( sizeofCube, -sizeofCube,  sizeofCube);  
    glEnd();  
    // Left Face of the cube
    // Load the Texture of this face
    LoadGLTextures(5);
    glBegin(GL_QUADS);
    	  // set the normal of this face
	  glNormal3d(-1, 0, 0);
          glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeofCube, -sizeofCube, -sizeofCube);
	  glTexCoord2f(sizeofCube, 0.0f); glVertex3f(-sizeofCube, -sizeofCube,  sizeofCube);
   	  glTexCoord2f(sizeofCube, sizeofCube); glVertex3f(-sizeofCube,  sizeofCube,  sizeofCube);
	  glTexCoord2f(0.0f, sizeofCube); glVertex3f(-sizeofCube,  sizeofCube, -sizeofCube);  
    glEnd();
}

// this function will rotate the cude by using glRotatef with the given axis-wise and angle.
void Rotate() {
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	 if( abs(360 - rot_a) == 0)
	 {
	 	rot_a = 0.0;
		// gettimeofday(&tv0, NULL);
		// get the mode of
		if(mode == 0){
			rot_1 = 0.0;
			rot_2 = -2.0;
			mode = 1;
			}
		else if(mode == 1){
			rot_2 = 0.0;
			rot_1 = -2.0;
			mode = 0;
		}
	 }
	 
	 glRotatef(rot_1, 0.0, 0.0, -1.0);
	 glRotatef(rot_2, -1.0, -1.0, -1.0);
	 
	 // calculate how many circle it has.
	 rot_a = rot_a + abs(rot_1) + abs(rot_2);
	 
	 glMultMatrixf(rotation_matrix);
	 glGetFloatv(GL_MODELVIEW_MATRIX, rotation_matrix);
}

void Configurate() {
	 float  aspect_ratio;
	 
	 // resize view point
	 XGetWindowAttributes(dpy, win, &wa);
	 glViewport(0, 0, wa.width, wa.height);
	 aspect_ratio = (float)(wa.width) / (float)(wa.height);

	 // set the projection view and other setting
	 glMatrixMode(GL_PROJECTION);
	 glLoadIdentity();
	 glOrtho(-2.50*aspect_ratio, 2.50*aspect_ratio, -2.50, 2.50, 1., 100.);

	 // set the lookAt point
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	 gluLookAt(20., 0., 0., 0., 0., 0., 0., 0., 1.);
	 glMultMatrixf(rotation_matrix);
	 // Call Draw to draw the cube each time.
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 Draw(sizeofCube);
	 // swap the buffer by calling glX lib
	 glXSwapBuffers(dpy, win);
}

// create a windows
void CreateWindow() {
	 // initiate all the window parameters by using glX
     	 dpy = XOpenDisplay(NULL);
     	 root = DefaultRootWindow(dpy);
     	 vi = glXChooseVisual(dpy, 0, att);
     	 cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
        
	 swa.event_mask = KeyPressMask;
	 swa.colormap   = cmap;
	 win = XCreateWindow(dpy, root, 0, 0, gwidth, gheight, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	 XStoreName(dpy, win, "HW2 - GL Mode");
	 XMapWindow(dpy, win);
}

void InitGL() {
	 // create the GL context and give to the current context parameters.
	 gettimeofday(&tv0, NULL);
	 glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	 glXMakeCurrent(dpy, win, glc);
	 glEnable(GL_DEPTH_TEST);
	 glClearColor(0.00, 0.00, 0.40, 1.00);
	 
	 // Enable the lighting feature, and set a new point light source.
	 glEnable(GL_LIGHTING);
	 glEnable(GL_LIGHT0);
	 GLfloat lightpos[] = {0.0, 0.0, 1.0, 0.};
	 glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	 
	 // set up the rotation matrix
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	 glGetFloatv(GL_MODELVIEW_MATRIX, rotation_matrix);
}

// get the time inteval from int rotation time tv0.
void UpdateTimer() {
 	gettimeofday(&tv, NULL);
	TimeCounter = tv.tv_usec-tv0.tv_usec;
 	// DT will decide what angle should be now.
	// DT = TimeCounter - LastFrameTimeCounter;
}

// exit the grogram safely
void Exit() {
	glXMakeCurrent(dpy, None, NULL);
	glXDestroyContext(dpy, glc);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

// Read in all the jpg file to image array
void ReadInFile(int face){
	infile[0] = "1.jpg";
	infile[1] = "2.jpg";
	infile[2] = "3.jpg";
	infile[3] = "4.jpg";
	infile[4] = "6.jpg";
	infile[5] = "5.jpg";
	int i;
	for(i = 0; i < face; i++)
		gimage[i] = read_jpeg_image(infile[i]);
}

// check for the keyboard event.
void Key_CB() {
    // detect the key board input
    if(XCheckWindowEvent(dpy, win, KeyPressMask, &xev)) {
        char    *key_string = XKeysymToString(XkbKeycodeToKeysym(dpy, xev.xkey.keycode, 0, 0));
	// stop the rotation of GL mode
        if(strncmp(key_string, "s", 1) == 0) {
                rot_temp_1 = rot_1;
		rot_temp_2 = rot_2;
		rot_1 = 0.0; 
		rot_2 = 0.0;
        }

	// restart the rotation of GL mode
	else if(strncmp(key_string, "r", 1) == 0)
	{
		rot_1 = rot_temp_1;
		rot_2 = rot_temp_2;
	}
	
	// quit the GL mode and baisc mode.
        else if(strncmp(key_string, "q", 1) == 0) {
                Exit();
		exit(0);
        }
		
	// switch to the GL mode.
        else if(strncmp(key_string, "g", 1) == 0) {
		 Exit();         
	      	 ReadInFile(6);
		 CreateWindow();
		 InitGL();
	 
	     while(true) {
        	 UpdateTimer();
	         Rotate();
	         Configurate(); 
         	 usleep(1000);
	         Key_CB();
       		}	
	}

	// switch to the basic mode.
	else if(strncmp(key_string, "b", 1) == 0)
	{
		// transform the draw mode
		Exit();
		int argc = 0;
		char **argv = NULL;
		basicImageRead();
		basicWindowDisplay(&argc, argv);	
	}
    }
}

//#include"hw2bmode.cxx"

int main(int argc, char *argv[]){
	 ReadInFile(6);
	 CreateWindow();
	 InitGL();
	 
     while(true) {
         UpdateTimer();
         Rotate();
         Configurate(); 
         usleep(1000);
         Key_CB();
     }
}
