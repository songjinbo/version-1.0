// dialog_opengl.cpp : 实现文件
//

#include "stdafx.h"
#include "Dialog.h"
#include "dialog_opengl.h"
#include "afxdialogex.h"
#include <math.h>
#include <vector>
#include <gl/GLAUX.H>
// dialog_opengl 对话框

IMPLEMENT_DYNAMIC(dialog_opengl, CDialog)

void drawSence();

dialog_opengl::dialog_opengl(CWnd* pParent /*=NULL*/)
	: CDialog(dialog_opengl::IDD, pParent)
{
	//drawSence();
}

dialog_opengl::~dialog_opengl()
{
}

void dialog_opengl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(dialog_opengl, CDialog)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_KEYUP()
	//ON_MESSAGE(WM_KEYCONTROL,KeyControl)
END_MESSAGE_MAP()

/***********************************/
/*************实际程序**************/
using namespace std;

typedef enum loca{
	x = 0,
	y,
	z
}loca;
typedef struct qian{
	float location[6];
}coordinate;

extern vector<double> voxel_x;//与线程PathPlan和三维显示的接口
extern vector<double> voxel_y;
extern vector<double> voxel_z;

extern vector<double> path_x; //3DAStar程序的输出，三维显示模块的输入
extern vector<double> path_y;
extern vector<double> path_z;

coordinate coordi[1300];
int coordiNum = 1200;
const float piover180 = 0.0174532925f;
float tranlaX = 0;
float tranlaY = 0;
float moveX = 0;
float moveY = 0;

float xpos = 0;
float zpos = 15;
float angle = 0;
float lookY = 0;
float lookleft = 0;
float lookforward = 15;

void drawLine();
void drawStartAndEnd(GLfloat x, GLfloat y, GLfloat z);
void drawUnitAxes(GLfloat x, GLfloat y, GLfloat z);

//主函数drawscene()
void drawSence()//本程序用来画三维地图
{
	//readCoordinate();
	while (1){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
		glClearDepth(1.0);							// Enables Clearing Of The Depth Buffer
		glDepthFunc(GL_LESS);						// The Type Of Depth Test To Do
		glEnable(GL_DEPTH_TEST);					// Enables Depth Testing
		glShadeModel(GL_SMOOTH);					// Enables Smooth Color Shading

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();							// Reset The Projection Matrix

		//gluPerspective(45.0f, 2.0, 0.1f, 100.0f);	// Calculate The Aspect Ratio Of The Window
		gluPerspective(85.0f, 2.0f, 1.0f, 500.0f);	// Calculate The Aspect Ratio Of The Window

		glMatrixMode(GL_MODELVIEW);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer

		//for循环用来绘制三维地图
		for (vector<double>::iterator it_x = voxel_x.begin(), it_y = voxel_y.begin(), it_z = voxel_z.begin();
			it_x != voxel_x.end(); it_x++, it_y++, it_z++)
		{
			glLoadIdentity();								// Reset The View
			gluLookAt(lookleft, 0, lookforward, tranlaX, tranlaY, 0, lookleft, 4.0, lookforward);
			drawUnitAxes(*it_x, *it_y, *it_z);							// Move Into The Screen And Left
		}

		drawLine(); //绘制最优路径
		//绘制最优路径上的端点
		drawStartAndEnd(*path_x.begin(), *path_y.begin(), *path_z.begin());
		drawStartAndEnd(*(path_x.end()-1), *(path_y.end()-1), *(path_z.end()-1));

		glPopMatrix();
		SwapBuffers(wglGetCurrentDC());

		break;
	}

}

void drawLine()
{
	glLineWidth(5);
	//glBegin(GL_LINE_LOOP);
	glBegin(GL_LINE_STRIP);
	glColor3f(1.0, 0, 0);
	for (vector<double>::iterator it_x = path_x.begin(), it_y = path_y.begin(), it_z = path_z.begin();
		it_x != path_x.end(); it_x++, it_y++, it_z++)
	{
		glVertex3f(*it_x, *it_y, *it_z);
	}
	glEnd();
}

void drawStartAndEnd(GLfloat x, GLfloat y, GLfloat z){
	GLfloat len = 0.2;

	glTranslatef(x, y, z);							// Move Into The Screen And Left
	glBegin(GL_QUADS);										// Start Drawing A Quad
	glColor3f(0.0f, 1.0f, 0.0f);							// Blue
	glVertex3f(len, len, -len);						// Top Right Of The Quad (Top)
	glVertex3f(-len, len, -len);						// Top Left Of The Quad (Top)
	glVertex3f(-len, len, len);						// Bottom Left Of The Quad (Top)
	glVertex3f(len, len, len);						// Bottom Right Of The Quad (Top)

	glColor3f(0.0f, 1.0f, 0.0f);							// Blue
	glVertex3f(len, -len, len);						// Top Right Of The Quad (Bottom)
	glVertex3f(-len, -len, len);						// Top Left Of The Quad (Bottom)
	glVertex3f(-len, -len, -len);						// Bottom Left Of The Quad (Bottom)
	glVertex3f(len, -len, -len);						// Bottom Right Of The Quad (Bottom)

	glColor3f(0.0f, 1.0f, 0.0f);							// Blue
	glVertex3f(len, len, len);						// Top Right Of The Quad (Front)
	glVertex3f(-len, len, len);						// Top Left Of The Quad (Front)
	glVertex3f(-len, -len, len);						// Bottom Left Of The Quad (Front)
	glVertex3f(len, -len, len);						// Bottom Right Of The Quad (Front)

	glColor3f(0.0f, 1.0f, 0.0f);							// Blue
	glVertex3f(len, -len, -len);						// Top Right Of The Quad (Back)
	glVertex3f(-len, -len, -len);						// Top Left Of The Quad (Back)
	glVertex3f(-len, len, -len);						// Bottom Left Of The Quad (Back)
	glVertex3f(len, len, -len);						// Bottom Right Of The Quad (Back)

	glColor3f(0.0f, 1.0f, 0.0f);							// Blue
	glVertex3f(len, len, len);						// Top Right Of The Quad (Left)
	glVertex3f(-len, len, -len);						// Top Left Of The Quad (Left)
	glVertex3f(-len, -len, -len);						// Bottom Left Of The Quad (Left)
	glVertex3f(-len, -len, len);						// Bottom Right Of The Quad (Left)

	glColor3f(0.0f, 1.0f, 0.0f);							// Blue
	glVertex3f(len, len, -len);						// Top Right Of The Quad (Right)
	glVertex3f(len, -len, len);						// Bottom Left Of The Quad (Right)
	glVertex3f(len, -len, -len);						// Bottom Right Of The Quad (Right)
	glEnd();												// Done Drawing The Quad
}
void drawUnitAxes(GLfloat x, GLfloat y, GLfloat z){
	GLfloat len = 0.1;

	glTranslatef(x, y, z);							// Move Into The Screen And Left
	glTranslatef(moveX, moveY + 4, 10);							// Move Into The Screen And Left
	glBegin(GL_QUADS);										// Start Drawing A Quad
	glColor3f(0.0f, 1.0f, 1.0f);							// Blue
	glVertex3f(len, len, -len);						// Top Right Of The Quad (Top)
	glVertex3f(-len, len, -len);						// Top Left Of The Quad (Top)
	glVertex3f(-len, len, len);						// Bottom Left Of The Quad (Top)
	glVertex3f(len, len, len);						// Bottom Right Of The Quad (Top)

	glColor3f(1.0f, 0.0f, 1.0f);							// Blue
	glVertex3f(len, -len, len);						// Top Right Of The Quad (Bottom)
	glVertex3f(-len, -len, len);						// Top Left Of The Quad (Bottom)
	glVertex3f(-len, -len, -len);						// Bottom Left Of The Quad (Bottom)
	glVertex3f(len, -len, -len);						// Bottom Right Of The Quad (Bottom)

	glColor3f(1.0f, 1.0f, 0.0f);							// Blue
	glVertex3f(len, len, len);						// Top Right Of The Quad (Front)
	glVertex3f(-len, len, len);						// Top Left Of The Quad (Front)
	glVertex3f(-len, -len, len);						// Bottom Left Of The Quad (Front)
	glVertex3f(len, -len, len);						// Bottom Right Of The Quad (Front)

	glColor3f(1.0f, 1.0f, 1.0f);							// Blue
	glVertex3f(len, -len, -len);						// Top Right Of The Quad (Back)
	glVertex3f(-len, -len, -len);						// Top Left Of The Quad (Back)
	glVertex3f(-len, len, -len);						// Bottom Left Of The Quad (Back)
	glVertex3f(len, len, -len);						// Bottom Right Of The Quad (Back)

	glColor3f(0.0f, 0.5f, 1.0f);							// Blue
	glVertex3f(len, len, len);						// Top Right Of The Quad (Left)
	glVertex3f(-len, len, -len);						// Top Left Of The Quad (Left)
	glVertex3f(-len, -len, -len);						// Bottom Left Of The Quad (Left)
	glVertex3f(-len, -len, len);						// Bottom Right Of The Quad (Left)

	glColor3f(0.5f, 1.0f, 1.0f);							// Blue
	glVertex3f(len, len, -len);						// Top Right Of The Quad (Right)
	glVertex3f(len, -len, len);						// Bottom Left Of The Quad (Right)
	glVertex3f(len, -len, -len);						// Bottom Right Of The Quad (Right)
	glEnd();												// Done Drawing The Quad
}

//LRESULT dialog_opengl::KeyControl(UINT wParam, LONG lParam)
//{
//	this->SetWindowText(L"test");
//	OnKeyUp(wParam, 0, 0);
//	return 0;
//}

bool dialog_opengl::bSetDCPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0,
		16,
		0,
		0,
		0,
		0,
		0, 0, 0
	};
	int nPixelFormat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);
	if (0 == nPixelFormat) return false;
	return SetPixelFormat(m_pDC->GetSafeHdc(), nPixelFormat, &pfd);
}

void dialog_opengl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	//drawSence();
}

int dialog_opengl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_pDC = new CClientDC(this);
	ASSERT(m_pDC != NULL);
	//选择像素
	if (!bSetDCPixelFormat()) return -1;
	//创建渲染环境，并使它成为当前渲染环境
	m_hRC = wglCreateContext(m_pDC->GetSafeHdc());
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
	return 0;
}


void dialog_opengl::OnSize(UINT nType, int cx, int cy)
{
	FILE *fp;
	char cxCh[50] = { 0 };
	CDialog::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	glViewport(0, 0, cx, cy);
	sprintf(cxCh, "%d  %d", cx, cy);
	fp = fopen("h.txt", "w+");
	if (fp == NULL){
	}
	else{
		fwrite(cxCh, 1, strlen(cxCh), fp);
		fclose(fp);
	}
	getchar();
	//设置投影矩阵（透视投影）
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


void dialog_opengl::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hRC);
	delete m_pDC;
}


void dialog_opengl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
	/*上下左右： 38 40 37 39*/
	/*i j k l 73 74 75 76*/
	/*a s d w  65 83 68 87*/
	switch (nChar)
	{
	case 37:
		angle--;
		lookleft = 15 * sin(angle);
		lookforward = 15 * cos(angle);
		break;
	case 38:
		if (lookforward > 0)
			lookforward--;
		else
			lookforward++;
		break;
	case 39:
		angle++;
		lookleft = 15 * sin(angle);
		lookforward = 15 * cos(angle);
		break;
	case 40:
		if (lookforward > 0)
			lookforward++;
		else
			lookforward--;
		break;
	case 73:
		tranlaY++;
		break;
	case 74:
		tranlaX--;
		break;
	case 75:
		tranlaY--;
		break;
	case 76:
		tranlaX++;
		break;
	case 65:
		moveX--;
		break;
	case 83:
		moveY--;
		break;
	case 68:
		moveX++;
		break;
	case 87:
		moveY++;
		break;
	default:
		break;
	}
	drawSence();
}
