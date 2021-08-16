/*
正方体六面贴图

stb_image可以读取基于jpeg的格式、png、TGA、bmp、PSD、GIF、HDR、pic、pnm格式
*/

#include <iostream>

#include <GL/glut.h>
#include <GL/gl.h>
#include <unistd.h> //延时

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

#define ESCAPE 27 //esc键ASCII码值

int window; //glut窗口标识号

bool masking; //mask是否开启
bool scene;   //绘制哪一个图像

GLuint loop;       //变量
GLuint texture[5]; //存储一个纹理
GLfloat roll;      //roll纹理

//加载纹理
void loadGLTexture()
{
  int w, h, c; //图片的宽度高度和通道数
  unsigned char *data = nullptr;

  glGenTextures(5, &texture[0]);

  for (loop = 0; loop < 5; loop++)
  {
    data = nullptr;
    switch (loop)
    {
    case 0:
      data = stbi_load("logo.bmp", &w, &h, &c, 0);
      break;
    case 1:
      data = stbi_load("image1.bmp", &w, &h, &c, 0);
      break;
    case 2:
      data = stbi_load("mask1.bmp", &w, &h, &c, 0);
      break;
    case 3:
      data = stbi_load("image2.bmp", &w, &h, &c, 0);
      break;
    case 4:
      data = stbi_load("mask2.bmp", &w, &h, &c, 0);
      break;
    }
    glBindTexture(GL_TEXTURE_2D, texture[loop]); //绑定纹理
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
}

//初始化函数，设置所有初始化值
void initGL(int width, int height)
{
  loadGLTexture();         //加载纹理
  glEnable(GL_TEXTURE_2D); //使用二维纹理映射

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //将界面背景清空为红绿蓝alpha值对应的颜色
  glClearDepth(1.0);                    //设置清空深度缓冲区（depth buffer）
  glDepthFunc(GL_LESS);                 //设置深度测试的类型
  glEnable(GL_DEPTH_TEST);              //设置进行深度测试（depth test）
  glShadeModel(GL_SMOOTH);              //栅格化模式为平滑，只有平滑和两种模式

  //glMatrixMode(GL_PROJECTION);//矩阵模式为投影模式
  glLoadIdentity(); //重置投影矩阵

  gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f); //计算窗口的显示比例

  glMatrixMode(GL_MODELVIEW); //
}

//OpenGL的主要绘图函数，绘制什么显示什么。
void displayGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear The Screen And The Depth Buffer
  glLoadIdentity();                                   // Reset The Modelview Matrix
  glTranslatef(0.0f, 0.0f, -2.0f);                    // Move Into The Screen 5 Units

  glBindTexture(GL_TEXTURE_2D, texture[0]); // Select Our Logo Texture
  glBegin(GL_QUADS);                        // Start Drawing A Textured Quad
  glTexCoord2f(0.0f, -roll + 3.0f);
  glVertex3f(-1.1f, -1.1f, 0.0f); // Bottom Left
  glTexCoord2f(3.0f, -roll + 3.0f);
  glVertex3f(1.1f, -1.1f, 0.0f); // Bottom Right
  glTexCoord2f(3.0f, -roll + 0.0f);
  glVertex3f(1.1f, 1.1f, 0.0f); // Top Right
  glTexCoord2f(0.0f, -roll + 0.0f);
  glVertex3f(-1.1f, 1.1f, 0.0f); // Top Left
  glEnd();                       // Done Drawing The Quad

  glEnable(GL_BLEND);       // Enable Blending
  glDisable(GL_DEPTH_TEST); // Disable Depth Testing

  if (masking) // Is Masking Enabled?
  {
    glBlendFunc(GL_DST_COLOR, GL_ZERO); // Blend Screen Color With Zero (Black)
  }

  if (scene) // Are We Drawing The Second Scene?
  {
    glTranslatef(0.0f, 0.0f, -1.0f);         // Translate Into The Screen One Unit
    glRotatef(roll * 360, 0.0f, 0.0f, 1.0f); // Rotate On The Z Axis 360 Degrees.
    if (masking)                             // Is Masking On?
    {
      glBindTexture(GL_TEXTURE_2D, texture[3]); // Select The Second Mask Texture
      glBegin(GL_QUADS);                        // Start Drawing A Textured Quad
      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(-1.1f, -1.1f, 0.0f); // Bottom Left
      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(1.1f, -1.1f, 0.0f); // Bottom Right
      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(1.1f, 1.1f, 0.0f); // Top Right
      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(-1.1f, 1.1f, 0.0f); // Top Left
      glEnd();                       // Done Drawing The Quad
    }

    glBlendFunc(GL_ONE, GL_ONE);              // Copy Image 2 Color To The Screen
    glBindTexture(GL_TEXTURE_2D, texture[4]); // Select The Second Image Texture
    glBegin(GL_QUADS);                        // Start Drawing A Textured Quad
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.1f, -1.1f, 0.0f); // Bottom Left
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.1f, -1.1f, 0.0f); // Bottom Right
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.1f, 1.1f, 0.0f); // Top Right
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.1f, 1.1f, 0.0f); // Top Left
    glEnd();                       // Done Drawing The Quad
  }
  else // Otherwise
  {
    if (masking) // Is Masking On?
    {
      glBindTexture(GL_TEXTURE_2D, texture[1]); // Select The First Mask Texture
      glBegin(GL_QUADS);                        // Start Drawing A Textured Quad
      glTexCoord2f(roll + 0.0f, 0.0f);
      glVertex3f(-1.1f, -1.1f, 0.0f); // Bottom Left
      glTexCoord2f(roll + 4.0f, 0.0f);
      glVertex3f(1.1f, -1.1f, 0.0f); // Bottom Right
      glTexCoord2f(roll + 4.0f, 4.0f);
      glVertex3f(1.1f, 1.1f, 0.0f); // Top Right
      glTexCoord2f(roll + 0.0f, 4.0f);
      glVertex3f(-1.1f, 1.1f, 0.0f); // Top Left
      glEnd();                       // Done Drawing The Quad
    }

    glBlendFunc(GL_ONE, GL_ONE);              // Copy Image 1 Color To The Screen
    glBindTexture(GL_TEXTURE_2D, texture[2]); // Select The First Image Texture
    glBegin(GL_QUADS);                        // Start Drawing A Textured Quad
    glTexCoord2f(roll + 0.0f, 0.0f);
    glVertex3f(-1.1f, -1.1f, 0.0f); // Bottom Left
    glTexCoord2f(roll + 4.0f, 0.0f);
    glVertex3f(1.1f, -1.1f, 0.0f); // Bottom Right
    glTexCoord2f(roll + 4.0f, 4.0f);
    glVertex3f(1.1f, 1.1f, 0.0f); // Top Right
    glTexCoord2f(roll + 0.0f, 4.0f);
    glVertex3f(-1.1f, 1.1f, 0.0f); // Top Left
    glEnd();                       // Done Drawing The Quad
  }

  glEnable(GL_DEPTH_TEST); // Enable Depth Testing
  glDisable(GL_BLEND);     // Disable Blending

  roll += 0.002f;  // Increase Our Texture Roll Variable
  if (roll > 1.0f) // Is Roll Greater Than One
  {
    roll -= 1.0f; // Subtract 1 From Roll
  }

  // since this is double buffered, swap the buffers to display what just got drawn.
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
  case 32: //space
    scene = !scene;
    break;
  case 'M':
  case 'm':
    masking = !masking;
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
  glutInitWindowSize(500, 500);     //初始界面大小
  glutInitWindowPosition(100, 100); //界面初始位置，相对于屏幕左上角的像素值
  //创建界面，界面标题为argv[0]，即程序名称，也可以以字符串进行指定
  //同时获取创建的窗口ID号，销毁时使用
  window = glutCreateWindow(argv[0]);

  glutIdleFunc(&displayGL);    //即使没有任何事件，也重绘场景
  glutReshapeFunc(&reshapeGL); //注册调整窗口大小回调函数
  glutDisplayFunc(&displayGL); //注册主要绘图回调函数
  glutKeyboardFunc(&keyboard); //注册键盘回调函数

  initGL(500, 500); //初始化窗口

  glutMainLoop(); //开始事件处理引擎

  return 0;
}
