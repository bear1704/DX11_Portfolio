#pragma once
#include "PCreatePlaneDlg.h"
#include "PEffectObject.h"

// PToolForm 폼 보기


class PToolForm : public CFormView
{
	DECLARE_DYNCREATE(PToolForm)

protected:
	PToolForm();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~PToolForm();

public:
	static PToolForm* CreateOne(CWnd* pParent);
	CPToolApp* app;

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EffToolForm };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	PCreatePlaneDlg m_DlgPlane;
	CButton m_BtnCreatePlane;
	PParser parse;
	FILE* fp;
	PEffectObject* m_pCurrentEffObj;
	std::vector<PEffectObject*> m_pCurrentEffectObjectList;
	PModel* m_pCurrentObject;

	afx_msg void OnBnClickedBtnCreateplane();
	CComboBox m_CtlBlendSrc;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();
	afx_msg void OnCbnSelchangeComboBlendSrc();
	BOOL m_IsAlphaBlend;
	afx_msg void OnCbnSelchangeComboBlendDest();
	CComboBox m_CtlBlendDest;
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedCheckBlend();
	void SaveData();
	void LoadData(std::string path);
	float m_FadeInNum;
	float m_FadeOutNum;
	afx_msg void OnBnClickedBtnEffectapply();
	CComboBox m_CtlPlaneList;
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnCbnSelchangeComboPlanelist();
	float m_VeloX;
	float m_VeloY;
	float m_Gravity;
	float m_VeloZ;
	float m_LaunchTime;
	float m_LifeTIme;
	afx_msg void OnBnClickedBtnCreateobject();
	afx_msg void OnBnClickedBtnObjectapply();
	float m_WorldTx;
	float m_WorldTy;
	float m_WorldTz;
	float m_WorldRx;
	float m_WorldRy;
	float m_WorldRz;
	float m_WorldSx;
	float m_WorldSy;
	float m_WorldSz;
	afx_msg void OnBnClickedBtnRefreshobj();
	CComboBox m_CtlObjectList;
	afx_msg void OnCbnSelchangeComboObjectlist();
	float m_AxisX;
	float m_AxisY;
	float m_AxisZ;
	float m_Radius;
	BOOL m_IsClockWise;
	float m_RotateSpeed;
	BOOL m_IsAnimationOn;
	float m_EffectTx;
	float m_EffectTy;
	float m_EffectTz;
	float m_EffectSx;
	float m_EffectSy;
	float m_EffectSz;
	BOOL m_IsUsingBillboard;
	CListBox m_EffectListBox;
	afx_msg void OnBnClickedBtnEffectadd();
	afx_msg void OnBnClickedBtnEffectremove();
	afx_msg void OnBnClickedBtnApplyall();
	afx_msg void OnBnClickedBtnEffectallsel();
	BOOL m_IsUsingFountain;
	afx_msg void OnBnClickedBtnDeleteitem();
};


