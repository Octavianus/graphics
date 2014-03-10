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
//////////////////////////////////////////////////////////////////////////////////
//                              GLOBAL IDENTIFIERS                              //
//////////////////////////////////////////////////////////////////////////////////
Display                 *dpy;
Window                  root, win;
XVisualInfo             *vi;
GLXContext              glc;
Colormap                cmap;
XSetWindowAttributes    swa;
XWindowAttributes       wa;
XEvent                  xev;
GLint                   att[]   = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

float                   TimeCounter, LastFrameTimeCounter, DT, prevTime = 0.0, FPS;
struct timeval          tv, tv0;
int                     Frame = 1, FramesPerFPS;

GLfloat                 rotation_matrix[16];

/* default window size on our display device, in pixels */
static int width  = 500;
static int height = 500;

static char *infile[6];
static ByteRaster *image[6];
GLuint texture[6];

//rotation parameters
float rot_z_vel = 20.0, rot_y_vel = 0.0;
// light position

void LoadGLTextures(int k)
{
    // 创建纹理
    glGenTextures(k+1, &texture[k]);
    glBindTexture(GL_TEXTURE_2D, texture[k]); // 绑定2D纹理
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image,
    // y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image[k]->width(), image[k]->height(),
            0, GL_RGB, GL_UNSIGNED_BYTE, image[k]->data);
            
    glEnable(GL_TEXTURE_2D);                        // Enable Texture Mapping ( NEW )
    glShadeModel(GL_SMOOTH);                        // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);                   // Black Background
    glClearDepth(1.0f);                         // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);                        // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);                         // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // Really Nice Perspective Calculations
	
}

void DrawCube(float size) {
    // Top Face of the cube
	// Load the Texture of this face
	LoadGLTextures(0);
    glBegin(GL_QUADS);
	// set the normal of this face
	  glNormal3d(0, 0, 1);
 	  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	  glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
   	  glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  
    glEnd();
    // Bottom Face of the cube
	// Load the Texture of this face
    LoadGLTextures(1);
    glBegin(GL_QUADS);
	// set the normal of this face
	  glNormal3d(0, 0, -1);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	  glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
   	  glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	  glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  
    glEnd();
	// Front Face of the cube
	// Load the Texture of this face
    LoadGLTextures(2);
    glBegin(GL_QUADS);
	// set the normal of this face
      glNormal3d(0, 1, 0);
	  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
   	  glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	  glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  
    glEnd();
	// Back Face of the cube
	// Load the Texture of this face
    LoadGLTextures(3);
    glBegin(GL_QUADS);
	// set the normal of this face
      glNormal3d(0, -1, 0);
  	  glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	  glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
   	  glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	  glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  
    glEnd();
	// Right Face of the cube
	// Load the Texture of this face
    LoadGLTextures(4);
    glBegin(GL_QUADS);
	// set the normal of this face
      glNormal3d(1, 0, 0);
	  glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	  glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
   	  glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	  glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  
    glEnd();  
	// Left Face of the cube
	// Load the Texture of this face
    LoadGLTextures(5);
    glBegin(GL_QUADS);
	// set the normal of this face
	  glNormal3d(-1, 0, 0);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	  glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
   	  glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  
    glEnd();
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
 XGetWindowAttributes(dpy, win, &wa);
 glViewport(0, 0, wa.width, wa.height);
 aspect_ratio = (float)(wa.width) / (float)(wa.height);
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
 glOrtho(0, (float)wa.width, 0, (float)wa.height, -1., 1.);

 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();

 glColor3f(1.0, 1.0, 1.0);

 sprintf(info_string, "%4.1f seconds * %4.1f fps at %i x %i", TimeCounter, FPS, wa.width, wa.height, rot_z_vel);
 glRasterPos2i(10, 10);
 glCallLists(strlen(info_string), GL_UNSIGNED_BYTE, info_string);

 sprintf(info_string, "<up,down,left,right> rotate cube * <F1> stop rotation ");
 glRasterPos2i(10, wa.height-32);
 glCallLists(strlen(info_string), GL_UNSIGNED_BYTE, info_string);
 /////////////////////////////////
 //     SWAP BUFFERS            //
 /////////////////////////////////
 glXSwapBuffers(dpy, win);
}
//////////////////////////////////////////////////////////////////////////////////
//                              CREATE A GL CAPABLE WINDOW                      //
//////////////////////////////////////////////////////////////////////////////////
void CreateWindow() {

   if((dpy = XOpenDisplay(NULL)) == NULL) {
        printf("\n\tcannot connect to x server\n\n");
        exit(0);
   }

 root = DefaultRootWindow(dpy);
 
   if((vi = glXChooseVisual(dpy, 0, att)) == NULL) {
        printf("\n\tno matching visual\n\n");
        exit(0);
   }
        
   if((cmap = XCreateColormap(dpy, root, vi->visual, AllocNone)) == 0) {
        printf("\n\tcannot create colormap\n\n");
        exit(0);
   }
        
 swa.event_mask = KeyPressMask;
 swa.colormap   = cmap;
 win = XCreateWindow(dpy, root, 0, 0, 1000, 1000, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
 XStoreName(dpy, win, "OpenGL Animation");
 XMapWindow(dpy, win);
}
//////////////////////////////////////////////////////////////////////////////////
//                              SETUP GL CONTEXT                                //
//////////////////////////////////////////////////////////////////////////////////
void SetupGL() {
 char           font_string[128];
 XFontStruct    *font_struct;
 /////////////////////////////////////////////////
 //     CREATE GL CONTEXT AND MAKE IT CURRENT   //
 /////////////////////////////////////////////////
   if((glc = glXCreateContext(dpy, vi, NULL, GL_TRUE)) == NULL) {
        printf("\n\tcannot create gl context\n\n");
        exit(0);
   }

 glXMakeCurrent(dpy, win, glc);
 glEnable(GL_DEPTH_TEST);
 glClearColor(0.00, 0.00, 0.40, 1.00);
    /////////////////////////////////////////////////
    //     FIND A FONT                             //
    /////////////////////////////////////////////////
    for(int font_size = 14; font_size < 32; font_size += 2) {
        sprintf(font_string, "-adobe-courier-*-r-normal--%i-*", font_size);
        font_struct = XLoadQueryFont(dpy, font_string);
        
        if(font_struct != NULL) {
                glXUseXFont(font_struct->fid, 32, 192, 32);          
                break;
        }
    }
 /////////////////////////////////////////////////
 //     INITIALIZE ROTATION MATRIX              //
 /////////////////////////////////////////////////
 glEnable(GL_LIGHTING);
 glEnable(GL_LIGHT0);
 GLfloat lightpos[] = {0.0, 0.0, 1.0, 0.};
 glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 glGetFloatv(GL_MODELVIEW_MATRIX, rotation_matrix);
}
//////////////////////////////////////////////////////////////////////////////////
//                              TIME COUNTER FUNCTIONS                          //
//////////////////////////////////////////////////////////////////////////////////
void InitTimeCounter() {
 gettimeofday(&tv0, NULL);
 FramesPerFPS = 30; }

void UpdateTimeCounter() {
 LastFrameTimeCounter = TimeCounter;
 gettimeofday(&tv, NULL);
 TimeCounter = (float)(tv.tv_sec-tv0.tv_sec) + 0.000001*((float)(tv.tv_usec-tv0.tv_usec));
 DT = TimeCounter - LastFrameTimeCounter;
}

void CalculateFPS() {
 Frame ++;

   if((Frame%FramesPerFPS) == 0) {
        FPS = ((float)(FramesPerFPS)) / (TimeCounter-prevTime);
        prevTime = TimeCounter;
   }
}
//////////////////////////////////////////////////////////////////////////////////
//                              EXIT PROGRAM                                    //
//////////////////////////////////////////////////////////////////////////////////
void ExitProgram() {
 glXMakeCurrent(dpy, None, NULL);
 glXDestroyContext(dpy, glc);
 XDestroyWindow(dpy, win);
 XCloseDisplay(dpy);
 exit(0);
}
//////////////////////////////////////////////////////////////////////////////////
//                              CHECK EVENTS                                    //
//////////////////////////////////////////////////////////////////////////////////
void CheckKeyboard() {

    if(XCheckWindowEvent(dpy, win, KeyPressMask, &xev)) {
        char    *key_string = XKeysymToString(XkbKeycodeToKeysym(dpy, xev.xkey.keycode, 0, 0));

        if(strncmp(key_string, "Left", 4) == 0) {
                rot_z_vel -= 200.0*DT;
        }

        else if(strncmp(key_string, "Right", 5) == 0) {
                rot_z_vel += 200.0*DT;
        }

        else if(strncmp(key_string, "Up", 2) == 0) {
                rot_y_vel -= 200.0*DT;
        }

        else if(strncmp(key_string, "Down", 4) == 0) {
                rot_y_vel += 200.0*DT;
        }

        else if(strncmp(key_string, "F1", 2) == 0) {
                rot_y_vel = 0.0; 
                rot_z_vel = 0.0;
        }

        else if(strncmp(key_string, "Escape", 5) == 0) {
                ExitProgram();
        }
    }
}

void ReadInFile(int face){
	infile[0] = "1.jpg";
	infile[1] = "2.jpg";
	infile[2] = "3.jpg";
	infile[3] = "4.jpg";
	infile[4] = "6.jpg";
	infile[5] = "5.jpg";
	int i;
	for(i = 0; i < face; i++)
		image[i] = read_jpeg_image(infile[i]);
}
//////////////////////////////////////////////////////////////////////////////////
//                              MAIN PROGRAM                                    //
//////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]){
 ReadInFile(6);
 CreateWindow();
 SetupGL();
 InitTimeCounter();

    while(true) {
        UpdateTimeCounter();
        CalculateFPS();
        RotateCube();
        ExposeFunc(); 
        usleep(1000);
        CheckKeyboard();
    }
}
