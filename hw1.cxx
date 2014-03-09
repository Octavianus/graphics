/* 
 * CSC533 HomeWork 1
 * Group Member: Laiyong Mu, Xudong Weng
 * Accompanied programs [raster.cxx, raster.h, raster-jpeg.cxx] 
 * are from "Paintdemo" provided by the instructor
 */

 /* Requirements and assumptions:
 (1) Assuming the size of the display screen and JPEG picture are the same, 600 x 600 pixels
 (2) Rotate the picture, press "R" to change the rotation direction
 (3) Continuous Zooming, Maximum zoom level 4x, Minimum zoom level 0.25x
 (4) One full Zoom cycle (max 4x -> min 0.25x -> max 4x) = rotate 4 cycles (8Pi degrees)
 (5) Press "Q" to exit
 */
 
#include <setjmp.h>
#include <stdio.h>       
#include <stdlib.h>   
#include <math.h>    

#include <GL/glu.h>       
#include <GL/glut.h>       

#include "raster.h"   

static void display_CB(void);            /* Called whenever redisplay needed */
static void idle_CB   (void);            /* Background activity */
static void key_CB(unsigned char key, int x, int y);     /* Called on key press */
static int rotateTimes = 0;              /* count the number of rotation steps, reset after each cycle */
static int zoomTimes = 0;                /* count the number of zooming steps, reset after rotated 4 cycles */

/* default window size on our display device, in pixels */
static int width  = 600;
static int height = 600;

/* rotating direction flag, press "R" switch its value */
static int anime_R = 1;     // default = 1, counter-clockwise, 0 clockwise

/* hard-coded image file paths */
static char infile[]  = "pic1.jpg";

/* a raster scan of the image */
static ByteRaster *image;

/* a buffer of the RGB color info (3 channels, total 600x600 pixels) in the original image file */
static int imagBuffer[600][600][3];

/* -------------------------------------------------------------------------- */
/* Rotate and Zoom Method */ 
/* Rotate/Zoom the display screen and then match the new positions of its pixels with 
   the image file. For the pixel with non-integer coordinates, using bilinear interpolation 
   to calculate its color information. Assuming background color is black.   
*/

void vrotatezoom (ByteRaster &img, int direc)           // direc = 1, rotate counter-clockwise, -1 clockwise
{
	// count # of rotation steps
	rotateTimes = rotateTimes + direc;            		
	if (rotateTimes == 36 || rotateTimes == -36)        // reset after 1 cycle, reset # = 2Pi/[rotation step size]
		rotateTimes = 0;
	// rotation angle related to the original image position
	float theta = rotateTimes*M_PI/18;                  // current rotation step size = Pi/18
	
	// count # of zooming steps
	zoomTimes++;                                        
	if (zoomTimes == 144)                               // reset after rotating each 4 cycles
		zoomTimes = 0;
	// zoom levels increase/decrease as power of 2. [zoom level] and [rotation angle] relationship: 
	// [1x]/[0], [2x]/[Pi], [4x]/[2Pi], [2x]/[3Pi], [1x]/[4Pi], 
	// [0.5x]/[5Pi], [0.25x]/[6Pi], [0.5x]/[7Pi], [1x]/[8Pi]
	float zoomPhase = zoomTimes/18.00;
	float zoom;                                            // zoom level, min 0.25x, max 4x
	if (zoomPhase >= 0.00 && zoomPhase < 2.00)             // zoom level 1x -> 4x
		zoom = pow(2, (zoomPhase));
	else if (zoomPhase >= 2.00 && zoomPhase <= 6.00)       // zoom level 4x -> 0.25x
		zoom = pow(2, (4.00 - zoomPhase));
	else                                                   // zoom level 0.25x -> 1x
		zoom = pow(2, (zoomPhase - 8.00));
		
	int i,j;     
	int cornerColor[4][3];  // RGB info of 4 pixels in the image around the screen pixel with non-integer coordinates after rotation/zooming, from top left to bottom left, clockwise
	double xTemp,yTemp,xNew,yNew; // new coordinates during and after zooming and rotation
	int cornerX[2], cornerY[2];   // coordinates of 4 surrounding pixel on the image file
		
	/* zoom and then rotate */
	for (i = 0; i < 600; i++)     // y axis
	{  
		for (j = 0; j < 600; j++) // x axis
		{ 
			xTemp = j * zoom + 300 * (1 - zoom);
			yTemp = i * zoom + 300 * (1 - zoom);
			xNew = 300 + (yTemp - 300)*cos(theta) - (xTemp-300) *sin(theta);
			yNew = 300 + (yTemp - 300)*sin(theta) + (xTemp-300) *cos(theta);
			cornerX[0] = (int)xNew;
			cornerX[1] = (int)xNew + 1;
			cornerY[0] = (int)yNew;
			cornerY[1] = (int)yNew + 1;
			
			/* get RGB color info for 4 surrounding pixels
			/* left top corner */
			if (0 <= cornerX[0] && cornerX[0] < width && cornerY[0] >= 0 && cornerY[0] < height)
			{
				cornerColor[0][0] = imagBuffer[cornerY[0]][cornerX[0]][0];
				cornerColor[0][1] = imagBuffer[cornerY[0]][cornerX[0]][1];
				cornerColor[0][2] = imagBuffer[cornerY[0]][cornerX[0]][2];
			}
			else
			{
				cornerColor[0][0] = 0;
				cornerColor[0][1] = 0;
				cornerColor[0][2] = 0;
			}
			/* right top corner */
			if (0 <= cornerX[1] && cornerX[1] < width && cornerY[0] >= 0 && cornerY[0] < height)
			{
				cornerColor[1][0] = imagBuffer[cornerY[0]][cornerX[1]][0];
				cornerColor[1][1] = imagBuffer[cornerY[0]][cornerX[1]][1];
				cornerColor[1][2] = imagBuffer[cornerY[0]][cornerX[1]][2];
			}	
			else
			{
				cornerColor[1][0] = 0;
				cornerColor[1][1] = 0;
				cornerColor[1][2] = 0;
			}
			/* right bottom corner */
			if (0 <= cornerX[1] && cornerX[1] < width && cornerY[1] >= 0 && cornerY[1] < height)
			{
				cornerColor[2][0] = imagBuffer[cornerY[1]][cornerX[1]][0];
				cornerColor[2][1] = imagBuffer[cornerY[1]][cornerX[1]][1];
				cornerColor[2][2] = imagBuffer[cornerY[1]][cornerX[1]][2];
			}
			else
			{
				cornerColor[2][0] = 0;
				cornerColor[2][1] = 0;
				cornerColor[2][2] = 0;
			}
			/* left bottom corner */
			if (0 <= cornerX[0] && cornerX[0] < width && cornerY[1] >= 0 && cornerY[1] < height)
			{
				cornerColor[3][0] = imagBuffer[cornerY[1]][cornerX[0]][0];
				cornerColor[3][1] = imagBuffer[cornerY[1]][cornerX[0]][1];
				cornerColor[3][2] = imagBuffer[cornerY[1]][cornerX[0]][2];
			}
			else
			{
				cornerColor[3][0] = 0;
				cornerColor[3][1] = 0;
				cornerColor[3][2] = 0;
			}
			
			/*colorr at rotated/zoomed pixel, Bilinear interpolation using info from 4 surrounding pixels */
			img.pixel(i, j)[0] = cornerColor[0][0]*(cornerX[1] - xNew)*(yNew - cornerY[0]) 
			                   + cornerColor[1][0]*(xNew - cornerX[0])*(yNew - cornerY[0]) 
							   + cornerColor[2][0]*(xNew - cornerX[0])*(cornerY[1] - yNew) 
							   + cornerColor[3][0]*(cornerX[1] - xNew)*(cornerY[1] - yNew);
			img.pixel(i, j)[1] = cornerColor[0][1]*(cornerX[1] - xNew)*(yNew - cornerY[0]) 
			                   + cornerColor[1][1]*(xNew - cornerX[0])*(yNew - cornerY[0]) 
							   + cornerColor[2][1]*(xNew - cornerX[0])*(cornerY[1] - yNew) 
							   + cornerColor[3][1]*(cornerX[1] - xNew)*(cornerY[1] - yNew);
			img.pixel(i, j)[2] = cornerColor[0][2]*(cornerX[1] - xNew)*(yNew - cornerY[0]) 
			                   + cornerColor[1][2]*(xNew - cornerX[0])*(yNew - cornerY[0]) 
							   + cornerColor[2][2]*(xNew - cornerX[0])*(cornerY[1] - yNew) 
							   + cornerColor[3][2]*(cornerX[1] - xNew)*(cornerY[1] - yNew);
		}
	} 
}

/* -------------------------------------------------------------------------- */
int main(int argc, char *argv[])
{
	/* window handle */
    int win;

	/* Read an RGB byte raster (of dimension w x h x 3) for an image */
	image = read_jpeg_image(infile);

	/* set the window dimensions to exactly fit the image. */
	/* Here, we assume the image size is 600x600 */
	if (image)
	{
		width  = image->width();
		height = image->height();
	}
	
	/* put original image info to a buffer */
	int i, j, k;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			/* copy each pixel info of the original image to the buffer */
			imagBuffer[i][j][0] = image->pixel(i,j)[0]; 
			imagBuffer[i][j][1] = image->pixel(i,j)[1];
			imagBuffer[i][j][2] = image->pixel(i,j)[2];
		}
	}
		
	/*-------------------------------------------------------------*/
	/* GLUT & GL initialization */
	/* initialize GLUT system */
    glutInit(&argc, argv);    
	/* initialize display format */
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	/* define and get handle to the window (render) context */
    glutInitWindowSize(width, height); 
    win = glutCreateWindow("HW1"); 

	/* set window's display callback */
    glutDisplayFunc(display_CB);       
	/* background events and animation */
	glutIdleFunc (idle_CB);
	/* set window's key callback */
    glutKeyboardFunc(key_CB);

	/* set background clear color to black */
    glClearColor((GLclampf)0.0,(GLclampf)0.0,(GLclampf)0.0,(GLclampf)0.0);  

	/* how object is mapped to window */
	gluOrtho2D(0, width, height, 0); 

	/* end GLUT & GL initialization */
	/*-------------------------------------------------------------*/

	/* start processing events...this is the event 'pump' */
    glutMainLoop();                      

    /* Execution never reaches this point, so return value is failure */
    return EXIT_FAILURE;
}


/* -------------------------------------------------------------------------- */
/* Function called whenever redisplay needed */
static void display_CB(void)
{
	/* clear the drawing buffer */
    glClear(GL_COLOR_BUFFER_BIT);        

	/* render a raster image into a viewport */
	int i, j;
	for (i = 0; i < image->height (); i++)
	{
		for (j = 0; j < image->width (); j++)
		{
			/* the R,G,B (byte) color of pixel(j,i) */
			glColor3d (image->pixel(i,j)[0]/255.,
					   image->pixel(i,j)[1]/255.,
					   image->pixel(i,j)[2]/255.);

			glBegin(GL_POINTS);
				glVertex2i (i,j);
			glEnd();          
		}
	}

	/* Complete any pending operations */
    glFlush();         

	/* Make the drawing buffer the frame buffer and vice versa */
    glutSwapBuffers(); 
}


/* -------------------------------------------------------------------------- */
/*
// This is the default background activity; 
// this function is called whenever no events are detected.
*/
static void idle_CB (void)
{
	/* rotation direction flag has been set. */
	if (anime_R)                    //rotate clockwise
		vrotatezoom (*image, 1);
	if (!anime_R)                   //rotate counter-clockwise
		vrotatezoom (*image, -1);
		
	/* state has changed: tell GLUT to redraw. */
	glutPostRedisplay();
}


/* -------------------------------------------------------------------------- */
/* Function is called on a key press */
static void key_CB(unsigned char key, int x, int y) 
{
    switch (key)
	{
		/* toggle rotation direction flags */
		case 'r':
			anime_R = !anime_R;
			break;
			
		/* quit the program -- 'hard quit' */
		case 'q':
			exit(0);
			break;
		default: 
			break;
	}
	/* state has changed: tell GLUT to redraw. */
	glutPostRedisplay ();
}

