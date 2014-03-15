/* 
 * CSC533 HomeWork 2
 * Group Member: Laiyong Mu, Xudong Weng
 * Accompanied programs [raster.cxx, raster.h, raster-jpeg.cxx] 
 * are from "Paintdemo" provided by the instructor
 */

 /* Assumptions in Basic mode:
 (1) Assuming the size of each plane of the cube is 600 x 600 pixels
 (2) Assuming the center of the Cube is at (0, 0, -600)
 (3) Assuming the view point is at (0, 0, 600)
 (4) Assuming the display window's z axis value = 0
 (4) The cube first rotate around Y-axis for one circle and then rotate around diagonal axis for one cycle
 (5) the dot light source is at infinite distance 
*/
 
#include <setjmp.h>
#include <stdio.h>       
#include <stdlib.h>   
#include <math.h>    
#include <GL/glu.h>       
#include <GL/glut.h>       
#include "raster.h"   

/* -------------------------------------------------------------------------- */
/* static variables used in basic mode */
static void display(void);            // Called whenever redisplay needed 
static void idle(void);            // Background activity 
static void key_CB(unsigned char key, int x, int y);
static int rotateTimes = 0;              // count the number of rotation steps, reset after each cycle 
static int rotateAxis = 1;               // 1 = Y axis, 2 = diagonal axis
static int background = 150;           //background color, 0-255, black to white 
static int cubeZ = -900;             // Z axis coordinate of the center of the cube 
static int imageSize = 600;          // the default image is square and 600x600 pixels

/* default display window size in pixels */
static int width  = 600;
static int height = 600;

/* rotation mode */
static int anime_B = 1;     // = 1, rotate in basic mode

/* a raster scan of the image */
static ByteRaster *image;

/* a buffer of the RGB color info for each plane (3 channels, total 600x600 pixels) in the original image file */
static int imagBuffer1[600][600][3];        // plane is parallel to z axis
static int imagBuffer2[600][600][3];        // plane is parallel to x axis
static int imagBuffer3[600][600][3];        // plane is parallel to z axis
static int imagBuffer4[600][600][3];        // plane is parallel to x axis
static int imagBuffer5[600][600][3];		// plane is parallel to X-Z plane-top
static int imagBuffer6[600][600][3];        // plane is parallel to X-Z plane-bottom

/* a buffer for pixels will displayed on the window */
static int imagBuffer[600][600][3];      // choose the default display window size 600x600

/* original view point */
static float viewPoint0[] = {0., 0., 600.};

/* rotation functions */
static float *rotateY(float X, float Y, float Z, float angle);     // rotate around y axis
static float *rotateDi(float X, float Y, float Z, float angle);    // rotate around diagonal axis (vector (1,1,1)

/* functions in Basic mode */
static void BasicIdle (void);
static void BasicDisplay(void);

extern void CreateWindow();
extern void Rotate();
extern void CreateWindown();
extern void ReadInFile(int face);
extern void UpdateTimer();
extern void InitGL();
extern void Configurate();
extern void Key_CB();

/* basic read image files */
void basicImageRead()
{
/* Read an RGB byte raster from an image from each plane and store in buffers */
	
	image = read_jpeg_image("m01.jpg");
	imageSize = image->width();
	int i, j;
	for (i = 0; i < imageSize; i++)
	{
		for (j = 0; j < imageSize; j++)
		{
			/* copy each pixel info of the original image to the buffer */
			imagBuffer1[i][j][0] = image->pixel(i,j)[0]; 
			imagBuffer1[i][j][1] = image->pixel(i,j)[1];
			imagBuffer1[i][j][2] = image->pixel(i,j)[2];
		} 
	}
	
	image = read_jpeg_image("m02.jpg");
	for (i = 0; i < imageSize; i++)
	{
		for (j = 0; j < imageSize; j++)
		{
			/* copy each pixel info of the original image to the buffer */
			imagBuffer2[i][j][0] = image->pixel(i,j)[0]; 
			imagBuffer2[i][j][1] = image->pixel(i,j)[1];
			imagBuffer2[i][j][2] = image->pixel(i,j)[2];
		} 
	}
	
	image = read_jpeg_image("m03.jpg");
	for (i = 0; i < imageSize; i++)
	{
		for (j = 0; j < imageSize; j++)
		{
			/* copy each pixel info of the original image to the buffer */
			imagBuffer3[i][j][0] = image->pixel(i,j)[0]; 
			imagBuffer3[i][j][1] = image->pixel(i,j)[1];
			imagBuffer3[i][j][2] = image->pixel(i,j)[2];
		} 
	}
	
	image = read_jpeg_image("m04.jpg");
	for (i = 0; i < imageSize; i++)
	{
		for (j = 0; j < imageSize; j++)
		{
			/* copy each pixel info of the original image to the buffer */
			imagBuffer4[i][j][0] = image->pixel(i,j)[0]; 
			imagBuffer4[i][j][1] = image->pixel(i,j)[1];
			imagBuffer4[i][j][2] = image->pixel(i,j)[2];
		} 
	}
	
	image = read_jpeg_image("m05.jpg");
	for (i = 0; i < imageSize; i++)
	{
		for (j = 0; j < imageSize; j++)
		{
			/* copy each pixel info of the original image to the buffer */
			imagBuffer5[i][j][0] = image->pixel(i,j)[0]; 
			imagBuffer5[i][j][1] = image->pixel(i,j)[1];
			imagBuffer5[i][j][2] = image->pixel(i,j)[2];
		} 
	}
	
	image = read_jpeg_image("m06.jpg");
	for (i = 0; i < imageSize; i++)
	{
		for (j = 0; j < imageSize; j++)
		{
			/* copy each pixel info of the original image to the buffer */
			imagBuffer6[i][j][0] = image->pixel(i,j)[0]; 
			imagBuffer6[i][j][1] = image->pixel(i,j)[1];
			imagBuffer6[i][j][2] = image->pixel(i,j)[2];
		} 
	}
		
}

static void key_CB(unsigned char key, int x, int y) 
{
	    switch (key)
	    {
		case 'g':
			ReadInFile(6);
			CreateWindow();
                 	InitGL();
		        while(true) {
			         UpdateTimer();
			          Rotate();
				  Configurate();
				  Key_CB();								          }
		break;
	    
	    	case 'q':
	    		exit(0);
		break;
	    }
	    glutPostRedisplay ();
}

/* basic mode - window display function */
void basicWindowDisplay(int *argc, char**argv)
{
	int win;
	/* GLUT & GL initialization */
	/* initialize GLUT system */
    glutInit(argc, argv);    
	/* initialize display format */
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	/* define and get handle to the window (render) context */
    glutInitWindowSize(width, height); 
    win = glutCreateWindow("HW2-Basic Mode"); 

	/* set window's display callback */
    glutDisplayFunc(BasicDisplay);       
	/* background events and animation */
	glutIdleFunc (BasicIdle);
	
	/* set background clear color to black */
    glClearColor((GLclampf)1.0,(GLclampf)1.0,(GLclampf)1.0,(GLclampf)0.0);  

	/* how object is mapped to window */
	gluOrtho2D(0, width, height, 0); 

	/* end GLUT & GL initialization */
	glutKeyboardFunc(key_CB);	
	/* start processing events...this is the event 'pump' */
    glutMainLoop();                      

}

/* -------------------------------------------------------------------------- */
/* function to rotate around y axis) */
float *rotateY(float X, float Y, float Z, float angle)
{
	static float newC[3];
	newC[1] = Y;
	newC[2] = Z*cos(angle) - X*sin(angle);
	newC[0] = Z*sin(angle) + X*cos(angle);
	return newC;
}

/* function to rotate around diagonal axis, vector (-1,1,-1)*/
float *rotateDi(float X, float Y, float Z, float angle)
{
	static float newC[3];
	float *temp;
	float X1,X2,Y1,Y2,Z1,Z2;
	// step 1: rotate the diagonal axis around Y axis 45 degree onto X-Z plane. 
	temp = rotateY(X,Y,Z,M_PI/4);
	X1 = *temp ;
	Y1 = *(temp+1);
	Z1 = *(temp+2);
	// step 2: rotate alpha around Z axis to overlap with Y axis, sin(alpha) = -sqrt(6)/3, cos(alpha) = sqrt(3)/3
	X2 = X1*sqrt(3)/3 + Y1*sqrt(6)/3;
	Y2 = -X1*sqrt(6)/3 + Y1*sqrt(3)/3;
	Z2 = Z1;
	// step 3: rotate around Diagonal axis (is Y axis now)
	temp = rotateY(X2,Y2,Z2,angle);
	X1 = *temp;
	Y1 = *(temp+1);
	Z1 = *(temp+2);
	// rotate (-alpha) around Z axis, sin = sqrt(6)/3, cos = sqrt(3)/3
	X2 = X1*sqrt(3)/3 - Y1*sqrt(6)/3;
	Y2 = X1*sqrt(6)/3 + Y1*sqrt(3)/3;
	Z2 = Z1;
	// rotate around Y axis -45 degree
	temp = rotateY(X2,Y2,Z2,-M_PI/4);
	newC[0] = *temp;
	newC[1] = *(temp+1);
	newC[2] = *(temp+2);
	return newC;
}

/* -------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	basicImageRead();
	basicWindowDisplay(&argc, argv);
}
*/

/* -------------------------------------------------------------------- */
/* Basic Mode: project 3d cube on to the 2d window based on the view point, drawing pixel by pixel */
/* Algorithm is to rotate the eye and the display window and then track the ray from eye and window pixel */

static void BasicProjection(void) 
{ 
	float window[3];          // coordinates of window pixels after rotation
	float viewPoint[3];       // coordinates of the eye position 
	float viewVector[3];      // coordinates of the vector of the observing ray originating from the view point;
	float lightVector[3] = {0., 0., 1.};  // the unit vector of the dot light source at infinite distance;
	float t, depth;           // 't' represent when the observing ray hit the plane, (p+t*N), p is the view point, N is the unit vector of the observing ray
	                          //'depth' is the current closest t;
	int i, j;                 // pixel position on the display window 
	
	int cornerColor[4][3];    // RGB info of 4 pixels in the image around the screen pixel with non-integer coordinates after rotation/zooming, from top left to bottom left, clockwise
	float xNew,yNew,zNew;     // new coordinates after rotation
	int cornerX[2], cornerY[2], cornerZ[2];   // coordinates of 4 surrounding pixel on the image file plane
	
	// rotate reset after each cycle (around Y-axis and diagonal axis)
	if (rotateTimes == -72)       // reset rotation angle and axis after 1 cycle, reset # = 2Pi/[rotation step size]
	{
		rotateTimes = 0;
		if (rotateAxis == 1)     // change from Y-axis rotation to Diagonal-axis rotation
			rotateAxis = 2;
		else	                 // change from Diagonal-axis rotation to Y-axis rotation
			rotateAxis = 1;
	}	
	
	// rotation angle related to the original image position
	float theta = rotateTimes*M_PI/36;          // current rotation step size = Pi/36
	float cosTheta[6];        // calculate cos value between light vector and the plane 
	float *tempC;             // temporary pointer to the coordinates after rotation
	
	/* rotate around the line that pass through the center of the cube and parallel to the Y axis */
	if (rotateAxis == 1)
	{
		// rotate the view point
		tempC = rotateY(viewPoint0[0], viewPoint0[1], viewPoint0[2]-cubeZ, theta);
		viewPoint[0] = *tempC;
		viewPoint[1] = *(tempC+1);
		viewPoint[2] = *(tempC+2) + cubeZ;
		// rotate the light vector
		tempC = rotateY(lightVector[0], lightVector[1], lightVector[2], theta);
		lightVector[0] = *tempC;
		lightVector[1] = *(tempC+1);
		lightVector[2] = *(tempC+2); 
		// calculate the light vector projection on the plane
		cosTheta[0] = ((lightVector[2]*1)/(sqrt(pow(lightVector[1],2)+pow(lightVector[0],2)+pow(lightVector[2],2))));
		cosTheta[1] = ((lightVector[0]*1)/(sqrt(pow(lightVector[1],2)+pow(lightVector[0],2)+pow(lightVector[2],2))));
		cosTheta[2] = -((lightVector[2]*1)/(sqrt(pow(lightVector[1],2)+pow(lightVector[0],2)+pow(lightVector[2],2))));	
		cosTheta[3] = -((lightVector[0]*1)/(sqrt(pow(lightVector[1],2)+pow(lightVector[0],2)+pow(lightVector[2],2))));
	}
	
	/* rotate around diagonal axis that pass through the center of the cube */
	if (rotateAxis == 2)
	{
		// rotate the view point
		tempC = rotateDi(viewPoint0[0], viewPoint0[1], viewPoint0[2]-cubeZ, theta);
		viewPoint[0] = *tempC;
		viewPoint[1] = *(tempC+1);
		viewPoint[2] = *(tempC+2) + cubeZ;
		// rotate the light vector
		tempC = rotateDi(lightVector[0], lightVector[1], lightVector[2], theta);
		lightVector[1] = *(tempC+1);
		lightVector[0] = *tempC;
		lightVector[2] = *(tempC+2); 
		// calculate the light vector projection on the plane
		cosTheta[0] = ((lightVector[2]*1)/(sqrt(pow(lightVector[0],2)+pow(lightVector[1],2)+pow(lightVector[2],2))));
		cosTheta[1] = ((lightVector[0]*1)/(sqrt(pow(lightVector[0],2)+pow(lightVector[1],2)+pow(lightVector[2],2))));
		cosTheta[2] = -((lightVector[2]*1)/(sqrt(pow(lightVector[0],2)+pow(lightVector[1],2)+pow(lightVector[2],2))));	
		cosTheta[3] = -((lightVector[0]*1)/(sqrt(pow(lightVector[0],2)+pow(lightVector[1],2)+pow(lightVector[2],2))));
		cosTheta[4] = ((lightVector[1]*1)/(sqrt(pow(lightVector[1],2)+pow(lightVector[0],2)+pow(lightVector[2],2))));
		cosTheta[5] = -((lightVector[1]*1)/(sqrt(pow(lightVector[1],2)+pow(lightVector[0],2)+pow(lightVector[2],2))));
	}
		
	for (i = 0; i < width; i++)       
	{
		for (j = 0; j < height; j++)   
		{
			if (rotateAxis == 1)   // rotate window pixel around y-axis
			{
				tempC = rotateY(i-(width/2), j-(height/2), -cubeZ, theta);
				window[0] = *tempC;
				window[1] = *(tempC+1);
				window[2] = *(tempC+2) + cubeZ;
			}

			if (rotateAxis == 2)  // rotate window pixel around diagonal axis
			{
				tempC = rotateDi(i-(width/2), j-(height/2), -cubeZ, theta);
				window[1] = *(tempC+1);
				window[0] = *tempC;
				window[2] = *(tempC+2) + cubeZ;
			}			
			
			// calculate the viewVector for the observing ray
			viewVector[0] = window[0] - viewPoint[0];
			viewVector[1] = window[1] - viewPoint[1];
			viewVector[2] = window[2] - viewPoint[2];
			depth = pow(2,16);     // initiation: the plane which hit by the observing ray is far far away
			
			/* Plane 1*/
			if (viewVector[2] * 1 < 0)     // plane 1 normal dot product with viewVector
			{
				t = ((imageSize/2)+cubeZ-viewPoint[2])/viewVector[2];   // when does the ray hit the plane
				if (t >= 0 && t < depth)   // t<0 -> not hit; t<depth -> another plane is in front of the current plane
				{
					xNew = viewVector[0] * t + viewPoint[0];
					yNew = viewVector[1] * t + viewPoint[1];
										
					cornerX[0] = (int)xNew;
					cornerX[1] = (int)xNew + 1;
					cornerY[0] = (int)yNew;
					cornerY[1] = (int)yNew + 1;
					
					// find the adjacent pixels of the image on plane where the ray hit the plane, otherwise, paint in background color
					if (((-(imageSize/2) <= cornerX[0] && cornerX[0] < (imageSize/2)) || (-(imageSize/2) <= cornerX[1] && cornerX[1] < (imageSize/2))) &&
						((cornerY[0] >= -(imageSize/2) && cornerY[0] < (imageSize/2)) || (cornerY[1] >= -(imageSize/2) && cornerY[1] < (imageSize/2))))
					{
						depth = t;
						
						/* get RGB color info for 4 surrounding pixels
						/* left top corner */
						if (-(imageSize/2) <= cornerX[0] && cornerX[0] < (imageSize/2) && cornerY[0] >= -(imageSize/2) && cornerY[0] < (imageSize/2))
						{	
							cornerColor[0][0] = imagBuffer1[cornerX[0]+(imageSize/2)][cornerY[0]+(imageSize/2)][0];
							cornerColor[0][1] = imagBuffer1[cornerX[0]+(imageSize/2)][cornerY[0]+(imageSize/2)][1];
							cornerColor[0][2] = imagBuffer1[cornerX[0]+(imageSize/2)][cornerY[0]+(imageSize/2)][2];
						}
						else
						{
							cornerColor[0][0] = background;
							cornerColor[0][1] = background;
							cornerColor[0][2] = background;
						}
						/* right top corner */
						if (-(imageSize/2) <= cornerX[1] && cornerX[1] < (imageSize/2) && cornerY[0] >= -(imageSize/2) && cornerY[0] < (imageSize/2))
						{	
							cornerColor[1][0] = imagBuffer1[cornerX[1]+(imageSize/2)][cornerY[0]+(imageSize/2)][0];
							cornerColor[1][1] = imagBuffer1[cornerX[1]+(imageSize/2)][cornerY[0]+(imageSize/2)][1];
							cornerColor[1][2] = imagBuffer1[cornerX[1]+(imageSize/2)][cornerY[0]+(imageSize/2)][2];
						}	
						else
						{
							cornerColor[1][0] = background;
							cornerColor[1][1] = background;
							cornerColor[1][2] = background;
						}
						/* right bottom corner */
						if (-(imageSize/2) <= cornerX[1] && cornerX[1] < (imageSize/2) && cornerY[1] >= -(imageSize/2) && cornerY[1] < (imageSize/2))
						{	
							cornerColor[2][0] = imagBuffer1[cornerX[1]+(imageSize/2)][cornerY[1]+(imageSize/2)][0];
							cornerColor[2][1] = imagBuffer1[cornerX[1]+(imageSize/2)][cornerY[1]+(imageSize/2)][1];
							cornerColor[2][2] = imagBuffer1[cornerX[1]+(imageSize/2)][cornerY[1]+(imageSize/2)][2];
						}
						else
						{
							cornerColor[2][0] = background;
							cornerColor[2][1] = background;
							cornerColor[2][2] = background;
						}
						/* left bottom corner */
						if (-(imageSize/2) <= cornerX[0] && cornerX[0] < (imageSize/2) && cornerY[1] >= -(imageSize/2) && cornerY[1] < (imageSize/2))
						{	
							cornerColor[3][0] = imagBuffer1[cornerX[0]+(imageSize/2)][cornerY[1]+(imageSize/2)][0];
							cornerColor[3][1] = imagBuffer1[cornerX[0]+(imageSize/2)][cornerY[1]+(imageSize/2)][1];
							cornerColor[3][2] = imagBuffer1[cornerX[0]+(imageSize/2)][cornerY[1]+(imageSize/2)][2];
						}
						else
						{
							cornerColor[3][0] = background;
							cornerColor[3][1] = background;
							cornerColor[3][2] = background;
						}
						
						/*color at rotated pixel, Bilinear interpolation using info from 4 surrounding pixels */
						/* cosTheta is to simulate ight effect from a dot light source at infinite distance */
						imagBuffer[i][j][0] = (int) (cosTheta[0]*(cornerColor[0][0]*(cornerX[1] - xNew)*(yNew - cornerY[0]) 
										   + cornerColor[1][0]*(xNew - cornerX[0])*(yNew - cornerY[0]) 
										   + cornerColor[2][0]*(xNew - cornerX[0])*(cornerY[1] - yNew) 
										   + cornerColor[3][0]*(cornerX[1] - xNew)*(cornerY[1] - yNew)));
						imagBuffer[i][j][1] = (int)(cosTheta[0]*(cornerColor[0][1]*(cornerX[1] - xNew)*(yNew - cornerY[0]) 
										   + cornerColor[1][1]*(xNew - cornerX[0])*(yNew - cornerY[0]) 
										   + cornerColor[2][1]*(xNew - cornerX[0])*(cornerY[1] - yNew) 
										   + cornerColor[3][1]*(cornerX[1] - xNew)*(cornerY[1] - yNew)));
						imagBuffer[i][j][2] = (int)(cosTheta[0]*(cornerColor[0][2]*(cornerX[1] - xNew)*(yNew - cornerY[0]) 
										   + cornerColor[1][2]*(xNew - cornerX[0])*(yNew - cornerY[0]) 
										   + cornerColor[2][2]*(xNew - cornerX[0])*(cornerY[1] - yNew) 
										   + cornerColor[3][2]*(cornerX[1] - xNew)*(cornerY[1] - yNew)));
					}
				}
			}
			
			/* Plane 2*/
			if (viewVector[0] * 1 < 0)     // plane 2 normal dot product with viewVector
			{ 
				t = ((imageSize/2)-viewPoint[0])/viewVector[0];
				if (t >= 0 && t < depth) 
				{
					yNew = viewVector[1] * t + viewPoint[1];
					zNew = viewVector[2] * t + viewPoint[2];
					cornerY[0] = (int)yNew;
					cornerY[1] = (int)yNew + 1;
					cornerZ[0] = (int)zNew;
					cornerZ[1] = (int)zNew + 1;
					
					if (((-(imageSize/2) <= cornerY[0] && cornerY[0] < (imageSize/2)) || (-(imageSize/2) <= cornerY[1] && cornerY[1] < (imageSize/2))) && 
						((cornerZ[0] > cubeZ-(imageSize/2) && cornerZ[0] <= cubeZ+(imageSize/2)) || (cornerZ[1] > cubeZ-(imageSize/2) && cornerZ[1] <= cubeZ+(imageSize/2))))
					{	
						depth = t;            // the new closet plane to the view point for current window pixel
						/* get RGB color info for 4 surrounding pixels
						/* left top corner */
						if (-(imageSize/2) <= cornerY[0] && cornerY[0] < (imageSize/2) && cornerZ[0] > cubeZ-(imageSize/2) && cornerZ[0] <= cubeZ+(imageSize/2))
						{	
							cornerColor[0][0] = imagBuffer2[cubeZ+(imageSize/2)-cornerZ[0]][cornerY[0]+(imageSize/2)][0];
							cornerColor[0][1] = imagBuffer2[cubeZ+(imageSize/2)-cornerZ[0]][cornerY[0]+(imageSize/2)][1];
							cornerColor[0][2] = imagBuffer2[cubeZ+(imageSize/2)-cornerZ[0]][cornerY[0]+(imageSize/2)][2];
						}
						else
						{	
							cornerColor[0][0] = background;
							cornerColor[0][1] = background;
							cornerColor[0][2] = background;
						}
						/* right top corner */
						if (-(imageSize/2) <= cornerY[1] && cornerY[1] < (imageSize/2) && cornerZ[0] > cubeZ-(imageSize/2) && cornerZ[0] <= cubeZ+(imageSize/2))
						{	
							cornerColor[1][0] = imagBuffer2[cubeZ+(imageSize/2)-cornerZ[0]][cornerY[1]+(imageSize/2)][0];
							cornerColor[1][1] = imagBuffer2[cubeZ+(imageSize/2)-cornerZ[0]][cornerY[1]+(imageSize/2)][1];
							cornerColor[1][2] = imagBuffer2[cubeZ+(imageSize/2)-cornerZ[0]][cornerY[1]+(imageSize/2)][2];
						}	
						else
						{
							cornerColor[1][0] = background;
							cornerColor[1][1] = background;
							cornerColor[1][2] = background;
						}
						/* right bottom corner */
						if (-(imageSize/2) <= cornerY[1] && cornerY[1] < (imageSize/2) && cornerZ[1] > cubeZ-(imageSize/2) && cornerZ[1] <= cubeZ+(imageSize/2))
						{	
							cornerColor[2][0] = imagBuffer2[cubeZ+(imageSize/2)-cornerZ[1]][cornerY[1]+(imageSize/2)][0];
							cornerColor[2][1] = imagBuffer2[cubeZ+(imageSize/2)-cornerZ[1]][cornerY[1]+(imageSize/2)][1];
							cornerColor[2][2] = imagBuffer2[cubeZ+(imageSize/2)-cornerZ[1]][cornerY[1]+(imageSize/2)][2];
						}
						else
						{
							cornerColor[2][0] = background;
							cornerColor[2][1] = background;
							cornerColor[2][2] = background;
						}
						/* left bottom corner */
						if (-(imageSize/2) <= cornerY[0] && cornerY[0] < (imageSize/2) && cornerZ[1] > cubeZ-(imageSize/2) && cornerZ[1] <= cubeZ+(imageSize/2))
						{	
							cornerColor[3][0] = imagBuffer2[cubeZ+(imageSize/2)-cornerZ[1]][cornerY[0]+(imageSize/2)][0];
							cornerColor[3][1] = imagBuffer2[cubeZ+(imageSize/2)-cornerZ[1]][cornerY[0]+(imageSize/2)][1];
							cornerColor[3][2] = imagBuffer2[cubeZ+(imageSize/2)-cornerZ[1]][cornerY[0]+(imageSize/2)][2];
						}
						else
						{
							cornerColor[3][0] = background;
							cornerColor[3][1] = background;
							cornerColor[3][2] = background;
						}
						
						/*color at rotated/zoomed pixel, Bilinear interpolation using info from 4 surrounding pixels */
						imagBuffer[i][j][0] = (int)(cosTheta[1]*(cornerColor[0][0]*(cornerZ[1] - zNew)*(yNew - cornerY[0]) 
										   + cornerColor[1][0]*(zNew - cornerZ[0])*(yNew - cornerY[0]) 
										   + cornerColor[2][0]*(zNew - cornerZ[0])*(cornerY[1] - yNew) 
										   + cornerColor[3][0]*(cornerZ[1] - zNew)*(cornerY[1] - yNew)));
						imagBuffer[i][j][1] = (int)(cosTheta[1]*(cornerColor[0][1]*(cornerZ[1] - zNew)*(yNew - cornerY[0]) 
										   + cornerColor[1][1]*(zNew - cornerZ[0])*(yNew - cornerY[0]) 
										   + cornerColor[2][1]*(zNew - cornerZ[0])*(cornerY[1] - yNew) 
										   + cornerColor[3][1]*(cornerZ[1] - zNew)*(cornerY[1] - yNew)));
						imagBuffer[i][j][2] = (int)(cosTheta[1]*(cornerColor[0][2]*(cornerZ[1] - zNew)*(yNew - cornerY[0]) 
										   + cornerColor[1][2]*(zNew - cornerZ[0])*(yNew - cornerY[0]) 
										   + cornerColor[2][2]*(zNew - cornerZ[0])*(cornerY[1] - yNew) 
										   + cornerColor[3][2]*(cornerZ[1] - zNew)*(cornerY[1] - yNew)));
					}
				}
			}
			
			/* Plane 3*/			
			if (viewVector[2] * 1 > 0)     // plane 3 normal dot product with viewVector
			{
				t = (cubeZ-(imageSize/2)-viewPoint[2])/viewVector[2];
				if ( t >= 0 && t < depth ) 
				{
					xNew = viewVector[0] * t + viewPoint[0];
					yNew = viewVector[1] * t + viewPoint[1];
									
					cornerX[0] = (int)xNew;
					cornerX[1] = (int)xNew + 1;
					cornerY[0] = (int)yNew;
					cornerY[1] = (int)yNew + 1;
					
					if (((-(imageSize/2) < cornerX[0] && cornerX[0] <= (imageSize/2)) || (-(imageSize/2) < cornerX[1] && cornerX[1] <= (imageSize/2))) &&
						((cornerY[0] >= -(imageSize/2) && cornerY[0] < (imageSize/2)) || (cornerY[1] >= -(imageSize/2) && cornerY[1] < (imageSize/2))))
					{
						depth = t;
					
						/* get RGB color info for 4 surrounding pixels
						/* left top corner */
						if (-(imageSize/2) < cornerX[0] && cornerX[0] <= (imageSize/2) && cornerY[0] >= -(imageSize/2) && cornerY[0] < (imageSize/2))
						{	
							cornerColor[0][0] = imagBuffer3[-cornerX[0]+(imageSize/2)][cornerY[0]+(imageSize/2)][0];
							cornerColor[0][1] = imagBuffer3[-cornerX[0]+(imageSize/2)][cornerY[0]+(imageSize/2)][1];
							cornerColor[0][2] = imagBuffer3[-cornerX[0]+(imageSize/2)][cornerY[0]+(imageSize/2)][2];
						}
						else
						{
							cornerColor[0][0] = background;
							cornerColor[0][1] = background;
							cornerColor[0][2] = background;
						}
						/* right top corner */
						if (-(imageSize/2) < cornerX[1] && cornerX[1] <= (imageSize/2) && cornerY[0] >= -(imageSize/2) && cornerY[0] < (imageSize/2))
						{	
							cornerColor[1][0] = imagBuffer3[-cornerX[1]+(imageSize/2)][cornerY[0]+(imageSize/2)][0];
							cornerColor[1][1] = imagBuffer3[-cornerX[1]+(imageSize/2)][cornerY[0]+(imageSize/2)][1];
							cornerColor[1][2] = imagBuffer3[-cornerX[1]+(imageSize/2)][cornerY[0]+(imageSize/2)][2];
						}	
						else
						{
							cornerColor[1][0] = background;
							cornerColor[1][1] = background;
							cornerColor[1][2] = background;
						}
						/* right bottom corner */
						if (-(imageSize/2) < cornerX[1] && cornerX[1] <= (imageSize/2) && cornerY[1] >= -(imageSize/2) && cornerY[1] < (imageSize/2))
						{	
							cornerColor[2][0] = imagBuffer3[-cornerX[1]+(imageSize/2)][cornerY[1]+(imageSize/2)][0];
							cornerColor[2][1] = imagBuffer3[-cornerX[1]+(imageSize/2)][cornerY[1]+(imageSize/2)][1];
							cornerColor[2][2] = imagBuffer3[-cornerX[1]+(imageSize/2)][cornerY[1]+(imageSize/2)][2];
						}
						else
						{
							cornerColor[2][0] = background;
							cornerColor[2][1] = background;
							cornerColor[2][2] = background;
						}
						/* left bottom corner */
						if (-(imageSize/2) < cornerX[0] && cornerX[0] <= (imageSize/2) && cornerY[1] >= -(imageSize/2) && cornerY[1] < (imageSize/2))
						{	
							cornerColor[3][0] = imagBuffer3[-cornerX[0]+(imageSize/2)][cornerY[1]+(imageSize/2)][0];
							cornerColor[3][1] = imagBuffer3[-cornerX[0]+(imageSize/2)][cornerY[1]+(imageSize/2)][1];
							cornerColor[3][2] = imagBuffer3[-cornerX[0]+(imageSize/2)][cornerY[1]+(imageSize/2)][2];
						}
						else
						{
							cornerColor[3][0] = background;
							cornerColor[3][1] = background;
							cornerColor[3][2] = background;
						}
						
						/*color at rotated/zoomed pixel, Bilinear interpolation using info from 4 surrounding pixels */
						imagBuffer[i][j][0] = (int)(cosTheta[2]*(cornerColor[0][0]*(cornerX[1] - xNew)*(yNew - cornerY[0]) 
										   + cornerColor[1][0]*(xNew - cornerX[0])*(yNew - cornerY[0]) 
										   + cornerColor[2][0]*(xNew - cornerX[0])*(cornerY[1] - yNew) 
										   + cornerColor[3][0]*(cornerX[1] - xNew)*(cornerY[1] - yNew)));
						imagBuffer[i][j][1] = (int)(cosTheta[2]*(cornerColor[0][1]*(cornerX[1] - xNew)*(yNew - cornerY[0]) 
										   + cornerColor[1][1]*(xNew - cornerX[0])*(yNew - cornerY[0]) 
										   + cornerColor[2][1]*(xNew - cornerX[0])*(cornerY[1] - yNew) 
										   + cornerColor[3][1]*(cornerX[1] - xNew)*(cornerY[1] - yNew)));
						imagBuffer[i][j][2] = (int)(cosTheta[2]*(cornerColor[0][2]*(cornerX[1] - xNew)*(yNew - cornerY[0]) 
										   + cornerColor[1][2]*(xNew - cornerX[0])*(yNew - cornerY[0]) 
										   + cornerColor[2][2]*(xNew - cornerX[0])*(cornerY[1] - yNew) 
										   + cornerColor[3][2]*(cornerX[1] - xNew)*(cornerY[1] - yNew)));
					}
				}
			}
		
		/* Plane 4*/
		if (viewVector[0] * 1 > 0)     // plane 4 normal dot product with viewVector
			{
				t = -((imageSize/2)+viewPoint[0])/viewVector[0];
				if (t >=0 && t < depth) 
				{
					yNew = viewVector[1] * t + viewPoint[1];
					zNew = viewVector[2] * t + viewPoint[2];
					
					cornerY[0] = (int)yNew;
					cornerY[1] = (int)yNew + 1;
					cornerZ[0] = (int)zNew;
					cornerZ[1] = (int)zNew + 1;
					
					if (((-(imageSize/2) <= cornerY[0] && cornerY[0] < (imageSize/2)) || (-(imageSize/2) <= cornerY[1] && cornerY[1] < (imageSize/2) )) &&
						((cornerZ[0] >= cubeZ-(imageSize/2) && cornerZ[0] < cubeZ+(imageSize/2)) || (cornerZ[1] >= cubeZ-(imageSize/2) && cornerZ[1] < cubeZ+(imageSize/2))))
					{
						depth = t;
						
						/* get RGB color info for 4 surrounding pixels
						/* left top corner */
												
						if (-(imageSize/2) <= cornerY[0] && cornerY[0] < (imageSize/2) && cornerZ[0] >= cubeZ-(imageSize/2) && cornerZ[0] < cubeZ+(imageSize/2))
						{	depth = t;
							cornerColor[0][0] = imagBuffer4[cornerZ[0]-(cubeZ-(imageSize/2))][cornerY[0]+(imageSize/2)][0];
							cornerColor[0][1] = imagBuffer4[cornerZ[0]-(cubeZ-(imageSize/2))][cornerY[0]+(imageSize/2)][1];
							cornerColor[0][2] = imagBuffer4[cornerZ[0]-(cubeZ-(imageSize/2))][cornerY[0]+(imageSize/2)][2];
						}
						else
						{
							cornerColor[0][0] = background;
							cornerColor[0][1] = background;
							cornerColor[0][2] = background;
						}
						/* right top corner */
						if (-(imageSize/2) <= cornerY[1] && cornerY[1] < (imageSize/2) && cornerZ[0] >= cubeZ-(imageSize/2) && cornerZ[0] < cubeZ+(imageSize/2))
						{	depth = t;
							cornerColor[1][0] = imagBuffer4[cornerZ[0]-(cubeZ-(imageSize/2))][cornerY[1]+(imageSize/2)][0];
							cornerColor[1][1] = imagBuffer4[cornerZ[0]-(cubeZ-(imageSize/2))][cornerY[1]+(imageSize/2)][1];
							cornerColor[1][2] = imagBuffer4[cornerZ[0]-(cubeZ-(imageSize/2))][cornerY[1]+(imageSize/2)][2];
						}	
						else
						{
							cornerColor[1][0] = background;
							cornerColor[1][1] = background;
							cornerColor[1][2] = background;
						}
						/* right bottom corner */
						if (-(imageSize/2) <= cornerY[1] && cornerY[1] < (imageSize/2) && cornerZ[1] >= cubeZ-(imageSize/2) && cornerZ[1] < cubeZ+(imageSize/2))
						{	depth = t;
							cornerColor[2][0] = imagBuffer4[cornerZ[1]-(cubeZ-(imageSize/2))][cornerY[1]+(imageSize/2)][0];
							cornerColor[2][1] = imagBuffer4[cornerZ[1]-(cubeZ-(imageSize/2))][cornerY[1]+(imageSize/2)][1];
							cornerColor[2][2] = imagBuffer4[cornerZ[1]-(cubeZ-(imageSize/2))][cornerY[1]+(imageSize/2)][2];
						}
						else
						{
							cornerColor[2][0] = background;
							cornerColor[2][1] = background;
							cornerColor[2][2] = background;
						}
						/* left bottom corner */
						if (-(imageSize/2) <= cornerY[0] && cornerY[0] < (imageSize/2) && cornerZ[1] >= cubeZ-(imageSize/2) && cornerZ[1] < cubeZ+(imageSize/2))
						{	depth = t;
							cornerColor[3][0] = imagBuffer4[cornerZ[1]-(cubeZ-(imageSize/2))][cornerY[0]+(imageSize/2)][0];
							cornerColor[3][1] = imagBuffer4[cornerZ[1]-(cubeZ-(imageSize/2))][cornerY[0]+(imageSize/2)][1];
							cornerColor[3][2] = imagBuffer4[cornerZ[1]-(cubeZ-(imageSize/2))][cornerY[0]+(imageSize/2)][2];
						}
						else
						{
							cornerColor[3][0] = background;
							cornerColor[3][1] = background;
							cornerColor[3][2] = background;
						}
						
						/*color at rotated/zoomed pixel, Bilinear interpolation using info from 4 surrounding pixels */
						imagBuffer[i][j][0] = (int)(cosTheta[3]*(cornerColor[0][0]*(cornerZ[1] - zNew)*(yNew - cornerY[0]) 
										   + cornerColor[1][0]*(zNew - cornerZ[0])*(yNew - cornerY[0]) 
										   + cornerColor[2][0]*(zNew - cornerZ[0])*(cornerY[1] - yNew) 
										   + cornerColor[3][0]*(cornerZ[1] - zNew)*(cornerY[1] - yNew)));
						imagBuffer[i][j][1] = (int)(cosTheta[3]*(cornerColor[0][1]*(cornerZ[1] - zNew)*(yNew - cornerY[0]) 
										   + cornerColor[1][1]*(zNew - cornerZ[0])*(yNew - cornerY[0]) 
										   + cornerColor[2][1]*(zNew - cornerZ[0])*(cornerY[1] - yNew) 
										   + cornerColor[3][1]*(cornerZ[1] - zNew)*(cornerY[1] - yNew)));
						imagBuffer[i][j][2] = (int)(cosTheta[3]*(cornerColor[0][2]*(cornerZ[1] - zNew)*(yNew - cornerY[0]) 
										   + cornerColor[1][2]*(zNew - cornerZ[0])*(yNew - cornerY[0]) 
										   + cornerColor[2][2]*(zNew - cornerZ[0])*(cornerY[1] - yNew) 
										   + cornerColor[3][2]*(cornerZ[1] - zNew)*(cornerY[1] - yNew)));
					}
				}
			}
			
			/* Plane 5*/
			if (viewVector[1] * 1 > 0)     // plane 5 normal dot product with viewVector
			{
				t = (-(imageSize/2)-viewPoint[1])/viewVector[1];
				if (t >= 0 && t < depth)
				{
					
					xNew = viewVector[0] * t + viewPoint[0];
					zNew = viewVector[2] * t + viewPoint[2];
										
					cornerX[0] = (int)xNew;
					cornerX[1] = (int)xNew + 1;
					cornerZ[0] = (int)zNew;
					cornerZ[1] = (int)zNew + 1;
					
					if (((-(imageSize/2) <= cornerX[0] && cornerX[0] < (imageSize/2)) || (-(imageSize/2) <= cornerX[1] && cornerX[1] < (imageSize/2))) &&
						((cornerZ[0] >= cubeZ-(imageSize/2) && cornerZ[0] < cubeZ+(imageSize/2)) || (cornerZ[1] >= cubeZ-(imageSize/2) && cornerZ[1] < cubeZ+(imageSize/2))))
					{
						depth = t;
						
						/* get RGB color info for 4 surrounding pixels
						/* left top corner */
						if (-(imageSize/2) <= cornerX[0] && cornerX[0] < (imageSize/2) && cornerZ[0] >= cubeZ-(imageSize/2) && cornerZ[0] < cubeZ+(imageSize/2))
						{	
							cornerColor[0][0] = imagBuffer5[cornerX[0]+(imageSize/2)][cornerZ[0]-(cubeZ-(imageSize/2))][0];
							cornerColor[0][1] = imagBuffer5[cornerX[0]+(imageSize/2)][cornerZ[0]-(cubeZ-(imageSize/2))][1];
							cornerColor[0][2] = imagBuffer5[cornerX[0]+(imageSize/2)][cornerZ[0]-(cubeZ-(imageSize/2))][2];
						}
						else
						{
							cornerColor[0][0] = background;
							cornerColor[0][1] = background;
							cornerColor[0][2] = background;
						}
						/* right top corner */
						if (-(imageSize/2) <= cornerX[1] && cornerX[1] < (imageSize/2) && cornerZ[0] >= cubeZ-(imageSize/2) && cornerZ[0] < cubeZ+(imageSize/2))
						{	
							cornerColor[1][0] = imagBuffer5[cornerX[1]+(imageSize/2)][cornerZ[0]-(cubeZ-(imageSize/2))][0];
							cornerColor[1][1] = imagBuffer5[cornerX[1]+(imageSize/2)][cornerZ[0]-(cubeZ-(imageSize/2))][1];
							cornerColor[1][2] = imagBuffer5[cornerX[1]+(imageSize/2)][cornerZ[0]-(cubeZ-(imageSize/2))][2];
						}	
						else
						{
							cornerColor[1][0] = background;
							cornerColor[1][1] = background;
							cornerColor[1][2] = background;
						}
						/* right bottom corner */
						if (-(imageSize/2) <= cornerX[1] && cornerX[1] < (imageSize/2) && cornerZ[1] >= cubeZ-(imageSize/2) && cornerZ[1] < cubeZ+(imageSize/2))
						{	
							cornerColor[2][0] = imagBuffer5[cornerX[1]+(imageSize/2)][cornerZ[0]-(cubeZ-(imageSize/2))][0];
							cornerColor[2][1] = imagBuffer5[cornerX[1]+(imageSize/2)][cornerZ[0]-(cubeZ-(imageSize/2))][1];
							cornerColor[2][2] = imagBuffer5[cornerX[1]+(imageSize/2)][cornerZ[0]-(cubeZ-(imageSize/2))][2];
						}
						else
						{
							cornerColor[2][0] = background;
							cornerColor[2][1] = background;
							cornerColor[2][2] = background;
						}
						/* left bottom corner */
						if (-(imageSize/2) <= cornerX[0] && cornerX[0] < (imageSize/2) && cornerZ[1] >= cubeZ-(imageSize/2) && cornerZ[1] < cubeZ+(imageSize/2))
						{	
							cornerColor[3][0] = imagBuffer5[cornerX[0]+(imageSize/2)][cornerZ[0]-(cubeZ-(imageSize/2))][0];
							cornerColor[3][1] = imagBuffer5[cornerX[0]+(imageSize/2)][cornerZ[0]-(cubeZ-(imageSize/2))][1];
							cornerColor[3][2] = imagBuffer5[cornerX[0]+(imageSize/2)][cornerZ[0]-(cubeZ-(imageSize/2))][2];
						}
						else
						{
							cornerColor[3][0] = background;
							cornerColor[3][1] = background;
							cornerColor[3][2] = background;
						}
						
						/*color at rotated/zoomed pixel, Bilinear interpolation using info from 4 surrounding pixels */
						imagBuffer[i][j][0] = (int) (cosTheta[5]*(cornerColor[0][0]*(cornerX[1] - xNew)*(zNew - cornerZ[0]) 
										   + cornerColor[1][0]*(xNew - cornerX[0])*(zNew - cornerZ[0]) 
										   + cornerColor[2][0]*(xNew - cornerX[0])*(cornerZ[1] - zNew) 
										   + cornerColor[3][0]*(cornerX[1] - xNew)*(cornerZ[1] - zNew)));
						imagBuffer[i][j][1] = (int)(cosTheta[5]*(cornerColor[0][1]*(cornerX[1] - xNew)*(zNew - cornerZ[0]) 
										   + cornerColor[1][1]*(xNew - cornerX[0])*(zNew - cornerZ[0]) 
										   + cornerColor[2][1]*(xNew - cornerX[0])*(cornerZ[1] - zNew) 
										   + cornerColor[3][1]*(cornerX[1] - xNew)*(cornerZ[1] - zNew)));
						imagBuffer[i][j][2] = (int)(cosTheta[5]*(cornerColor[0][2]*(cornerX[1] - xNew)*(zNew - cornerZ[0]) 
										   + cornerColor[1][2]*(xNew - cornerX[0])*(zNew - cornerZ[0]) 
										   + cornerColor[2][2]*(xNew - cornerX[0])*(cornerZ[1] - zNew) 
										   + cornerColor[3][2]*(cornerX[1] - xNew)*(cornerZ[1] - zNew)));
					}
				}
			}
			/* Plane 6*/
			if (viewVector[1] * 1 < 0)     // plane 6 normal dot product with viewVector
			{
				t = ((imageSize/2)-viewPoint[1])/viewVector[1];
				if (t >= 0 && t < depth)
				{
					
					xNew = viewVector[0] * t + viewPoint[0];
					zNew = viewVector[2] * t + viewPoint[2];
										
					cornerX[0] = (int)xNew;
					cornerX[1] = (int)xNew + 1;
					cornerZ[0] = (int)zNew;
					cornerZ[1] = (int)zNew + 1;
					
					if (((-(imageSize/2) <= cornerX[0] && cornerX[0] < (imageSize/2)) || (-(imageSize/2) <= cornerX[1] && cornerX[1] < (imageSize/2))) &&
						((cornerZ[0] > cubeZ-(imageSize/2) && cornerZ[0] <= cubeZ+(imageSize/2)) || (cornerZ[1] > cubeZ-(imageSize/2) && cornerZ[1] <= cubeZ+(imageSize/2))))
					{
						depth = t;
						
						/* get RGB color info for 4 surrounding pixels
						/* left top corner */
						if (-(imageSize/2) <= cornerX[0] && cornerX[0] < (imageSize/2) && cornerZ[0] > cubeZ-(imageSize/2) && cornerZ[0] <= cubeZ+(imageSize/2))
						{	
							cornerColor[0][0] = imagBuffer6[cornerX[0]+(imageSize/2)][-cornerZ[1]-(imageSize/2)][0];
							cornerColor[0][1] = imagBuffer6[cornerX[0]+(imageSize/2)][-cornerZ[1]-(imageSize/2)][1];
							cornerColor[0][2] = imagBuffer6[cornerX[0]+(imageSize/2)][-cornerZ[1]-(imageSize/2)][2];
						}
						else
						{
							cornerColor[0][0] = background;
							cornerColor[0][1] = background;
							cornerColor[0][2] = background;
						}
						/* right top corner */
						if (-(imageSize/2) <= cornerX[1] && cornerX[1] < (imageSize/2) && cornerZ[0] > cubeZ-(imageSize/2) && cornerZ[0] <= cubeZ+(imageSize/2))
						{	
							cornerColor[1][0] = imagBuffer6[cornerX[1]+(imageSize/2)][-cornerZ[1]+(cubeZ+(imageSize/2))][0];
							cornerColor[1][1] = imagBuffer6[cornerX[1]+(imageSize/2)][-cornerZ[1]+(cubeZ+(imageSize/2))][1];
							cornerColor[1][2] = imagBuffer6[cornerX[1]+(imageSize/2)][-cornerZ[1]+(cubeZ+(imageSize/2))][2];
						}	
						else
						{
							cornerColor[1][0] = background;
							cornerColor[1][1] = background;
							cornerColor[1][2] = background;
						}
						/* right bottom corner */
						if (-(imageSize/2) <= cornerX[1] && cornerX[1] < (imageSize/2) && cornerZ[1] > cubeZ-(imageSize/2) && cornerZ[1] <= cubeZ+(imageSize/2))
						{	
							cornerColor[2][0] = imagBuffer6[cornerX[1]+(imageSize/2)][-cornerZ[1]+(cubeZ+(imageSize/2))][0];
							cornerColor[2][1] = imagBuffer6[cornerX[1]+(imageSize/2)][-cornerZ[1]+(cubeZ+(imageSize/2))][1];
							cornerColor[2][2] = imagBuffer6[cornerX[1]+(imageSize/2)][-cornerZ[1]+(cubeZ+(imageSize/2))][2];
						}
						else
						{
							cornerColor[2][0] = background;
							cornerColor[2][1] = background;
							cornerColor[2][2] = background;
						}
						/* left bottom corner */
						if (-(imageSize/2) <= cornerX[0] && cornerX[0] < (imageSize/2) && cornerZ[1] > cubeZ-(imageSize/2) && cornerZ[1] <= cubeZ+(imageSize/2))
						{	
							cornerColor[3][0] = imagBuffer6[cornerX[0]+(imageSize/2)][-cornerZ[1]+(cubeZ+(imageSize/2))][0];
							cornerColor[3][1] = imagBuffer6[cornerX[0]+(imageSize/2)][-cornerZ[1]+(cubeZ+(imageSize/2))][1];
							cornerColor[3][2] = imagBuffer6[cornerX[0]+(imageSize/2)][-cornerZ[1]+(cubeZ+(imageSize/2))][2];
						}
						else
						{
							cornerColor[3][0] = background;
							cornerColor[3][1] = background;
							cornerColor[3][2] = background;
						}
						
						/*color at rotated/zoomed pixel, Bilinear interpolation using info from 4 surrounding pixels */
						imagBuffer[i][j][0] = (int) (cosTheta[4]*(cornerColor[0][0]*(cornerX[1] - xNew)*(zNew - cornerZ[0]) 
										   + cornerColor[1][0]*(xNew - cornerX[0])*(zNew - cornerZ[0]) 
										   + cornerColor[2][0]*(xNew - cornerX[0])*(cornerZ[1] - zNew) 
										   + cornerColor[3][0]*(cornerX[1] - xNew)*(cornerZ[1] - zNew)));
						imagBuffer[i][j][1] = (int)(cosTheta[4]*(cornerColor[0][1]*(cornerX[1] - xNew)*(zNew - cornerZ[0]) 
										   + cornerColor[1][1]*(xNew - cornerX[0])*(zNew - cornerZ[0]) 
										   + cornerColor[2][1]*(xNew - cornerX[0])*(cornerZ[1] - zNew) 
										   + cornerColor[3][1]*(cornerX[1] - xNew)*(cornerZ[1] - zNew)));
						imagBuffer[i][j][2] = (int)(cosTheta[4]*(cornerColor[0][2]*(cornerX[1] - xNew)*(zNew - cornerZ[0]) 
										   + cornerColor[1][2]*(xNew - cornerX[0])*(zNew - cornerZ[0]) 
										   + cornerColor[2][2]*(xNew - cornerX[0])*(cornerZ[1] - zNew) 
										   + cornerColor[3][2]*(cornerX[1] - xNew)*(cornerZ[1] - zNew)));
					}
				}
			}
			
			// if the observing ray does not hit any plane, draw the current window pixel as background color
			if (depth == pow(2,16))
				{
					imagBuffer[i][j][0] = background;
					imagBuffer[i][j][1] = background;
					imagBuffer[i][j][2] = background;
				}
		}
	}
	rotateTimes--;

}


/* -------------------------------------------------------------------------- */
/* Function called whenever redisplay needed */
static void BasicDisplay(void)
{
	/* clear the drawing buffer */
    glClear(GL_COLOR_BUFFER_BIT);        
    BasicProjection();
	/* render a raster image into a viewport */
	int i, j;
	for (i = 0; i < 600; i++)
	{
		for (j = 0; j < 600; j++)
		{
			/* the R,G,B (byte) color of pixel(i,j) */
			
			glColor3d (imagBuffer[i][j][0]/255.,
					   imagBuffer[i][j][1]/255.,
					   imagBuffer[i][j][2]/255.);
			
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
static void BasicIdle (void)
{
	/* rotation direction flag has been set. */
	if (anime_B)                    //rotate clockwise
		//BasicProjection();
			
	/* state has changed: tell GLUT to redraw. */
	glutPostRedisplay();
}

