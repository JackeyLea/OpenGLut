///
/// \note 三维物体移动
///

#include <iostream>

#include <GL/glut.h>
#include <GL/gl.h>
#include <unistd.h> //延时
#include <iostream>

// 图片库
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

#define STAR_NUM 50 //粒子数量

#define ESCAPE 27      //esc键ASCII码值
#define PAGE_UP 73     //PgUp按键ASCII码值
#define PAGE_DOWN 81   //PgDn按键
#define UP_ARROW 72    //向上箭头
#define DOWN_ARROW 80  //向下箭头
#define LEFT_ARROW 75  //向左箭头
#define RIGHT_ARROW 77 //向右箭头

int window; //glut窗口标识号

int twinkle = 0; //是否闪烁

typedef struct
{
    int r, g, b;   //粒子颜色
    GLfloat dist;  //粒子距中心距离
    GLfloat angle; //粒子当前角度
} stars;

stars star[STAR_NUM]; //粒子数组

GLfloat zoom = -15.0f; //观察距离
GLfloat tilt = 90.0f;  //倾斜视场
GLfloat spin;          //旋转粒子

GLuint loop;       //通用循环变量
GLuint texture[1]; //一个纹理

//加载纹理
void loadGLTexture()
{
    int w, h, c; //图片的宽度高度和通道数

    unsigned char *data = stbi_load("star.bmp", &w, &h, &c, 0);

    //创建纹理
    glGenTextures(3, &texture[0]);

    //纹理1-低质量缩放
    glBindTexture(GL_TEXTURE_2D, texture[0]); //绑定纹理

    //纹理过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //二位纹理，细节等级，RGB3组件，宽，高，边框，rgb数据，无符号整数，数据
    if (c == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (c == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
}

//初始化函数，设置所有初始化值
void initGL(int width, int height)
{
    loadGLTexture();         //加载纹理
    glEnable(GL_TEXTURE_2D); //使用二维纹理映射

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //将界面背景清空为红绿蓝alpha值对应的颜色
    glClearDepth(1.0);                    //设置清空深度缓冲区（depth buffer）

    glShadeModel(GL_SMOOTH); //栅格化模式为平滑，只有平滑和两种模式

    glMatrixMode(GL_PROJECTION); //矩阵模式为投影模式
    glLoadIdentity();            //重置投影矩阵

    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f); //计算窗口的显示比例

    glMatrixMode(GL_MODELVIEW); //

    //设置混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); //设置混合功能
    glEnable(GL_BLEND);                //使用混合

    //设置粒子
    for (loop = 0; loop < STAR_NUM; loop++)
    {
        star[loop].angle = 0.0f; //初始时没有旋转

        star[loop].dist = loop * 1.0f / STAR_NUM * 5.0f; //计算到中心的距离
        star[loop].r = rand() % 256;
        star[loop].g = rand() % 256;
        star[loop].b = rand() % 256;
    }
}

//OpenGL的主要绘图函数，绘制什么显示什么。
void displayGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清空界面和depth缓冲区

    glBindTexture(GL_TEXTURE_2D, texture[0]); //选择纹理

    for (loop = 0; loop < STAR_NUM; loop++)
    {                                      // loop through all the stars.
        glLoadIdentity();                  // reset the view before we draw each star.
        glTranslatef(0.0f, 0.0f, zoom);    // zoom into the screen.
        glRotatef(tilt, 1.0f, 0.0f, 0.0f); // tilt the view.

        glRotatef(star[loop].angle, 0.0f, 1.0f, 0.0f); // rotate to the current star's angle.
        glTranslatef(star[loop].dist, 0.0f, 0.0f);     // move forward on the X plane (the star's x plane).

        glRotatef(-star[loop].angle, 0.0f, 1.0f, 0.0f); // cancel the current star's angle.
        glRotatef(-tilt, 1.0f, 0.0f, 0.0f);             // cancel the screen tilt.

        if (twinkle)
        { // twinkling stars enabled ... draw an additional star.
            // assign a color using bytes
            glColor4ub(star[STAR_NUM - loop].r, star[STAR_NUM - loop].g, star[STAR_NUM - loop].b, 255);

            glBegin(GL_QUADS); // begin drawing the textured quad.
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-1.0f, -1.0f, 0.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(1.0f, -1.0f, 0.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, 0.0f);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-1.0f, 1.0f, 0.0f);
            glEnd(); // done drawing the textured quad.
        }

        // main star
        glRotatef(spin, 0.0f, 0.0f, 1.0f); // rotate the star on the z axis.

        // Assign A Color Using Bytes
        glColor4ub(star[loop].r, star[loop].g, star[loop].b, 255);
        glBegin(GL_QUADS); // Begin Drawing The Textured Quad
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glEnd(); // Done Drawing The Textured Quad

        spin += 0.01f;                                     // used to spin the stars.
        star[loop].angle += loop * 1.0f / STAR_NUM * 1.0f; // change star angle.
        star[loop].dist -= 0.01f;                          // bring back to center.

        if (star[loop].dist < 0.0f)
        {                                // star hit the center
            star[loop].dist += 5.0f;     // move 5 units from the center.
            star[loop].r = rand() % 256; // new red color.
            star[loop].g = rand() % 256; // new green color.
            star[loop].b = rand() % 256; // new blue color.
        }
    }

    //双缓冲技术，交换缓冲区以显示刚刚绘制的图像
    glutSwapBuffers();
}

// 窗口大小变化回调函数
void reshapeGL(int w, int h)
{
    if (h == 0)
    { //防止出现长度为0的情况，尤其是下面还将h用作除数
        h = 1;
    }
    glViewport(0, 0, w, h); //重置当前窗口的视场和角度
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, static_cast<GLfloat>(w / h), 0.1f, 100.0f); //设置长宽视景
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
    case 84:
    case 116: //是否闪烁
        std::cout << "Twinkle is: " << twinkle << std::endl;
        twinkle = twinkle ? 0 : 1;
        std::cout << "Twinkle is: " << twinkle << " now" << std::endl;
        break;
    default:
        break;
    }
}

void specialKeyPressed(int key, int x, int y)
{
    usleep(100);

    switch (key)
    {
    case GLUT_KEY_PAGE_UP:
        zoom -= 0.2f;
        break;
    case GLUT_KEY_PAGE_DOWN:
        zoom += 0.2f;
        break;
    case GLUT_KEY_UP:
        tilt -= 0.5f;
        break;
    case GLUT_KEY_DOWN:
        tilt += 0.5f;
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

    glutIdleFunc(&displayGL);            //即使没有任何事件，也重绘场景
    glutReshapeFunc(&reshapeGL);         //注册调整窗口大小回调函数
    glutDisplayFunc(&displayGL);         //注册主要绘图回调函数
    glutKeyboardFunc(&keyboard);         //注册键盘回调函数
    glutSpecialFunc(&specialKeyPressed); //特殊键盘按键回调函数

    initGL(640, 480); //初始化窗口

    glutMainLoop(); //开始事件处理引擎

    return 0;
}
