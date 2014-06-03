
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glu.h>       
#include <GL/glut.h> 
#include <math.h> 
#include <string.h>    
#include "raster.h"

extern bool write_jpeg_image(const char *filename, const ByteRaster& img);
static ByteRaster *gimage[12];        // pixel buffer for 12 jpeg picture
static ByteRaster *OutputImage;       // pixel buffer to write out
extern ByteRaster *read_jpeg_image(const char *filename);
void ReadInFile(int face);           // read jpeg file to buffer
static int blocks = 12;
static char *infile[12];  
GLuint texture[12];                  // texture ID for different planes 
float disofPic = 2.0;
GLfloat rotation_matrix[16];        // rotation matrix in GL mode
int refreshMills = 300;              // refresh interval in milliseconds   
GLfloat angleGL = 0.0f; 
float Psize = 1.0f;    
int winMain;
//float	g_Angle = 283.0;	
int refreshTimes = 0;
char outfile[96][24];
int blockID = 0;

void createImposter()
{
	// create imposter names (12 blocks, 8 directions each)
	
	int i;
	char *path = "..\/proj3\/impos\/impos_";
	
	char ctemp[2];
	for (i = 0; i < 96; i++){
		strcpy (outfile[i], path);
		sprintf(ctemp, "%d", i);
		strcat(outfile[i], ctemp);
	}
}

void ReadInFile(int blocks){
	infile[0] = "t01.jpg";
	infile[1] = "t02.jpg";
	infile[2] = "t03.jpg";
	infile[3] = "t04.jpg";
	infile[4] = "t05.jpg";
	infile[5] = "t06.jpg";
	infile[6] = "t07.jpg";
	infile[7] = "t08.jpg";
	infile[8] = "t09.jpg";
	infile[9] = "t10.jpg";
	infile[10] = "t11.jpg";
	infile[11] = "t12.jpg";
	int i;
	
	for(i = 0; i < blocks; i++)
		gimage[i] = read_jpeg_image(infile[i]);
	OutputImage = read_jpeg_image("impos_template.jpg");
}



/* initiation */
void initGL() {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set background color to black and opaque
	glClearDepth(1.0f);                   // Set background depth to farthest
	
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

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
    //glClearColor(0.5f, 0.2f, 1.0f, 1.0f);         // Grey Background
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);         // Grey Background
    glClearDepth(1.0f);                           // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);                      // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);                       // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // Really Nice Perspective Calculations
}

// draw a pic in a block. pics in a block has the same orientation
// pic orientation ranges from 0, 45, 90, 135 degrees
void DrawOnePic(float x, float y, int blockID)
{
    LoadGLTextures(blockID);
	
	// pic orientation 0 degree
	if (blockID % 4 == 0) {
		glBegin(GL_QUADS);
			glNormal3d(0, -1, 0); //set the normal of this face for lighting
			glTexCoord2f(Psize, 0.0f); glVertex3f(x + Psize, y, Psize); //right top
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, Psize); //left top
			glTexCoord2f(0.0f, Psize); glVertex3f(x, y, 0.0); //left bottom
			glTexCoord2f(Psize, Psize); glVertex3f(x + Psize, y, 0.0);  //right bottom
		glEnd();
	}
	
	// pic orientation 45 degree
	if (blockID % 4 == 1) {
		glBegin(GL_QUADS);
			glNormal3d(-1, -1, 0); // set the normal of this face for lighting
			glTexCoord2f(Psize, 0.0f); glVertex3f(x + Psize, y, Psize);  // right top
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x + Psize/2, y + Psize/2, Psize); // left top
			glTexCoord2f(0.0f, Psize); glVertex3f(x + Psize/2, y + Psize/2, 0.0); // left bottom
			glTexCoord2f(Psize, Psize); glVertex3f(x + Psize, y, 0.0);  // right bottom
		glEnd();
	}
	// pic orientation 90 degree
	if (blockID % 4 == 2) {
		glBegin(GL_QUADS);
			glNormal3d(-1, 0, 0); // set the normal of this face for lighting
			glTexCoord2f(Psize, 0.0f); glVertex3f(x + Psize, y, Psize);  // right top
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x + Psize, y + Psize, Psize); // left top
			glTexCoord2f(0.0f, Psize); glVertex3f(x + Psize, y + Psize, 0.0); // left bottom
			glTexCoord2f(Psize, Psize); glVertex3f(x + Psize, y, 0.0);  // right bottom
		glEnd();
	}
	
	// pic orientation 135 degree
	if (blockID % 4 == 3) {
		glBegin(GL_QUADS);
			glNormal3d(-1, 1, 0); // set the normal of this face for lighting
			glTexCoord2f(Psize, 0.0f); glVertex3f(x, y, Psize);  // right top
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x + Psize/2, y + Psize/2, Psize); // left top
			glTexCoord2f(0.0f, Psize); glVertex3f(x + Psize/2, y + Psize/2, 0.0); // left bottom
			glTexCoord2f(Psize, Psize); glVertex3f(x, y, 0.0);  // right bottom
		glEnd();
	}
}

void DrawOneBlock(int blockID)
{
	int xpic = 4, ypic = 4; 
	int i;
		for(i = 0; i < xpic; i++){
			int j;
			for(j = 0; j < ypic; j++){
				float cx = i*disofPic;
				float cy = j*disofPic;
				DrawOnePic(cx, cy, blockID);
			}
		}
	
}

void OutputImpostor(int blockID)
{
	GLubyte data[3 * 1200 * 150];
	glReadPixels(0, 0, 1200, 150, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	int i, j;
	for (j = 149; j >= 0; j--){
		for (i = 0; i < 1200; i++) {
			OutputImage->pixel(i,j)[0] = data[(149-j)*1200*3+i*3]; //*255;
			OutputImage->pixel(i,j)[1] = data[(149-j)*1200*3+i*3+1]; //*255;
			OutputImage->pixel(i,j)[2] = data[(149-j)*1200*3+i*3+2]; //*255;
		}	
	}
	
	bool writeSuccess = 0;
	if (blockID == 0)
	{
		if (refreshTimes == 8)
			writeSuccess = write_jpeg_image (outfile[refreshTimes-8],*OutputImage);
		else
			writeSuccess = write_jpeg_image (outfile[refreshTimes],*OutputImage);
	}
	else
	{
		if (refreshTimes <= 8)
			writeSuccess = write_jpeg_image (outfile[blockID * 8 + refreshTimes -1],*OutputImage);
	}
	if (writeSuccess)
		printf("successfully output image \n");
	
}

void mainDisplay(){
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);    // Clear screen and Z-buffer
	
	glMatrixMode(GL_MODELVIEW);     
		glLoadIdentity();                  // Reset transformations
		glTranslatef(-0.0f, 0.0f, -13.5f);
		glRotatef(angleGL, 0.0f, 1.0f, 0.0f); 
		glTranslatef(0.0f, 0.0f, 13.5f);
		gluLookAt(3.5, -10, 0.5, 3.5, -5.5, 0.5, 0., 0., 1.);
	DrawOneBlock(blockID);
	glLoadIdentity();                // Reset the model-view matrix 
	              // Swap the front and back frame buffers (double buffering)
		
    glMatrixMode(GL_PROJECTION);    // To operate on the Projection matrix
		glLoadIdentity();               // Reset matrix
        gluPerspective(7.0f, 8.0f, 0.1f, 50.0f); // Enable perspective projection with fovy, aspect, zNear and zFar
		//gluPerspective(15.0f, 3.0f, 0.1f, 50.0f);
		
	glViewport(0, 0, 1200, 150);     // set up the display window coordinates
	//glViewport(0, 0, 1200, 400);     // set up the display window coordinates
	
	glFlush();                       //Complete any pending operations 
	//OutputImpostor(blockID);
	glutSwapBuffers(); 
	
}

/* refresh timer */
void timer(int value) {
	//OutputImpostor(blockID);
	
	OutputImpostor(blockID);
	printf("%d\n", refreshTimes);
	
	if (refreshTimes < 8 && blockID < 12){
		glutTimerFunc(refreshMills, timer, 0); // next timer call in xx milliseconds later
		angleGL += 45.0f;
	}
	
	if (refreshTimes == 8 && blockID < 11) {
		blockID++;
		angleGL = 0.0f;
		refreshTimes = 0;
		glutTimerFunc(refreshMills, timer, 0); // next timer call in xx milliseconds later
	}
	
	if (refreshTimes < 8)
		refreshTimes++;
	
	glutPostRedisplay();                   // Post re-draw request to activate display function
}

/* main() function */
int main(int argc, char* argv[]){
   	createImposter();
	glutInit(&argc,argv);                         //  Initialize GLUT and process user parameters
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  //  Request double buffered true color window with Z-buffer
	glutInitWindowSize(1200, 150);                 // Set the window's initial width & height
	//glutInitWindowSize(1200, 400); 
	glutInitWindowPosition(0, 0);               // Position the window's initial top-left corner
	winMain = glutCreateWindow("3D Map");   // Create window

	ReadInFile(12); // Read jpeg images to pixel buffer
	initGL();
	glutDisplayFunc(mainDisplay);
    //glutPostRedisplay(); 	// OpenGL initialization
	
	glutTimerFunc(0, timer, 0);     // First timer call immediately 
	
	glutMainLoop();
 
    return 0;                       
}