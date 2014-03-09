#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<math.h>
#include<time.h>
#include<sys/time.h>
#include<X11/Xlib.h>
#include<X11/XKBlib.h>
#include<GL/glx.h>
#include<GL/glext.h>
#include <GL/glu.h>       
#include <GL/glut.h>  
#include<setjmp.h>

#include"raster.h"

//  GLOBAL IDENTIFIERS 

float TimeCounter, LastFrameTimeCounter, DT, prevTime = 0.0;
struct timeval tv, tv0;
int Frame = 1;

GLfloat rotation_matrix[16];

/* default window size on our display device, in pixels */
static int width  = 500;
static int height = 500;

static char *infile[6];
static ByteRaster *image[6];
GLuint texture[6];

//rotation parameters
float rot_z_vel = 20.0, rot_y_vel = 0.0;

void LoadGLTextures(int k)
{
	
}

// draw an empty cube with identical color.
void DrawCube(float size) {

    glBegin(GL_QUADS);
 	  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	  glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
   	  glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  
    glEnd();

    glBegin(GL_QUADS);
    	  glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	  glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
   	  glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	  glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  
    glEnd();

    glBegin(GL_QUADS);
    	  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
   	  glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	  glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  
    glEnd();

    glBegin(GL_QUADS);
    	  glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	  glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
   	  glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	  glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  
    glEnd();

    glBegin(GL_QUADS);
    	  glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	  glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
   	  glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	  glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  
    glEnd();  
  
    glBegin(GL_QUADS);
    	  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	  glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
   	  glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  
    glEnd();


}

//////////////////////////////////////////////////////////////////////////////////
//                              TIME COUNTER FUNCTIONS                          //
//////////////////////////////////////////////////////////////////////////////////
void InitTimeCounter() {
 gettimeofday(&tv0, NULL);
}

void UpdateTimeCounter() {
 LastFrameTimeCounter = TimeCounter;
 gettimeofday(&tv, NULL);
 TimeCounter = (float)(tv.tv_sec-tv0.tv_sec) + 0.000001*((float)(tv.tv_usec-tv0.tv_usec));
 DT = TimeCounter - LastFrameTimeCounter;
}

//////////////////////////////////////////////////////////////////////////////////
//                              ROTATE THE CUBE                                 //
//////////////////////////////////////////////////////////////////////////////////
void RotateCube() {
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	 glRotatef(rot_y_vel*DT, 0.0, 1.0, 0.0);
	 glRotatef(rot_z_vel*DT, 0.0, 0.0, 1.0);
	 glMultMatrixf(rotation_matrix);
	 glGetFloatv(GL_MODELVIEW_MATRIX, rotation_matrix);
}
//////////////////////////////////////////////////////////////////////////////////
//                              EXPOSURE FUNCTION                               //
//////////////////////////////////////////////////////////////////////////////////
void ExposeFunc() {
	 float  aspect_ratio;
	 char   info_string[256];
	 /////////////////////////////////
	 //     RESIZE VIEWPORT         //
	 /////////////////////////////////
	 
	 glViewport(0, 0, width, height);
	 aspect_ratio = (float)(width) / (float)(height);
	 /////////////////////////////////////////
	 //     SETUP PROJECTION & MODELVIEW        //
	 /////////////////////////////////////////
	 glMatrixMode(GL_PROJECTION);
	 glLoadIdentity();
	 glOrtho(-2.50*aspect_ratio, 2.50*aspect_ratio, -2.50, 2.50, 1., 100.);

	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	 gluLookAt(10., 0., 0., 0., 0., 0., 0., 0., 1.);
	 glMultMatrixf(rotation_matrix);
	 /////////////////////////////////
	 //     DRAW CUBE               //
	 /////////////////////////////////
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 DrawCube(1.0);
	 /////////////////////////////////
	 //     DISPLAY TIME, FPS etc.  //
	 /////////////////////////////////
	 glMatrixMode(GL_PROJECTION);
	 glLoadIdentity();
	 glOrtho(0, (float)width, 0, (float)height, -1., 1.);

	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();

	 glutSwapBuffers();
}

// define the keyboard event
static void key_CB(unsigned char key, int x, int y)
{
	switch(key)
	{
		// switch rotation mode 
		case 'q':
			exit(0);
			break;
		// stop rotation
		case 's':
			rot_y_vel = 0.0; 
            rot_z_vel = 0.0;
			break;
		// restart rotation
		case 'r':
			rot_y_vel = 50.0; 
            rot_z_vel = 0.0;
			break;
		default:
			break;
	}
	glutPostRedisplay ();
}	
//////////////////////////////////////////////////////////////////////////////////
//                              MAIN PROGRAM                                    //
//////////////////////////////////////////////////////////////////////////////////

void ReadInFile(int face){
	infile[0] = "1.jpg";
	infile[1] = "2.jpg";
	infile[2] = "3.jpg";
	infile[3] = "4.jpg";
	infile[4] = "5.jpg";
	infile[5] = "6.jpg";
	int i;
	for(i = 0; i < face; i++)
		image[i] = read_jpeg_image(infile[i]);
}

int main(int argc, char *argv[]){

    int win;

    // read in all the pic from 1 - 6
    ReadInFile(6);

    InitTimeCounter();
    /* GLUT & GL initialization */
    /* initialize GLUT system */
    glutInit(&argc, argv);    
    /* initialize display format */
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    /* define and get handle to the window (render) context */
    glutInitWindowSize(width, height); 
    win = glutCreateWindow("HW2"); 
    /* set background clear color to black */
    glClearColor((GLclampf)0.0,(GLclampf)0.0,(GLclampf)0.0,(GLclampf)0.0);  
	
    // init the rotation matrix 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, rotation_matrix);

    while(true) {
        UpdateTimeCounter();
        RotateCube();
        ExposeFunc(); 
	usleep(1000);
        glutKeyboardFunc(key_CB);
    }
	
}
