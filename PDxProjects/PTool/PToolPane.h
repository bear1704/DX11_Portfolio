#pragma once
//#include <afxdockablepane.h>
#include "PToolForm.h"

class PToolPane :
	public CDockablePane
{
	DECLARE_DYNAMIC(PToolPane);

public:
	PToolPane();
	virtual ~PToolPane();
	PToolForm* m_wndForm;

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};

