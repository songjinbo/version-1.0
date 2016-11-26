
// DialogDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CDialogDlg �Ի���



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


// CDialogDlg ��Ϣ�������
BOOL CDialogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	
		//�����ʼ��
		SetWindowText(L"���˻����ܷ�����ʾϵͳ"); //���ĶԻ������
		SkinH_AttachEx(L"../SkinSharp/Skins/TVB.she", NULL);//���Ƥ��

		//������ʼ��
		InitWindow(&m_DisplayLeft, &m_DisplayDepth,&m_DisplayMap);
		InitThread();
		system("md data"); //����һ���ļ��У���Ų���������
	return false;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDialogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDialogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/**********************************************************/

using namespace cv;
using namespace std;

#define MAXA 6400
#define PI 3.1415926

volatile ProgressStatus progress_status = is_stopped; //�г�ͻ����
char path[MAX_PATH] = { "" };//��������·�����г�ͻ����
int file_count;//����ѡ����ļ��е�depth.xml�ļ���֡��

double start_and_end[6]; //����·���滮ģ��,�г�ͻ����

CCriticalSection critical_rawdata;//����queue_depth��queue_left��queue_position�ķ���
vector<Mat> vec_depth;
vector<Mat> vec_left;
vector<Position> vec_position;

CCriticalSection critical_single_rawdata;//����depth_iamge��left_image��position�ķ���
Mat depth_image;
Mat left_image;
Position position;

int count_voxel_file = 1;//���ڶ����ػ������ݽ��м���
int count_opened = 0;
int no;

vector<double> voxel_x; //GetVoxelThread�����,PathPlanThread������
vector<double> voxel_y;
vector<double> voxel_z;

vector<double> path_x; //3DAStar������������ά��ʾģ�������
vector<double> path_y;
vector<double> path_z;

void CDialogDlg::OnBnClickedStart()
{
	this->SetWindowText(L"���˻����ܷ�����ʾϵͳ");
	GetDlgItem(IDC_START)->EnableWindow(FALSE);
	if (progress_status == is_stopped || progress_status == complete)
	{
		//��ʼ�����̣����Զ�ε��չʾ
		Init();
		//���������յ����������
		UpdateData(TRUE);
		start_and_end[0] = m_dstartx;
		start_and_end[1] = m_dstarty;
		start_and_end[2] = m_dstartz;
		start_and_end[3] = m_dendx;
		start_and_end[4] = m_dendy;
		start_and_end[5] = m_dendz;

		progress_status = is_ruuning;	
		//��ʼ���̵߳���������
		m_pget_image_thread->PostThreadMessage(WM_GETIMAGE_BEGIN, NULL, NULL);
		GetDlgItem(IDC_STATUS_GETIMAGE)->SetWindowTextW(_T("GetImage������������"));
		GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("GetImage������������"));//���״̬��
		GetDlgItem(IDC_STATUS_GETVOXEL)->SetWindowTextW(_T("GetVoxel�̴߳����ɹ�"));
		GetDlgItem(IDC_STATUS_PATHPLAN)->SetWindowTextW(_T("PathPlan�̴߳����ɹ�"));
	}
	else
	{
		//����ֱ���ͷ��̣߳����߳��Լ�return��Ȼ�����ͷ�
		progress_status = is_stopped;	
		GetDlgItem(IDC_START)->SetWindowTextW(_T("��ʼ"));
	}
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
}

LPCWSTR stringToLPCWSTR(std::string orig);

int CountFrames()
{
	char str_file_count[80];
	string command = "/c dir " + string(path) + " | findstr \"depth[0-9]*\.xml\" | find /v /c \"\">>.\\data\\file_count.txt";//ͳ����ѡ�ļ���depth.xml�ļ�����Ŀ�����뼼���ļ�file_count.txt��

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = L"cmd.exe";//���õĳ�����
	ShExecInfo.lpParameters = stringToLPCWSTR(command);//���ó���������в���
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;//����״̬Ϊ����
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	fstream file_read(".\\data\\file_count.txt", ios::in);
	file_read.getline(str_file_count, 80);
	int file_count = atoi(str_file_count);
	file_read.close();
	file_read.open(".\\data\\file_count.txt", ios::out); //���file_count.txt�ļ�������
	file_read.close();

	return file_count;
}

void CDialogDlg::Init()
{
	//��ʼ�����̣����Զ�ε��չʾ
	//critical_single_rawdata.Unlock();
	//critical_rawdata.Unlock();
	GetDlgItem(IDC_START)->SetWindowTextW(_T("ֹͣ"));
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
	file_count=CountFrames(); //ÿ�ε����ʼ��ʱ��Ҫ������ѡ�ļ��е�depth.xml������
}

char*display_window_name[2] = { "view_left", "view_depth" }; //�����������Ҫ����

void CDialogDlg::InitWindow(CStatic *m_DisplayLeft, CStatic *m_DisplayDepth, CStatic *m_DisplayMap)
{
	//��������������ʾ�����ͼƬ
	namedWindow(display_window_name[0], WINDOW_AUTOSIZE);
	HWND hWnd_left = (HWND)cvGetWindowHandle(display_window_name[0]);
	HWND hParent_left = ::GetParent(hWnd_left);
	::ShowWindow(hParent_left, SW_HIDE); //ԭ��������ʾ�Ĵ�������
	::SetParent(hWnd_left, m_DisplayLeft->m_hWnd);//����ʾ���渽����IDC_DISPLAYLEFT��
	//��������������ʾ���ͼ
	namedWindow(display_window_name[1], WINDOW_AUTOSIZE);
	HWND hWnd_depth = (HWND)cvGetWindowHandle(display_window_name[1]);
	HWND hParent_depth = ::GetParent(hWnd_depth);
	::ShowWindow(hParent_depth, SW_HIDE);
	::SetParent(hWnd_depth, m_DisplayDepth->m_hWnd);
	
	//��������������ʾ��ά��ͼ
	map_window.Create(IDD_DIALOG_OPENGL, this); //����һ���Ի���
	CRect rt;									//�ı�Ի���Ĵ�С��picture control���	
	m_DisplayMap->GetClientRect(rt);
	map_window.MoveWindow(rt);
	HWND hParent_map = ::GetParent(map_window);
	::ShowWindow(hParent_map, SW_HIDE); //ԭ��������ʾ�Ĵ�������
	::SetParent(map_window.m_hWnd, m_DisplayMap->m_hWnd); //��picture control��Ϊ�Ի���ĸ�����
	map_window.ShowWindow(SW_SHOW); //��ʾ�Ի���

	//���ñ��������ʽ
	titleFont.CreatePointFont(300, L"����");
	GetDlgItem(IDC_TITLE)->SetFont(&titleFont, true);

	//����group box�ؼ��������ʽ
	groupFont.CreateFont(20, 0, 0, 0, FW_BLACK, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	GetDlgItem(IDC_START_POSI)->SetWindowText(L"�������");
	GetDlgItem(IDC_START_POSI)->SetFont(&groupFont);
	GetDlgItem(IDC_END_POSI)->SetFont(&groupFont);
	GetDlgItem(IDC_POSE)->SetFont(&groupFont);
	GetDlgItem(IDC_INPUT_DIREC)->SetFont(&groupFont);
	GetDlgItem(IDC_SENSOR_DATA)->SetFont(&groupFont);
	GetDlgItem(IDC_3D_SCENE)->SetFont(&groupFont);

	//����λ�����ݵ������ʽ
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
		//GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("�̴߳����ɹ�"));
		GetDlgItem(IDC_STATUS_GETIMAGE)->SetWindowTextW(_T("GetImage�̴߳����ɹ�"));
		GetDlgItem(IDC_STATUS_GETVOXEL)->SetWindowTextW(_T("GetVoxel�̴߳����ɹ�"));
		GetDlgItem(IDC_STATUS_PATHPLAN)->SetWindowTextW(_T("PathPlan�̴߳����ɹ�"));
	}
	else
	{
		GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("�̴߳���ʧ��"));
		GetDlgItem(IDC_START)->EnableWindow(FALSE);
	}
}

void drawSence();

LRESULT CDialogDlg::DisplayImage(WPARAM wParam, LPARAM lParam)
{
	if (wParam == get_one_voxel_complete) //��ʾͼƬ����Ϣ
	{
		critical_single_rawdata.Lock();
		Mat depth_32S;//CV_16S��ʾ��ͼƬ�������⣬��ת��Ϊ8U
		Mat depth_image_cv8u;
		depth_image.convertTo(depth_32S, CV_32SC1);
		depth_32S = depth_32S * 255 / MAXA;
		depth_32S.convertTo(depth_image_cv8u, CV_8UC1);

		double x = double(int(position.x * 1000)) / 1000; double y = double(int(position.y * 1000)) / 1000; double z = double(int(position.z * 1000)) / 1000;//����С�������λ
		double yaw_angle = double(int(position.yaw * 180 / PI * 1000)) / 1000; //��ɽǶȣ�����С�������λ
		double roll_angle = double(int(position.roll * 180 / PI * 1000)) / 1000;
		double pitch_angle = double(int(position.pitch * 180 / PI * 1000)) / 1000;
		
		m_dx = x;
		m_dy = y;
		m_dz = z;
		m_droll = roll_angle;
		m_dpitch = pitch_angle;
		m_dyaw = roll_angle;
		UpdateData(false);         // ��������

		imshow(display_window_name[0], left_image);
		imshow(display_window_name[1], depth_image_cv8u);
		waitKey(1); //����Ҫ�еģ���������
		critical_single_rawdata.Unlock();
		
		//��GetImage������������queue�е�����
		m_pget_image_thread->PostThreadMessage(WM_GETIMAGE_BEGIN, NULL, NULL);

		Sleep(50); //�ӳ�50ms��������ʾ��ʱ��
	}
	
	else //���̺߳�����״̬��Ϣ
	{
		GetDlgItem(IDC_START)->EnableWindow(FALSE);
		if (wParam == no_file)
		{
			GetDlgItem(IDC_START)->SetWindowTextW(_T("��ʼ"));
			GetDlgItem(IDC_STATUS_GETIMAGE)->SetWindowTextW(_T("���н�����Ŀ¼û���ļ�"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("���н�����Ŀ¼û���ļ�"));
			progress_status = complete;//����ֹͣ����
		}
		else if (wParam == get_image_is_stopped)
		{
			GetDlgItem(IDC_STATUS_GETIMAGE)->SetWindowTextW(_T("���н�����GetImage������ǿ�ƽ���"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("ǿ�ƽ���"));
		}
		else if (wParam == get_voxel_is_stopped)
		{
			GetDlgItem(IDC_STATUS_GETVOXEL)->SetWindowTextW(_T("���н�����GetVoxel������ǿ�ƽ���"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("ǿ�ƽ���"));
		}
		else if (wParam == path_plan_is_stopped)
		{
			GetDlgItem(IDC_STATUS_PATHPLAN)->SetWindowTextW(_T("���н�����PathPlan������ǿ�ƽ���"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("ǿ�ƽ���"));
		}
		else if (wParam == get_image_complete)
		{
			GetDlgItem(IDC_STATUS_GETIMAGE)->SetWindowTextW(_T("���н�����ͼ�����ݶ�ȡ���"));
			m_pget_voxel_thread->PostThreadMessage(WM_GETVOXEL_BEGIN, NULL, NULL);
			GetDlgItem(IDC_STATUS_GETVOXEL)->SetWindowTextW(_T("GetVoxel������������"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("GetVoxel������������"));
		}
		else if (wParam == no_data_in_queue)
		{
			GetDlgItem(IDC_STATUS_GETVOXEL)->SetWindowTextW(_T("����Ϊ��"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("����Ϊ��"));
		}
		else if (wParam == get_all_voxel_complete)
		{
			GetDlgItem(IDC_STATUS_GETVOXEL)->SetWindowTextW(_T("���н��������ػ����"));
			m_ppath_plan_thread->PostThreadMessage(WM_PATHPLAN_BEGIN, NULL, NULL);
			GetDlgItem(IDC_STATUS_PATHPLAN)->SetWindowTextW(_T("PathPlan������������"));
		}
		else if (wParam == path_accessible)
		{
			GetDlgItem(IDC_START)->SetWindowTextW(_T("��ʼ"));
			GetDlgItem(IDC_STATUS_PATHPLAN)->SetWindowTextW(_T("���н������ҵ�·��"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("���н������ҵ�·��"));
			progress_status =complete;
			drawSence(); 
		}
		else if (wParam == no_path_accessible)
		{
			GetDlgItem(IDC_START)->SetWindowTextW(_T("��ʼ"));
			GetDlgItem(IDC_STATUS_PATHPLAN)->SetWindowTextW(_T("���н�����δ�ҵ�·��"));
			GetDlgItem(IDC_STATUS)->SetWindowTextW(_T("���н�����δ�ҵ�·��"));
			progress_status = complete;
			drawSence(); 
		}
		GetDlgItem(IDC_START)->EnableWindow(TRUE);
	}
	return 1;
}

void CDialogDlg::OnBnClickedBrowse()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	wchar_t szDir[MAX_PATH];
	
	BROWSEINFO bi;
	ITEMIDLIST *pidl;
	bi.hwndOwner = this->m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;
	bi.lpszTitle = L"��ѡ��Ŀ¼";
	bi.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	pidl = SHBrowseForFolder(&bi);
	if (pidl == NULL || !SHGetPathFromIDList(pidl, szDir))
	{
		GetDlgItem(IDC_STATUS)->SetWindowTextW(L"Ŀ¼��ʧ��");
		//if (GetFocus() == GetDlgItem(IDC_BROWSE))
			//GetDlgItem(IDC_BROWSE)->PostMessage(WM_KILLFOCUS, 0, 0);
		return;
	}
	else
	{
		critical_rawdata.Lock(); //��ֹpath�������ʳ�ͻ
		WideCharToMultiByte(CP_ACP, 0, szDir, -1, path, MAX_PATH, NULL, NULL);
		strcat_s(path, "\\");
		critical_rawdata.Unlock();
		GetDlgItem(IDC_STATUS)->SetWindowTextW(L"Ŀ¼�򿪳ɹ�");
		GetDlgItem(IDC_DISPLAYPATH)->SetWindowTextW(szDir);
		//if (GetFocus() == GetDlgItem(IDC_BROWSE))
			//GetDlgItem(IDC_BROWSE)->PostMessage(WM_KILLFOCUS, 0, 0);
		return;
	}
	UpdateData(FALSE);
}

//�����ı���ά��ͼ���ӽ�
extern float tranlaX;
extern float tranlaY;
extern float moveX;
extern float moveY;
extern float angle;
extern float lookleft;
extern float lookforward;

BOOL CDialogDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
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
