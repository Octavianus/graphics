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

// 我们将使用的到的纹理
GLuint texture[1];

// 纹理坐标
GLfloat g_u = 1.0f;
GLfloat g_v = 1.0f;

// 读取bmp图片部分
// 图片的类型 包含宽 高 数据
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
    unsigned long size; // 图片长度
    unsigned long i;    // 计数
    unsigned short int planes;
    unsigned short int bpp;
    char temp;            // bgr -rgb 变换

    if ((file = fopen(filename, "rb")) == NULL)
    {
        printf("File Not Found: %s\n", filename);
        return 0;
    }

    // 跳过文件的头部，准备读取宽度和高度 
    // 如果想自己具体的情况可以去看看bmp图片的相关信息
    fseek(file, 18, SEEK_CUR);

    // 读取宽度
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1)
    {
        printf("Error reading width from %s. \n", filename);
        return 0;
    }
    printf("Width of %s: %lu\n", filename, image->sizeX);
    
    // 读取宽度
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1)
    {
        printf("Error reading height from %s. \n", filename);
        return 0;
    }
    printf("Height of %s: %lu\n", filename, image->sizeY);

    // 计算长度(24bits或3bytes每个像素)
    size = image->sizeX * image->sizeY * 3;

    // 读取
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

    // 跳过余下的头文件数据
    fseek(file, 24, SEEK_CUR);

    // 读取数据
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

    // 交换颜色 bgr -> rgb
    for (i = 0; i < size; i += 3)
    {
        temp = image->data[i];
        image->data[i] = image->data[i+2];
        image->data[i+2] = temp;
    }

    // 完成
    return 1;
}

// 读取 bitmaps 并转化成纹理
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
    // 创建纹理
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]); // 绑定2D纹理
    // 当图片大于当前纹理时线性缩放
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 当图片小于当前纹理时线性缩放
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image,
    // y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    // 具体想了解，就要在网上
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY,
            0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
}

//===========================OpenGL 部分=========================
// 初始化opengl的一些参数
void init()
{
    // 读取一张纹理
    LoadGLTextures();
    glEnable(GL_TEXTURE_2D); // 我们要使用纹理需要打开这个

    glClearColor(0.0f, 0.0f, 1.0f, 0.0f); // 清理背景颜色为蓝色
    glClearDepth(1.0);                     
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
}

//------------------------------------
// 设置刷新时间
void timer(int p)
{
    glutPostRedisplay();
    glutTimerFunc(20, timer, 0);
}

//-----------------------------------
// 显示我们要绘制的模型
void display()
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清理颜色缓存和深度缓存
 glLoadIdentity(); // 重置矩阵

 glTranslatef(0.0f, 0.0f, -6.0f);
 // 设置旋转的角度，这里glRotatef 第1个参数是角度，2～4 参数是指定旋转轴
 // 这里我们设置于y轴作旋转轴

 //-------------------------------------------------------------
 // 绘制一个矩形
 glTranslatef(-1.5f, 0.0f, 0.0f);
 glBegin(GL_TRIANGLES);
    // 矩形由2个三角形组成，左下一个 右上一个 
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);

    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);

 glEnd();

 //-------------------------------------------------------------
 // 让纹理坐标移动起来
 glTranslatef(3.5f, 0.0f, 0.0f);
 glBegin(GL_TRIANGLES);
    // 矩形由2个三角形组成，左下一个 右上一个 
    glTexCoord2f(0.0f + g_u, 0.0f + g_v); glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f + g_u, 0.0f + g_v); glVertex3f( 1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f + g_u, 1.0f + g_v); glVertex3f(-1.0f, 1.0f, 0.0f);

    glTexCoord2f(0.0f + g_u, 1.0f + g_v); glVertex3f(-1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f + g_u, 0.0f + g_v); glVertex3f( 1.0f,-1.0f, 0.0f);
    glTexCoord2f(1.0f + g_u, 1.0f + g_v); glVertex3f( 1.0f, 1.0f, 0.0f);

 glEnd();
 g_u += 0.01f;
 g_v += 0.01f;

 glutSwapBuffers(); //双缓冲
}

//-----------------------------------
// 当窗口变化的时候会调用这里
void reshape(int w, int h)
{
    /* 视口设置
     * 这个是当窗口发生大小变化的时候会调用这里，大家可以自己
     * 拉伸下窗口就可以看到不一样的地方了
     */
    glViewport(0, 0, (GLsizei) w, (GLsizei) h); // 设置视口
    glMatrixMode(GL_PROJECTION); // 设置当前矩阵为投影矩阵
    glLoadIdentity();

    if (h == 0) h = 1; // 防止除0情况
    gluPerspective(45.0f,(GLfloat) w / (GLfloat)h , 0.5f, -1000.0f);

    glMatrixMode(GL_MODELVIEW); // 设置当前矩阵为模型矩阵
    glLoadIdentity();
}

//-----------------------------------
//键盘事件 
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27: // 当按下键盘的esc键的时候退出
            exit(0);
            break;
    }    
}

//------------------------------------
// 鼠标鼠标事件·
void MouseEvent(int button, int state, int x, int y)
{
}

//-------------------------------------
// 鼠标移动事件
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
    glutTimerFunc(20,timer,0); // 设置更新时间
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(MouseEvent);
    glutMotionFunc(MotionMove);
    glutMainLoop();

    return 0;
}