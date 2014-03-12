#include<X11/Xlib.h>
#include<X11/XKBlib.h>
#include<GL/glx.h>
#include<GL/glext.h> 

// All the parameters needed for init the window and time.
Display                 *dpy;
Window                  root, win;
XEvent                  xev;
XVisualInfo             *vi;
GLXContext              glc;
Colormap                cmap;
XSetWindowAttributes    swa;
XWindowAttributes       wa;

float                   TimeCounter, LastFrameTimeCounter, DT;
struct timeval          tv, tv0;
int                     Frame = 1;

// rotate matrix
GLfloat                 rotation_matrix[16];

GLint                   att[]   = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };