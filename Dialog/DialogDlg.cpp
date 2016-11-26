
// DialogDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "afxmt.h"
#include <stdlib.h>
#include <queue>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Dialog.h"
#include "afxdialogex.h"
#include "DialogDlg.h"
#include "GetImageThread.h"
#include "GetVoxelThread.h"
#include "dialog_opengl.h"
#include "..\\SkinSharp\\SkinH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDialogDlg 对话框



CDialogDlg::CDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogDlg::IDD, pParent)
	, m_dx(0)
	, m_dy(0)
	, m_dz(0)
	, m_dyaw(0)
	, m_dpitch(0)
	, m_droll(0)
	, m_dstartx(0)
	, m_dendx(0)
	, m_dstarty(0)
	, m_dendy(0)
	, m_dendz(0)
	, m_dstartz(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_X, m_dx);
	DDX_Text(pDX, IDC_Y, m_dy);
	DDX_Text(pDX, IDC_Z, m_dz);
	DDX_Text(pDX, IDC_YAW, m_dyaw);
	DDX_Text(pDX, IDC_PITCH, m_dpitch);
	DDX_Text(pDX, IDC_ROLL, m_droll);
	DDX_Control(pDX, IDC_DISPLAYLEFT, m_DisplayLeft);
	DDX_Control(pDX, IDC_DISPLAYDEPTH, m_DisplayDepth);
	DDX_Text(pDX, IDC_ENDX, m_dendx);
	DDX_Text(pDX, IDC_ENDY, m_dendy);
	DDX_Text(pDX, IDC_ENDZ, m_dendz);
	DDX_Text(pDX, IDC_STARTX, m_dstartx);
	DDX_Text(pDX, IDC_STARTY, m_dstarty);
	DDX_Text(pDX, IDC_STARTZ, m_dstartz);
	DDX_Control(pDX, IDC_DISPLAYMAP, m_DisplayMap);
	//  DDX_Control(pDX, IDC_TITLE, m_Title);
}

BEGIN_MESSAGE_MAP(CDialogDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CDialogDlg::OnBnClickedStart)
	ON_MESSAGE(WM_DISPLAY_IMAGE, DisplayImage)
	ON_BN_CLICKED(IDC_BROWSE, &CDialogDlg::OnBnClickedBrowse)
	//ON_WM_ERASEBKGND()
	//ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDialogDlg 消息处理程序
BOOL CDialogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// TODO:  在此添加额外的初始化代码
	
		//界面初始化
		SetWindowText(L"无人机智能飞行演示系统"); //更改对话框标题
		SkinH_AttachEx(L"../SkinSharp/Skins/TVB.she", NULL);//添加皮肤

		//变量初始化
		InitWindow(&m_DisplayLeft, &m_DisplayDepth,&m_DisplayMap);
		InitThread();
		system("md data"); //创建一个文件夹，存放产生的数据
	return false;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDialogDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDialogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDialogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/**********************************************************/

using namespace cv;
using namespace std;

#define MAXA 6400
#define PI 3.1415926

volatile ProgressStatus progress_status = is_stopped; //有冲突隐患
char path[MAX_PATH] = { "" };//输入数据路径，有冲突隐患
int file_count;//计数选择的文件夹的depth.xml文件的帧数

double start_and_end[6]; //传给路径规划模块,有冲突隐患

CCriticalSection critical_rawdata;//控制queue_depth、queue_left和queue_position的访问
vector<Mat> vec_depth;
vector<Mat> vec_left;
vector<Position> vec_position;

CCriticalSection critical_single_rawdata;//控制depth_iamge、left_image和position的访问
Mat depth_image;
Mat left_image;
Position position;

int count_voxel_file = 1;//用于对体素化的数据进行计数
int count_opened = 0;
int no;

vector<double> voxel_x; //GetVoxelThread的输出,PathPlanThread的输入
vector<double> voxel_y;
vector<double> voxel_z;

vector<double> path_x; //3DAStar程序的输出，三维显示模块的输入
vector<double> path_y;
vector<double> path_z;

void CDialogDlg::OnBnClickedStart()
{
	this->SetWindowText(L"无人机智能飞行演示系统");
	GetDlgItem(IDC_START)->EnableWindow(FALSE);
	if (progress_status == is_stopped || progress_status == complete)
	{
		//初始化过程，可以多次点击展示
		Init();
		//获得起点与终点的坐标数据
		UpdateData(TRUE);
		start_and_end[0] = m_dstartx;
		start_and_end[1] = m_dstarty;
		start_and_end[2] = m_dstartz;
		start_and_end[3] = m_dendx;
		start_and_end[4] = m_dendy;
		start_and_end[5] = m_dendz;

		progress_status = is_ruuning;	
		//开始子线程的两个函数
		m_pget_image_thread->PostThreadMessage(WM_GETIMAGE_BEGIN, NULL, NULL);
		GetDlgItem(IDC_STATUS_GETIMAGE)->SetWindowTextW(_T("GetImage函数正在运行"));
		GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("GetImage函数正在运行"));//清空状态栏
		GetDlgItem(IDC_STATUS_GETVOXEL)->SetWindowTextW(_T("GetVoxel线程创建成功"));
		GetDlgItem(IDC_STATUS_PATHPLAN)->SetWindowTextW(_T("PathPlan线程创建成功"));
	}
	else
	{
		//不能直接释放线程，让线程自己return，然后再释放
		progress_status = is_stopped;	
		GetDlgItem(IDC_START)->SetWindowTextW(_T("开始"));
	}
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
}

LPCWSTR stringToLPCWSTR(std::string orig);

int CountFrames()
{
	char str_file_count[80];
	string command = "/c dir " + string(path) + " | findstr \"depth[0-9]*\.xml\" | find /v /c \"\">>.\\data\\file_count.txt";//统计所选文件夹depth.xml文件的数目并存入技术文件file_count.txt中

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = L"cmd.exe";//调用的程序名
	ShExecInfo.lpParameters = stringToLPCWSTR(command);//调用程序的命令行参数
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;//窗口状态为隐藏
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	fstream file_read(".\\data\\file_count.txt", ios::in);
	file_read.getline(str_file_count, 80);
	int file_count = atoi(str_file_count);
	file_read.close();
	file_read.open(".\\data\\file_count.txt", ios::out); //清除file_count.txt文件的内容
	file_read.close();

	return file_count;
}

void CDialogDlg::Init()
{
	//初始化过程，可以多次点击展示
	//critical_single_rawdata.Unlock();
	//critical_rawdata.Unlock();
	GetDlgItem(IDC_START)->SetWindowTextW(_T("停止"));
	depth_image = Mat(Scalar(0));
	left_image = Mat(Scalar(0));
	vec_depth.clear();
	vec_left.clear();
	vec_position.clear();
	voxel_x.clear();
	voxel_y.clear();
	voxel_z.clear();
	position.x = position.y = position.z = position.pitch = position.yaw = position.roll = 0;
	count_voxel_file = 1;
	count_opened = 0;
	no = 1;
	file_count=CountFrames(); //每次点击开始的时候都要计算所选文件夹的depth.xml的数量
}

char*display_window_name[2] = { "view_left", "view_depth" }; //这个变量不需要更改

void CDialogDlg::InitWindow(CStatic *m_DisplayLeft, CStatic *m_DisplayDepth, CStatic *m_DisplayMap)
{
	//创建窗口用来显示左相机图片
	namedWindow(display_window_name[0], WINDOW_AUTOSIZE);
	HWND hWnd_left = (HWND)cvGetWindowHandle(display_window_name[0]);
	HWND hParent_left = ::GetParent(hWnd_left);
	::ShowWindow(hParent_left, SW_HIDE); //原先用来显示的窗口消隐
	::SetParent(hWnd_left, m_DisplayLeft->m_hWnd);//将显示画面附着在IDC_DISPLAYLEFT上
	//创建窗口用来显示深度图
	namedWindow(display_window_name[1], WINDOW_AUTOSIZE);
	HWND hWnd_depth = (HWND)cvGetWindowHandle(display_window_name[1]);
	HWND hParent_depth = ::GetParent(hWnd_depth);
	::ShowWindow(hParent_depth, SW_HIDE);
	::SetParent(hWnd_depth, m_DisplayDepth->m_hWnd);
	
	//创建窗口用来显示三维地图
	map_window.Create(IDD_DIALOG_OPENGL, this); //创建一个对话框
	CRect rt;									//改变对话框的大小与picture control相等	
	m_DisplayMap->GetClientRect(rt);
	map_window.MoveWindow(rt);
	HWND hParent_map = ::GetParent(map_window);
	::ShowWindow(hParent_map, SW_HIDE); //原先用来显示的窗口消隐
	::SetParent(map_window.m_hWnd, m_DisplayMap->m_hWnd); //将picture control设为对话框的父窗口
	map_window.ShowWindow(SW_SHOW); //显示对话框

	//设置标题字体格式
	titleFont.CreatePointFont(300, L"楷体");
	GetDlgItem(IDC_TITLE)->SetFont(&titleFont, true);

	//设置group box控件的字体格式
	groupFont.CreateFont(20, 0, 0, 0, FW_BLACK, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	GetDlgItem(IDC_START_POSI)->SetWindowText(L"起点坐标");
	GetDlgItem(IDC_START_POSI)->SetFont(&groupFont);
	GetDlgItem(IDC_END_POSI)->SetFont(&groupFont);
	GetDlgItem(IDC_POSE)->SetFont(&groupFont);
	GetDlgItem(IDC_INPUT_DIREC)->SetFont(&groupFont);
	GetDlgItem(IDC_SENSOR_DATA)->SetFont(&groupFont);
	GetDlgItem(IDC_3D_SCENE)->SetFont(&groupFont);

	//设置位姿数据的字体格式
	poseFont.CreateFont(20, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,_T("Arial"));
	GetDlgItem(IDC_X)->SetFont(&poseFont);
	GetDlgItem(IDC_Y)->SetFont(&poseFont);
	GetDlgItem(IDC_Z)->SetFont(&poseFont);
	GetDlgItem(IDC_ROLL)->SetFont(&poseFont);
	GetDlgItem(IDC_PITCH)->SetFont(&poseFont);
	GetDlgItem(IDC_YAW)->SetFont(&poseFont);
	GetDlgItem(IDC_STATIC)->SetFont(&poseFont);


}

void CDialogDlg::InitThread()
{
	m_pget_image_thread = (GetImageThread*)AfxBeginThread(RUNTIME_CLASS(GetImageThread));
	m_pget_voxel_thread = (GetVoxelThread*)AfxBeginThread(RUNTIME_CLASS(GetVoxelThread));
	m_ppath_plan_thread = (PathPlanThread*)AfxBeginThread(RUNTIME_CLASS(PathPlanThread));
	if (m_pget_image_thread && m_pget_voxel_thread && m_ppath_plan_thread)
	{
		//GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("线程创建成功"));
		GetDlgItem(IDC_STATUS_GETIMAGE)->SetWindowTextW(_T("GetImage线程创建成功"));
		GetDlgItem(IDC_STATUS_GETVOXEL)->SetWindowTextW(_T("GetVoxel线程创建成功"));
		GetDlgItem(IDC_STATUS_PATHPLAN)->SetWindowTextW(_T("PathPlan线程创建成功"));
	}
	else
	{
		GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("线程创建失败"));
		GetDlgItem(IDC_START)->EnableWindow(FALSE);
	}
}

void drawSence();

LRESULT CDialogDlg::DisplayImage(WPARAM wParam, LPARAM lParam)
{
	if (wParam == get_one_voxel_complete) //显示图片的消息
	{
		critical_single_rawdata.Lock();
		Mat depth_32S;//CV_16S显示成图片会有问题，需转化为8U
		Mat depth_image_cv8u;
		depth_image.convertTo(depth_32S, CV_32SC1);
		depth_32S = depth_32S * 255 / MAXA;
		depth_32S.convertTo(depth_image_cv8u, CV_8UC1);

		double x = double(int(position.x * 1000)) / 1000; double y = double(int(position.y * 1000)) / 1000; double z = double(int(position.z * 1000)) / 1000;//保留小数点后三位
		double yaw_angle = double(int(position.yaw * 180 / PI * 1000)) / 1000; //变成角度，保留小数点后三位
		double roll_angle = double(int(position.roll * 180 / PI * 1000)) / 1000;
		double pitch_angle = double(int(position.pitch * 180 / PI * 1000)) / 1000;
		
		m_dx = x;
		m_dy = y;
		m_dz = z;
		m_droll = roll_angle;
		m_dpitch = pitch_angle;
		m_dyaw = roll_angle;
		UpdateData(false);         // 更新数据

		imshow(display_window_name[0], left_image);
		imshow(display_window_name[1], depth_image_cv8u);
		waitKey(1); //必须要有的，不能忘记
		critical_single_rawdata.Unlock();
		
		//让GetImage函数继续处理queue中的数据
		m_pget_image_thread->PostThreadMessage(WM_GETIMAGE_BEGIN, NULL, NULL);

		Sleep(50); //延迟50ms，留给显示的时间
	}
	
	else //子线程函数的状态消息
	{
		GetDlgItem(IDC_START)->EnableWindow(FALSE);
		if (wParam == no_file)
		{
			GetDlgItem(IDC_START)->SetWindowTextW(_T("开始"));
			GetDlgItem(IDC_STATUS_GETIMAGE)->SetWindowTextW(_T("运行结束，目录没有文件"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("运行结束，目录没有文件"));
			progress_status = complete;//发出停止命令
		}
		else if (wParam == get_image_is_stopped)
		{
			GetDlgItem(IDC_STATUS_GETIMAGE)->SetWindowTextW(_T("运行结束，GetImage函数被强制结束"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("强制结束"));
		}
		else if (wParam == get_voxel_is_stopped)
		{
			GetDlgItem(IDC_STATUS_GETVOXEL)->SetWindowTextW(_T("运行结束，GetVoxel函数被强制结束"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("强制结束"));
		}
		else if (wParam == path_plan_is_stopped)
		{
			GetDlgItem(IDC_STATUS_PATHPLAN)->SetWindowTextW(_T("运行结束，PathPlan函数被强制结束"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("强制结束"));
		}
		else if (wParam == get_image_complete)
		{
			GetDlgItem(IDC_STATUS_GETIMAGE)->SetWindowTextW(_T("运行结束，图像数据读取完毕"));
			m_pget_voxel_thread->PostThreadMessage(WM_GETVOXEL_BEGIN, NULL, NULL);
			GetDlgItem(IDC_STATUS_GETVOXEL)->SetWindowTextW(_T("GetVoxel函数正在运行"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("GetVoxel函数正在运行"));
		}
		else if (wParam == no_data_in_queue)
		{
			GetDlgItem(IDC_STATUS_GETVOXEL)->SetWindowTextW(_T("队列为空"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("队列为空"));
		}
		else if (wParam == get_all_voxel_complete)
		{
			GetDlgItem(IDC_STATUS_GETVOXEL)->SetWindowTextW(_T("运行结束，体素化完毕"));
			m_ppath_plan_thread->PostThreadMessage(WM_PATHPLAN_BEGIN, NULL, NULL);
			GetDlgItem(IDC_STATUS_PATHPLAN)->SetWindowTextW(_T("PathPlan函数正在运行"));
		}
		else if (wParam == path_accessible)
		{
			GetDlgItem(IDC_START)->SetWindowTextW(_T("开始"));
			GetDlgItem(IDC_STATUS_PATHPLAN)->SetWindowTextW(_T("运行结束，找到路径"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("运行结束，找到路径"));
			progress_status =complete;
			drawSence(); 
		}
		else if (wParam == no_path_accessible)
		{
			GetDlgItem(IDC_START)->SetWindowTextW(_T("开始"));
			GetDlgItem(IDC_STATUS_PATHPLAN)->SetWindowTextW(_T("运行结束，未找到路径"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("运行结束，未找到路径"));
			progress_status = complete;
			drawSence(); 
		}
		GetDlgItem(IDC_START)->EnableWindow(TRUE);
	}
	return 1;
}

void CDialogDlg::OnBnClickedBrowse()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	wchar_t szDir[MAX_PATH];
	
	BROWSEINFO bi;
	ITEMIDLIST *pidl;
	bi.hwndOwner = this->m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;
	bi.lpszTitle = L"请选择目录";
	bi.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	pidl = SHBrowseForFolder(&bi);
	if (pidl == NULL || !SHGetPathFromIDList(pidl, szDir))
	{
		GetDlgItem(IDC_STATUS)->SetWindowTextW(L"目录打开失败");
		//if (GetFocus() == GetDlgItem(IDC_BROWSE))
			//GetDlgItem(IDC_BROWSE)->PostMessage(WM_KILLFOCUS, 0, 0);
		return;
	}
	else
	{
		critical_rawdata.Lock(); //防止path变量访问冲突
		WideCharToMultiByte(CP_ACP, 0, szDir, -1, path, MAX_PATH, NULL, NULL);
		strcat_s(path, "\\");
		critical_rawdata.Unlock();
		GetDlgItem(IDC_STATUS)->SetWindowTextW(L"目录打开成功");
		GetDlgItem(IDC_DISPLAYPATH)->SetWindowTextW(szDir);
		//if (GetFocus() == GetDlgItem(IDC_BROWSE))
			//GetDlgItem(IDC_BROWSE)->PostMessage(WM_KILLFOCUS, 0, 0);
		return;
	}
	UpdateData(FALSE);
}

//用来改变三维地图的视角
extern float tranlaX;
extern float tranlaY;
extern float moveX;
extern float moveY;
extern float angle;
extern float lookleft;
extern float lookforward;

BOOL CDialogDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYUP)
	{
		switch (pMsg->wParam)
		{
		case 37:
			angle--;
			lookleft = 15 * sin(angle);
			lookforward = 15 * cos(angle);
			drawSence(); 
			break;
		case 38:
			if (lookforward > 0)
				lookforward--;
			else
				lookforward++;
			drawSence(); 
			break;
		case 39:
			angle++;
			lookleft = 15 * sin(angle);
			lookforward = 15 * cos(angle);
			drawSence(); 
			break;
		case 40:
			if (lookforward > 0)
				lookforward++;
			else
				lookforward--;
			drawSence(); 
			break;
		case 73:
			tranlaY++;
			drawSence(); 
			break;
		case 74:
			tranlaX--;
			drawSence(); 
			break;
		case 75:
			tranlaY--;
			drawSence(); 
			break;
		case 76:
			tranlaX++;
			drawSence(); 
			break;
		case 65:
			moveX--;
			drawSence(); 
			break;
		case 83:
			moveY--;
			drawSence(); 
			break;
		case 68:
			moveX++;
			drawSence(); 
			break;
		case 87:
			moveY++;
			drawSence();
			break;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
