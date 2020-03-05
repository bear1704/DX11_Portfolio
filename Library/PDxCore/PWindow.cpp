#include "PWindow.h"

PWindow* myWindow = nullptr;
HWND g_hWnd;
HINSTANCE g_hInstance;
FLOAT_RECT g_rectangle_client;
//FLOAT_RECT g_world_rect;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	if (myWindow != nullptr)
	{
		myWindow->MsgProc(hWnd, msg, wParam, lParam);
	}

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


LRESULT PWindow::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { return 1; }


bool PWindow::InitWindow(HINSTANCE hInstance, LPCWSTR titleName, int X, int Y, int width, int height)
{

	this->hInstance = hInstance;
	g_hInstance = hInstance;

	if (!MyRegisterClass())
		return false;

	RECT rt = { 0, 0, width, height };
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindow(className, titleName, WS_OVERLAPPEDWINDOW, 
		X, Y, rt.right-rt.left, rt.bottom-rt.top, NULL, NULL, this->hInstance, NULL);

	if (hWnd == NULL) return 0;

	ShowWindow(hWnd, SW_SHOW);
	CenterWindow();
	RECT rectangle_client_int;
	
	g_hWnd = hWnd;
	GetClientRect(hWnd, &rectangle_client_int);
	rectangle_client = { (float)rectangle_client_int.left, (float)rectangle_client_int.top, (float)rectangle_client_int.right, (float)rectangle_client_int.bottom };
	g_rectangle_client = rectangle_client;


	return true;
}

bool PWindow::MyRegisterClass()
{
	WNDCLASSEXW wcex;
	wcex.lpfnWndProc = WndProc;
	wcex.lpszClassName = className;
	wcex.hInstance = hInstance;
	
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wcex.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(nullptr, IDC_WAIT);
	
	wcex.lpszMenuName = NULL;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;

	if (!RegisterClassExW(&wcex))
		return false;
	
	return true;

	return RegisterClassEx(&wcex);
	
}

void PWindow::CenterWindow()
{
	FLOAT_RECT rtWindow;
	RECT rtWindow_int;
	GetWindowRect(hWnd, &rtWindow_int);
	rtWindow = { (float)rtWindow_int.left, (float)rtWindow_int.top, (float)rtWindow_int.right, (float)rtWindow_int.bottom};


	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int x = (screenWidth - (rtWindow.right - rtWindow.left)) / 2;
	int y = (screenHeight - (rtWindow.bottom - rtWindow.top)) / 2;
	MoveWindow(hWnd, x, y, rtWindow.right - rtWindow.left, rtWindow.bottom - rtWindow.top, true);
}






PWindow::PWindow(LPCWSTR className)
{
	this->className = className;
}

PWindow::PWindow()
{
	this->className = L"KGCA";
	myWindow = this;
}



PWindow::~PWindow()
{
}


