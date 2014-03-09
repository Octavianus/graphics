#include <iostream>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER

#endif

#ifdef __GNUC__
#include <unistd.h>
#endif

using namespace std;

// ���ǽ�ʹ�õĵ�������
GLuint texture[1];

// ��������
GLfloat g_u = 1.0f;
GLfloat g_v = 1.0f;

// ��ȡbmpͼƬ����
// ͼƬ������ ������ �� ����
struct Image
{
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct Image Image;


int ImageLoad(char *filename, Image *image)
{
    FILE *file;
    unsigned long size; // ͼƬ����
    unsigned long i;    // ����
    unsigned short int planes;
    unsigned short int bpp;
    char temp;            // bgr -rgb �任

    if ((file = fopen(filename, "rb")) == NULL)
    {
        printf("File Not Found: %s\n", filename);
        return 0;
    }

    // �����ļ���ͷ����׼����ȡ��Ⱥ͸߶� 
    // ������Լ�������������ȥ����bmpͼƬ�������Ϣ
    fseek(file, 18, SEEK_CUR);

    // ��ȡ���
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1)
    {
        printf("Error reading width from %s. \n", filename);
        return 0;
    }
    printf("Width of %s: %lu\n", filename, image->sizeX);
    
    // ��ȡ���
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1)
    {
        printf("Error reading height from %s. \n", filename);
        return 0;
    }
    printf("Height of %s: %lu\n", filename, image->sizeY);

    // ���㳤��(24bits��3bytesÿ������)
    size = image->sizeX * image->sizeY * 3;

    // ��ȡ
    if ((fread(&planes, 2, 1, file)) != 1)
    {
        printf("Error reading planes from %s. \n", filename);
        return 0;
    }

    if (planes != 1)
    {
        printf("Planes from %s is not 1: %u\n", filename, planes);
        return 0;
    }

    if ((i = fread(&bpp, 2, 1, file)) != 1)
    {
        printf("Error reading bpp from %s. \n", filename);
        return 0;
    }
    if (bpp != 24)
    {
        printf("Bpp from %s is not 24: %u\n", filename, bpp);
        return 0;
    }

    // �������µ�ͷ�ļ�����
    fseek(file, 24, SEEK_CUR);

    // ��ȡ����
    image->data = (char*) malloc (size);
    if (image->data == NULL)
    {
        printf("Error allocating memory for color-corrected image data");
        return 0;
    }

    if ((i = fread(image->data, size, 1, file)) != 1)
    {
        printf("Error reading image data from %s. \n", filename);
        return 0;
    }

    // ������ɫ bgr -> rgb
    for (i = 0; i < size; i += 3)
    {
        temp = image->data[i];
        image->data[i] = image->data[i+2];
        image->data[i+2] = temp;
    }

    // ���
    return 1;
}

// ��ȡ bitmaps ��ת��������
void LoadGLTextures()
{
    Image *image1;
    image1 = (Image*) malloc (sizeof(Image));
    if (image1 == NULL)
    {
        printf("Error allocating space for image");
        exit(0);
    }    

    if (!ImageLoad("4.bmp", image1))
        exit(1);
    // ��������
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]); // ��2D����
    // ��ͼƬ���ڵ�ǰ����ʱ��������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // ��ͼƬС�ڵ�ǰ����ʱ��������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image,
    // y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    // �������˽⣬��Ҫ������
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY,
            0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
}

//===========================OpenGL ����=========================
// ��ʼ��opengl��һЩ����
void init()
{
    // ��ȡһ������
    LoadGLTextures();
    glEnable(GL_TEXTURE_2D); // ����Ҫʹ��������Ҫ�����

    glClearColor(0.0f, 0.0f, 1.0f, 0.0f); // ��������ɫΪ��ɫ
    glClearDepth(1.0);                     
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
}

//------------------------------------
// ����ˢ��ʱ��
void timer(int p)
{
    glutPostRedisplay();
    glutTimerFunc(20, timer, 0);
}

//-----------------------------------
// ��ʾ����Ҫ���Ƶ�ģ��
void display()
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ɫ�������Ȼ���
 glLoadIdentity(); // ���þ���

 glTranslatef(0.0f, 0.0f, -6.0f);
 // ������ת�ĽǶȣ�����glRotatef ��1�������ǽǶȣ�2��4 ������ָ����ת��
 // ��������������y������ת��

 //-------------------------------------------------------------
 // ����һ������
 glTranslatef(-1.5f, 0.0f, 0.0f);
 glBegin(GL_TRIANGLES);
    // ������2����������ɣ�����һ�� ����һ�� 
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);

    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);

 glEnd();

 //-------------------------------------------------------------
 // �����������ƶ�����
 glTranslatef(3.5f, 0.0f, 0.0f);
 glBegin(GL_TRIANGLES);
    // ������2����������ɣ�����һ�� ����һ�� 
    glTexCoord2f(0.0f + g_u, 0.0f + g_v); glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f + g_u, 0.0f + g_v); glVertex3f( 1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f + g_u, 1.0f + g_v); glVertex3f(-1.0f, 1.0f, 0.0f);

    glTexCoord2f(0.0f + g_u, 1.0f + g_v); glVertex3f(-1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f + g_u, 0.0f + g_v); glVertex3f( 1.0f,-1.0f, 0.0f);
    glTexCoord2f(1.0f + g_u, 1.0f + g_v); glVertex3f( 1.0f, 1.0f, 0.0f);

 glEnd();
 g_u += 0.01f;
 g_v += 0.01f;

 glutSwapBuffers(); //˫����
}

//-----------------------------------
// �����ڱ仯��ʱ����������
void reshape(int w, int h)
{
    /* �ӿ�����
     * ����ǵ����ڷ�����С�仯��ʱ�����������ҿ����Լ�
     * �����´��ھͿ��Կ�����һ���ĵط���
     */
    glViewport(0, 0, (GLsizei) w, (GLsizei) h); // �����ӿ�
    glMatrixMode(GL_PROJECTION); // ���õ�ǰ����ΪͶӰ����
    glLoadIdentity();

    if (h == 0) h = 1; // ��ֹ��0���
    gluPerspective(45.0f,(GLfloat) w / (GLfloat)h , 0.5f, -1000.0f);

    glMatrixMode(GL_MODELVIEW); // ���õ�ǰ����Ϊģ�;���
    glLoadIdentity();
}

//-----------------------------------
//�����¼� 
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27: // �����¼��̵�esc����ʱ���˳�
            exit(0);
            break;
    }    
}

//------------------------------------
// �������¼���
void MouseEvent(int button, int state, int x, int y)
{
}

//-------------------------------------
// ����ƶ��¼�
void MotionMove(int x,int y)
{
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGLDemo");
    init();
    glutTimerFunc(20,timer,0); // ���ø���ʱ��
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(MouseEvent);
    glutMotionFunc(MotionMove);
    glutMainLoop();

    return 0;
}