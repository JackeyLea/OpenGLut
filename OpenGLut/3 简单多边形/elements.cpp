///
/// \note 在空白界面的基础上绘制
///

#include <iostream>

#include <GL/glut.h>
#include <GL/gl.h>
#include <unistd.h>//延时
#include <math.h>

using namespace std;

#define ESCAPE 27//esc键ASCII码值

int window; //glut窗口标识号

int n=40;
float Pi=3.1415926;
float R=1.0;
float factor=0.1;

//初始化函数，设置所有初始化值
void initGL(int width, int height)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //将界面背景清空为红绿蓝alpha值对应的颜色
    glClearDepth(1.0);//设置清空深度缓冲区（depth buffer）
    glDepthFunc(GL_LESS);//设置深度测试的类型
    glEnable(GL_DEPTH_TEST);//设置进行深度测试（depth test）
    glShadeModel(GL_SMOOTH);//栅格化模式为平滑，只有平滑和两种模式

    glMatrixMode(GL_PROJECTION);//矩阵模式为投影模式
    glLoadIdentity();//重置投影矩阵

    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);//计算窗口的显示比例

    glMatrixMode(GL_MODELVIEW);//
}

//OpenGL的主要绘图函数，绘制什么显示什么。
void displayGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清空界面和depth缓冲区
    glLoadIdentity();//调整视场
    glTranslatef(0.0f,0.f,-6.0f);
    //此部分是绘制图像

    // glPointSize(5.0f);//点的大小
    // glBegin(GL_POINTS);
    // glVertex2f(0.5f, 0.5f);
    // glEnd();

    // glEnable(GL_LINE_STIPPLE);//启动虚线模式
    // glLineStipple(2, 0x0F0F); //0000111100001111
    // glLineWidth(10.0f);
    // glBegin(GL_LINES);
    // glVertex2f(0.0f, 0.0f);
    // glVertex2f(0.5f, 0.5f);
    // glEnd();
    // glDisable(GL_LINE_STIPPLE);//关闭虚线模式

    // glBegin(GL_POLYGON); //画正n边形
    // for (int i = 0; i<n; ++i)
    //     glVertex2f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i));
    // glEnd();

    // glBegin(GL_TRIANGLES);
    // glVertex3f(-0.5,0,0);
    // glVertex3f(0,1,0);
    // glVertex3f(0.5,0,0);
    // glEnd();

    // glBegin(GL_QUADS);
    // glVertex3f(-0.5,0.5,0);
    // glVertex3f(0.5,0.5,0);
    // glVertex3f(0.5,-0.5,0);
    // glVertex3f(-0.5,-0.5,0);
    // glEnd();

    // GLfloat a = 1 / (2 - 2 * cos(72 * Pi / 180));
    // GLfloat bx = a * cos(18 * Pi / 180);
    // GLfloat by = a * sin(18 * Pi / 180);
    // GLfloat cy = -a * cos(18 * Pi / 180);
    // GLfloat
    //     PointA[2] = { 0, a },
    //     PointB[2] = { bx, by },
    //     PointC[2] = { 0.5, cy },
    //     PointD[2] = { -0.5, cy },
    //     PointE[2] = { -bx, by };
    // glClear(GL_COLOR_BUFFER_BIT);
    // glBegin(GL_LINE_LOOP);
    // glVertex2fv(PointA);
    // glVertex2fv(PointC);
    // glVertex2fv(PointE);
    // glVertex2fv(PointB);
    // glVertex2fv(PointD);
    // glEnd();

    GLfloat x;
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINES);
    glVertex2f(-1.0f, 0.0f);// 画x轴
    glVertex2f(1.0f, 0.0f);
    glVertex2f(0.0f, -1.0f);// 画y轴
    glVertex2f(0.0f, 1.0f);
    glEnd();
    glBegin(GL_LINE_STRIP);
    for (x = -1.0f / factor; x<1.0f / factor; x += 0.01f)
    {
        glVertex2f(x*factor, sin(x)*factor);
    }
    glEnd();
    glFlush();

    glFlush(); //刷新缓冲区
    //双缓冲技术，交换缓冲区以显示刚刚绘制的图像
    glutSwapBuffers();
}

// 窗口大小变化回调函数
void reshapeGL(int w, int h)
{
    if(h==0){//防止出现长度为0的情况，尤其是下面还将h用作除数
        h=1;
    }
    glViewport(0, 0, w, h);//重置当前窗口的视场和角度
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, static_cast<GLfloat>(w/h),0.1f,100.0f); //设置长宽视景
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//如果有按键按下调用此函数
void keyboard(unsigned char key, int x, int y)
{
    usleep(100);//稍微延时以免干扰

    if (key == ESCAPE)//如果是escape键
    {
        glutDestroyWindow(window); //销毁窗口
        exit(0);//正常退出程序
    }
}

int main(int argc,char **argv)
{
    //初始化glut状态，并且接管所有传给程序的参数
    glutInit(&argc, argv);
    //设置显示模式：双缓冲、RGBA颜色、alpha支持、depth缓冲支持
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  

    // 初始化窗口
    glutInitWindowSize(640, 480);       //初始界面大小
    glutInitWindowPosition(100, 100);   //界面初始位置，相对于屏幕左上角的像素值
    //创建界面，界面标题为argv[0]，即程序名称，也可以以字符串进行指定
    //同时获取创建的窗口ID号，销毁时使用
    window = glutCreateWindow(argv[0]); 

    glutIdleFunc(&displayGL);//即使没有任何事件，也重绘场景
    glutReshapeFunc(&reshapeGL);//注册调整窗口大小回调函数
    glutDisplayFunc(&displayGL);//注册主要绘图回调函数
    glutKeyboardFunc(&keyboard);//注册键盘回调函数

    initGL(640,480);//初始化窗口

    glutMainLoop();//开始事件处理引擎

    return 0;
}
