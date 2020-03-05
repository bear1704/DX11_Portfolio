#pragma once
#include "PDevice.h"
#include "PInput.h"
#include "PTimer.h"
#include "PSoundMgr.h"
#include "P2DCamera.h"
#include "PWallAndPlatform.h"
#include "PObjectInfoManager.h"

class PCore : public PDevice
{
public:
	PCore();
	virtual ~PCore();
public:
	PTimer timer;
	HDC handle_ScreenDC;
	HDC handle_off_screenDC;
	HBITMAP handle_off_screen_bitmap;
	DWORD background_color;
	HBRUSH handle_background_brush;
	HANDLE handle_pen;

protected:
	bool is_wireframe_render_;

public:
	virtual bool Init();
	virtual bool PreFrame();
	virtual bool Frame();
	virtual bool PostFrame();
	virtual bool PreRender();
	virtual bool Render();
	virtual bool PostRender();
	virtual bool Release();

public:
	bool PCoreInit();
	bool PCoreFrame();
	bool PCoreRender();
	bool PCoreRelease();
public:
	virtual void MessageProc(MSG msg);

public:
	bool Run();
	void set_is_wireframe_render(bool is);
	void ToggleIsWireframeRender();

public:



};

