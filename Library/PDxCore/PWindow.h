#pragma once
#include "PStd.h"


class PWindow
{
public:
	PWindow(LPCWSTR className);
	PWindow();
	virtual ~PWindow();
public:
	HINSTANCE hInstance;
	HWND hWnd;
	LPCWSTR className;
	FLOAT_RECT rectangle_client;


public:
	bool InitWindow(HINSTANCE hInstance, LPCWSTR titleName, int X = 0, int Y = 0, int width = 800, int height = 600);
	bool MyRegisterClass();
	void CenterWindow();
	LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	

};

