
// Dialog.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//



#pragma once

//#include "SkinMagicLib.h"

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDialogApp: 
// �йش����ʵ�֣������ Dialog.cpp
//

class CDialogApp : public CWinApp
{
public:
	CDialogApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
//	virtual int ExitInstance();
//	virtual int ExitInstance();
};

extern CDialogApp theApp;