
// PToolView.cpp: CPToolView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "PTool.h"
#endif

#include "PToolDoc.h"
#include "PToolView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPToolView

IMPLEMENT_DYNCREATE(CPToolView, CView)

BEGIN_MESSAGE_MAP(CPToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPToolView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CPToolView 생성/소멸

CPToolView::CPToolView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CPToolView::~CPToolView()
{
}

BOOL CPToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CPToolView 그리기

void CPToolView::OnDraw(CDC* /*pDC*/)
{
	CPToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CPToolView 인쇄


void CPToolView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CPToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CPToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CPToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CPToolView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CPToolView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CPToolView 진단

#ifdef _DEBUG
void CPToolView::AssertValid() const
{
	CView::AssertValid();
}

void CPToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPToolDoc* CPToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPToolDoc)));
	return (CPToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CPToolView 메시지 처리기


LRESULT CPToolView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CPToolApp* pApp = (CPToolApp*)AfxGetApp();
	HWND hWnd = pApp->m_tool.hWnd;
	MSG msg;
	msg.message = message;
	msg.wParam = wParam;
	msg.lParam = lParam;
	msg.hwnd = hWnd;
	pApp->m_tool.MessageProc(msg);
	return CView::WindowProc(message, wParam, lParam);
}
