///
/// \note 粒子系统
///

#include <iostream>

#include <GL/glut.h>
#include <GL/gl.h>
#include <unistd.h> //延时
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

#define TAB 9
#define RETURN 13 //回车
#define ESCAPE 27 //esc键ASCII码值
#define SPACE 32
#define PLUS 43
#define MINUS 45
#define N2 50 //
#define N4 52 //
#define N6 54 //
#define N8 56
#define PAGE_UP 73
#define PAGE_DOWN 81
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77

#define MAX_PARTICLES 1000 //粒子数量

float slowdown = 2.0f; //减速
float xspeed;          //
float yspeed;          //
float zoom = -40.f;    //

int window; //glut窗口标识号

GLuint loop;       //通用循环变量
GLuint col;        //当前颜色选择

typedef struct // Create A Structure For Particle
{
    bool active; // Active (Yes/No)
    float life;  // Particle Life
    float fade;  // Fade Speed
    float r;     // Red Value
    float g;     // Green Value
    float b;     // Blue Value
    float x;     // X Position
    float y;     // Y Position
    float z;     // Z Position
    float xi;    // X Direction
    float yi;    // Y Direction
    float zi;    // Z Direction
    float xg;    // X Gravity
    float yg;    // Y Gravity
    float zg;    // Z Gravity
} particles;     // Particles Structure

particles particle[MAX_PARTICLES]; //粒子数组

static GLfloat colors[12][3] = // Rainbow Of Colors
    {
        {1.0f, 0.5f, 0.5f}, 
        {1.0f, 0.75f, 0.5f}, 
        {1.0f, 1.0f, 0.5f}, 
        {0.75f, 1.0f, 0.5f}, 
        {0.5f, 1.0f, 0.5f}, 
        {0.5f, 1.0f, 0.75f}, 
        {0.5f, 1.0f, 1.0f}, 
        {0.5f, 0.75f, 1.0f}, 
        {0.5f, 0.5f, 1.0f}, 
        {0.75f, 0.5f, 1.0f}, 
        {1.0f, 0.5f, 1.0f}, 
        {1.0f, 0.5f, 0.75f}};

//初始化函数，设置所有初始化值
void initGL(int width, int height)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //将界面背景清空为红绿蓝alpha值对应的颜色
    glClearDepth(1.0);                    //设置清空深度缓冲区（depth buffer）

    glShadeModel(GL_SMOOTH); //栅格化模式为平滑，只有平滑和两种模式

    glMatrixMode(GL_PROJECTION); //矩阵模式为投影模式
    glLoadIdentity();            //重置投影矩阵

    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f); //计算窗口的显示比例

    glMatrixMode(GL_MODELVIEW); //

    //设置混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);                 //设置混合功能
    glEnable(GL_BLEND);                                //使用混合
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective Calculations
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);           // Really Nice Point Smoothing

    //设置粒子
    for (loop = 0; loop < MAX_PARTICLES; loop++) // Initials All The Textures
    {
        particle[loop].active = true;                                    // Make All The Particles Active
        particle[loop].life = 1.0f;                                      // Give All The Particles Full Life
        particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;    // Random Fade Speed
        particle[loop].r = colors[(loop + 1) / (MAX_PARTICLES / 12)][0]; // Select Red Rainbow Color
        particle[loop].g = colors[(loop + 1) / (MAX_PARTICLES / 12)][1]; // Select Green Rainbow Color
        particle[loop].b = colors[(loop + 1) / (MAX_PARTICLES / 12)][2]; // Select Blue Rainbow Color
        particle[loop].xi = float((rand() % 50) - 26.0f) * 10.0f;        // Random Speed On X Axis
        particle[loop].yi = float((rand() % 50) - 25.0f) * 10.0f;        // Random Speed On Y Axis
        particle[loop].zi = float((rand() % 50) - 25.0f) * 10.0f;        // Random Speed On Z Axis
        particle[loop].xg = 0.0f;                                        // Set Horizontal Pull To Zero
        particle[loop].yg = -0.8f;                                       // Set Vertical Pull Downward
        particle[loop].zg = 0.0f;                                        // Set Pull On Z Axis To Zero
    }
}

//OpenGL的主要绘图函数，绘制什么显示什么。
void displayGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清空界面和depth缓冲区
    glLoadIdentity();

    for (loop = 0; loop < MAX_PARTICLES; loop++) // Loop Through All The Particles
    {
        if (particle[loop].active) // If The Particle Is Active
        {
            float x = particle[loop].x;        // Grab Our Particle X Position
            float y = particle[loop].y;        // Grab Our Particle Y Position
            float z = particle[loop].z + zoom; // Particle Z Pos + Zoom

            // Draw The Particle Using Our RGB Values, Fade The Particle Based On It's Life
            glColor4f(particle[loop].r, particle[loop].g, particle[loop].b, particle[loop].life);

            glBegin(GL_TRIANGLE_STRIP); // Build Quad From A Triangle Strip
            glTexCoord2d(1, 1);
            glVertex3f(x + 0.5f, y + 0.5f, z); // Top Right
            glTexCoord2d(0, 1);
            glVertex3f(x - 0.5f, y + 0.5f, z); // Top Left
            glTexCoord2d(1, 0);
            glVertex3f(x + 0.5f, y - 0.5f, z); // Bottom Right
            glTexCoord2d(0, 0);
            glVertex3f(x - 0.5f, y - 0.5f, z); // Bottom Left
            glEnd();                           // Done Building Triangle Strip

            particle[loop].x += particle[loop].xi / (slowdown * 1000); // Move On The X Axis By X Speed
            particle[loop].y += particle[loop].yi / (slowdown * 1000); // Move On The Y Axis By Y Speed
            particle[loop].z += particle[loop].zi / (slowdown * 1000); // Move On The Z Axis By Z Speed

            particle[loop].xi += particle[loop].xg;     // Take Pull On X Axis Into Account
            particle[loop].yi += particle[loop].yg;     // Take Pull On Y Axis Into Account
            particle[loop].zi += particle[loop].zg;     // Take Pull On Z Axis Into Account
            particle[loop].life -= particle[loop].fade; // Reduce Particles Life By 'Fade'

            if (particle[loop].life < 0.0f) // If Particle Is Burned Out
            {
                particle[loop].life = 1.0f;                                   // Give It New Life
                particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f; // Random Fade Value
                particle[loop].x = 0.0f;                                      // Center On X Axis
                particle[loop].y = 0.0f;                                      // Center On Y Axis
                particle[loop].z = 0.0f;                                      // Center On Z Axis
                particle[loop].xi = xspeed + float((rand() % 60) - 32.0f);    // X Axis Speed And Direction
                particle[loop].yi = yspeed + float((rand() % 60) - 30.0f);    // Y Axis Speed And Direction
                particle[loop].zi = float((rand() % 60) - 30.0f);             // Z Axis Speed And Direction
                particle[loop].r = colors[col][0];                            // Select Red From Color Table
                particle[loop].g = colors[col][1];                            // Select Green From Color Table
                particle[loop].b = colors[col][2];                            // Select Blue From Color Table
            }
        }
    }

    //双缓冲技术，交换缓冲区以显示刚刚绘制的图像
    glutSwapBuffers();
}

// 窗口大小变化回调函数
void reshapeGL(int w, int h)
{
    glViewport(0, 0, w, max(h, 1)); //重置当前窗口的视场和角度 防止出现长度为0的情况，尤其是下面还将h用作除数
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, static_cast<GLfloat>(w / h), 0.1f, 200.0f); //设置长宽视景
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//如果有按键按下调用此函数
void keyboard(unsigned char key, int x, int y)
{
    usleep(100); //稍微延时以免干扰

    switch (key)
    {
    case ESCAPE:
        glutDestroyWindow(window); //销毁窗口
        exit(0);                   //正常退出程序
        break;
    case UP_ARROW: //zoom in
        zoom += 0.1f;
        if (yspeed < 200){
            yspeed += 1.0f;
        }
        break;
    case DOWN_ARROW: //zoom out
        zoom -= 0.1f;
        if (yspeed > -200){
            yspeed -= 1.0f;
        }
        break;
    case LEFT_ARROW: //
        if (xspeed > -200){
            xspeed -= 1.0f;
        }
        break;
    case RIGHT_ARROW:
        if (xspeed < 200){
            xspeed += 1.0f;
        }
        break;
    case PLUS:
        if (slowdown > 0.0f){
            slowdown -= 0.01f;
        }
        break;
    case MINUS:
        if (slowdown < 4.0f){
            slowdown += 0.01f;
        }
        break;
    case SPACE:
        col++;
        if (col > 11)
            col = 0;
        break;
    case TAB:
        particle[loop].x = 0.0f;                                  // Center On X Axis
        particle[loop].y = 0.0f;                                  // Center On Y Axis
        particle[loop].z = 0.0f;                                  // Center On Z Axis
        particle[loop].xi = float((rand() % 50) - 26.0f) * 10.0f; // Random Speed On X Axis
        particle[loop].yi = float((rand() % 50) - 25.0f) * 10.0f; // Random Speed On Y Axis
        particle[loop].zi = float((rand() % 50) - 25.0f) * 10.0f; // Random Speed On Z Axis
        break;
    case N2: //6
        if (particle[loop].yg > -1.5f){
            particle[loop].yg -= 0.01f;
        }
        break;
    case N4:
        if (particle[loop].xg > -1.5f){
            particle[loop].xg -= 0.01f;
        }
        break;
    case N6:
        if (particle[loop].xg < 1.5f){
            particle[loop].xg += 0.01f;
        }
        break;
    case N8:
        if (particle[loop].yg < 1.5f){
            particle[loop].yg += 0.01f;
        }
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
    //初始化glut状态，并且接管所有传给程序的参数
    glutInit(&argc, argv);
    //设置显示模式：双缓冲、RGBA颜色、alpha支持、depth缓冲支持
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);

    // 初始化窗口
    glutInitWindowSize(640, 480);     //初始界面大小
    glutInitWindowPosition(100, 100); //界面初始位置，相对于屏幕左上角的像素值
    //创建界面，界面标题为argv[0]，即程序名称，也可以以字符串进行指定
    //同时获取创建的窗口ID号，销毁时使用
    window = glutCreateWindow(argv[0]);

    glutIdleFunc(&displayGL);    //即使没有任何事件，也重绘场景
    glutReshapeFunc(&reshapeGL); //注册调整窗口大小回调函数
    glutDisplayFunc(&displayGL); //注册主要绘图回调函数
    glutKeyboardFunc(&keyboard); //注册键盘回调函数

    initGL(640, 480); //初始化窗口

    glutMainLoop(); //开始事件处理引擎

    return 0;
}
