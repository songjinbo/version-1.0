
// DialogDlg.h : ͷ�ļ�
//

#pragma once

//#include "afxwin.h"
#include "GetImageThread.h"
#include "GetVoxelThread.h"
#include "PathPlanThread.h"
#include "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\atlmfc\include\afxwin.h"
#include "dialog_opengl.h"
#include "Resource.h"


#define WM_DISPLAY_IMAGE WM_USER+2

// CDialogDlg �Ի���
class CDialogDlg : public CDialogEx
{
// ����
public:
	CDialogDlg(CWnd* pParent = NULL);	// ��׼���캯��
// �Ի�������
	enum { IDD = IDD_DIALOG_DIALOG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CBrush m_BkBrush; //����һ����ˢ

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	/*afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);*/
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	double m_dx;
	double m_dy;
	double m_dz;
	double m_dyaw;
	double m_dpitch;
	double m_droll;
protected:
	//�����߳���Ҫ�õ�
	HANDLE hThread_get_image;
	DWORD ThreadID_get_image;
	void InitWindow(CStatic *, CStatic *, CStatic *);
	void Init();
	LRESULT DisplayImage(WPARAM wParam, LPARAM lParam);

public:
	CStatic m_DisplayLeft;
	CStatic m_DisplayDepth;
	afx_msg void OnBnClickedBrowse();
	void InitThread();
	GetImageThread* m_pget_image_thread;
	GetVoxelThread *m_pget_voxel_thread;
	PathPlanThread *m_ppath_plan_thread;
	double m_dendx;
	double m_dendy;
	double m_dendz;
	double m_dstartx;
	double m_dstarty;
	double m_dstartz;
	CStatic m_DisplayMap;
	dialog_opengl map_window;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	CStatic m_Title;
private:
	CFont titleFont;
	CFont groupFont;
	CFont poseFont;

};

enum ProgressStatus
{
	//���̵߳�״̬��־λ
	is_ruuning = 1,
	is_stopped,
	complete,

	//GetImage������״̬��־λ
	get_image_is_running,
	no_file,
	get_image_is_stopped,
	get_image_complete,

	//GetVoxel������״̬��־λ
	get_voxel_is_running,
	no_data_in_queue,
	get_voxel_is_stopped,
	get_one_voxel_complete,
	get_all_voxel_complete,

	//PathPlan������״̬��־λ
	path_plan_is_running,
	path_plan_is_stopped,
	path_accessible,
	no_path_accessible
};

typedef ProgressStatus get_image_ret_code;
typedef ProgressStatus get_voxel_ret_code;
typedef ProgressStatus path_plan_ret_code;