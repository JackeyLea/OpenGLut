///
/// \note 对三维物体施加光照
///

#include <iostream>

#include <GL/glut.h>
#include <GL/gl.h>
#include <unistd.h>//延时

// 图片库
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

#define ESCAPE 27//esc键ASCII码值
#define PAGE_UP 73//PgUp按键ASCII码值
#define PAGE_DOWN 81//PgDn按键
#define UP_ARROW 72//向上箭头
#define DOWN_ARROW 80//向下箭头
#define LEFT_ARROW 75//向左箭头
#define RIGHT_ARROW 77//向右箭头

int window; //glut窗口标识号

bool light;//是否开启光照

GLfloat xrot,yrot;//x,y旋转角度
GLfloat xspeed,yspeed;//x,y旋转速度

GLfloat z=-5.0f;//在屏幕坐标轴的深度

GLfloat lightAmbient[]={0.5f,0.5f,0.5f,1.0f};
GLfloat lightDiffuse[]={1.0f,1.0f,1.0f,1.0f};

GLfloat lightPosition[]={0.0f,0.0f,2.0f,1.0f};

GLuint filter;//滤镜模式：最近、线性、插值

GLuint texture[3];//存储三个纹理

//加载纹理
void loadGLTexture(){
    int w,h,c;//图片的宽度高度和通道数

    unsigned char *data=stbi_load("crate.bmp",&w,&h,&c,0);

    //创建纹理
    glGenTextures(3,&texture[0]);

    //纹理1-低质量缩放
    glBindTexture(GL_TEXTURE_2D,texture[0]);//绑定纹理

    //纹理过滤
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
    //二位纹理，细节等级，RGB3组件，宽，高，边框，rgb数据，无符号整数，数据
    if(c==3){
        glTexImage2D(GL_TEXTURE_2D, 0, 3,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,data);
    }else if(c==4){
        glTexImage2D(GL_TEXTURE_2D, 0, 3,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
    }

    //纹理2-线性缩放
    glBindTexture(GL_TEXTURE_2D,texture[1]);//绑定纹理

    //纹理过滤
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    
    //二位纹理，细节等级，RGB3组件，宽，高，边框，rgb数据，无符号整数，数据
    if(c==3){
        glTexImage2D(GL_TEXTURE_2D, 0, 3,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,data);
    }else if(c==4){
        glTexImage2D(GL_TEXTURE_2D, 0, 3,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
    }

    //纹理3-映射缩放
    glBindTexture(GL_TEXTURE_2D,texture[0]);//绑定纹理

    //纹理过滤
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
    
    //二位纹理，细节等级，RGB3组件，宽，高，边框，rgb数据，无符号整数，数据
    if(c==3){
        glTexImage2D(GL_TEXTURE_2D, 0, 3,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,data);
    }else if(c==4){
        glTexImage2D(GL_TEXTURE_2D, 0, 3,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
    }

    //二位纹理，RGB3颜色，宽，高，Rgb边框值，rgb数据，无符号整数，数据
    if(c==3){
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3,w,h,GL_RGB,GL_UNSIGNED_BYTE,data);
    }else if(c==4){
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3,w,h,GL_RGBA,GL_UNSIGNED_BYTE,data);
    }
}

//初始化函数，设置所有初始化值
void initGL(int width, int height)
{
    loadGLTexture();//加载纹理
    glEnable(GL_TEXTURE_2D);//使用二维纹理映射

    glClearColor(1.0f, 1.0f, 0.0f, 0.0f); //将界面背景清空为红绿蓝alpha值对应的颜色
    glClearDepth(1.0);//设置清空深度缓冲区（depth buffer）
    glDepthFunc(GL_LESS);//设置深度测试的类型
    glEnable(GL_DEPTH_TEST);//设置进行深度测试（depth test）
    glShadeModel(GL_SMOOTH);//栅格化模式为平滑，只有平滑和两种模式

    glMatrixMode(GL_PROJECTION);//矩阵模式为投影模式
    glLoadIdentity();//重置投影矩阵

    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);//计算窗口的显示比例

    glMatrixMode(GL_MODELVIEW);//

    //设置光照模式
    glLightfv(GL_LIGHT1,GL_AMBIENT,lightAmbient);//添加光照ambient
    glLightfv(GL_LIGHT1,GL_DIFFUSE,lightDiffuse);//添加光照diffuse
    glLightfv(GL_LIGHT1,GL_POSITION,lightPosition);//设置光照位置
    glEnable(GL_LIGHT1);//启用光照1模式
}

//OpenGL的主要绘图函数，绘制什么显示什么。
void displayGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清空界面和depth缓冲区
    glLoadIdentity();//调整视场
    glTranslatef(0.0f,0.0f,z);//平移三角形位置

    //此部分是绘制图像
    glRotatef(xrot,1.0f,0.0f,0.0f);//绕x轴旋转
    glRotatef(yrot,0.0f,1.0f,0.0f);//绕y轴旋转

    glBindTexture(GL_TEXTURE_2D,texture[filter]);//选择纹理
    
    glBegin(GL_QUADS);//开始绘制
    // Front Face (note that the texture's corners have to match the quad's corners)
    glNormal3f(0.0f,0.0f,1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
    
    // Back Face
    glNormal3f(0.0f,0.0f,-1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
	
    // Top Face
    glNormal3f(0.0f,1.0f,0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    
    // Bottom Face   
    glNormal3f(0.0f,-1.0f,0.0f);    
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    
    // Right face
    glNormal3f(1.0f,0.0f,0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    
    // Left Face
    glNormal3f(-1.0f,0.0f,0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glEnd();//正方形绘制结束

    xrot+=xspeed;
    yrot+=yspeed;

    //glFlush(); //刷新缓冲区
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

    switch(key){
        case ESCAPE:
        glutDestroyWindow(window); //销毁窗口
        exit(0);//正常退出程序
        break;
    case 76:
    case 108://调整光照
        std::cout<<"Light is: "<<light<<std::endl;
        light = light?0:1;
        std::cout<<"Light is: "<<light<<" now"<<std::endl;
        if(!light){
            glDisable(GL_LIGHTING);
        }else{
            glEnable(GL_LIGHTING);
        }
        break;
    case 70:
    case 102:
        std::cout<<"Filter is: "<<filter<<std::endl;
        filter+=1;
        if(filter>2){
            filter=0;
        }
        std::cout<<"Filter is: "<<filter<<" now."<<std::endl;
        break;
    default:
        break;
    }
}

void specialKeyPressed(int key,int x,int y){
    usleep(100);

    switch(key){
    case GLUT_KEY_PAGE_UP:
        z-=0.02f;
        break;
    case GLUT_KEY_PAGE_DOWN:
        z+=0.02f;
        break;
    case GLUT_KEY_UP:
        xspeed-=0.01f;
        break;
    case GLUT_KEY_DOWN:
        yspeed+=0.01f;
        break;
    case GLUT_KEY_LEFT:
        yspeed-=0.01f;
        break;
    case GLUT_KEY_RIGHT:
        yspeed+=0.01f;
        break;
    default:
        break;
    }
}

int main(int argc,char **argv)
{
    //初始化glut状态，并且接管所有传给程序的参数
    glutInit(&argc, argv);
    //设置显示模式：双缓冲、RGBA颜色、alpha支持、depth缓冲支持
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  

    // 初始化窗口
    glutInitWindowSize(500, 500);       //初始界面大小
    glutInitWindowPosition(100, 100);   //界面初始位置，相对于屏幕左上角的像素值
    //创建界面，界面标题为argv[0]，即程序名称，也可以以字符串进行指定
    //同时获取创建的窗口ID号，销毁时使用
    window = glutCreateWindow(argv[0]); 

    glutIdleFunc(&displayGL);//即使没有任何事件，也重绘场景
    glutReshapeFunc(&reshapeGL);//注册调整窗口大小回调函数
    glutDisplayFunc(&displayGL);//注册主要绘图回调函数
    glutKeyboardFunc(&keyboard);//注册键盘回调函数
    glutSpecialFunc(&specialKeyPressed);//特殊键盘按键回调函数

    initGL(500,500);//初始化窗口

    glutMainLoop();//开始事件处理引擎

    return 0;
}
