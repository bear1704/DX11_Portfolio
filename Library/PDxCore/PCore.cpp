#include "PCore.h"

HDC g_handle_off_screenDC;
HDC g_handle_screenDC;


PCore::PCore()
{
}


PCore::~PCore()
{
}

bool PCore::Init()
{
	return true;
}

bool PCore::PreFrame()
{
	
	return true;
}

bool PCore::Frame()
{

	return true;
}

bool PCore::PostFrame()
{
	return true;
}

bool PCore::PreRender()
{
	DevicePreRender();
	
	if (is_wireframe_render_)
		DX::ApplyRasterizerState(immediate_device_context_, DX::PDxState::rs_state_wireframe_);
	else
		DX::ApplyRasterizerState(immediate_device_context_, DX::PDxState::rs_state_solidframe_);

	
	DX::ApplyDepthStencilState(immediate_device_context_, DX::PDxState::depth_stencil_state_enable_); //재진수정
	
	DX::ApplySamplerState(immediate_device_context_, DX::PDxState::sampler_state_anisotropic);

	return true;
}

bool PCore::Render()
{
	return true;
}

bool PCore::PostRender()
{
	//BitBlt(handle_ScreenDC, 0, 0, rectangle_client.right, rectangle_client.bottom, handle_off_screenDC, 0, 0, SRCCOPY);
	swap_chain_->Present(0, 0);
	return false;
}

bool PCore::Release()
{
	return true;
}

bool PCore::PCoreInit()
{
	handle_ScreenDC = GetDC(hWnd); //스크린에 출력하기 위한 Device Context를 GetDC를 통해 생성한다.
	g_handle_screenDC = handle_ScreenDC; //다른 곳에서 참조할 수 있도록 전역변수와 스크린DC를 공유한다.

	handle_off_screenDC = CreateCompatibleDC(handle_ScreenDC); //백버퍼를 만들고, screenDC와 호환되도록 형태를 만든다.
	handle_off_screen_bitmap = CreateCompatibleBitmap(handle_ScreenDC, rectangle_client.right, rectangle_client.bottom); //백버퍼에 사용할 비트맵
	SelectObject(handle_off_screenDC, handle_off_screen_bitmap); //비트맵과 백버퍼를 바인드

	background_color = RGB(255, 255, 255);
	handle_background_brush = CreateSolidBrush(background_color); //백그라운드에 사용할 컬러 브러시(블랙)
	SelectObject(handle_off_screenDC, handle_background_brush); //브러시와 백버퍼를 바인드

	g_handle_off_screenDC = handle_off_screenDC;

	handle_pen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	SelectObject(handle_off_screenDC, handle_pen);

	timer.Init();
	PInput::GetInstance().Init();
	PSoundMgr::GetInstance().Init();
	P2DCamera::GetInstance().Init();
	PCollision::GetInstance().Init();
	PWallAndPlatform::GetInstance().Init();
	PObjectInfoManager::GetInstance().Init();
	InitDevice(hWnd, g_rectangle_client.right, g_rectangle_client.bottom);
	DX::PDxState::SetState(device_);
	PTextureManager::GetInstance().LoadTextureFromScript(L"data/tex.txt", device_);


	return Init();
}

bool PCore::PCoreFrame()
{
	PreFrame();
	timer.Frame();
	PInput::GetInstance().Frame();
	PSoundMgr::GetInstance().Frame();
	P2DCamera::GetInstance().Frame();
	Frame();
	return PostFrame();
}

bool PCore::PCoreRender()
{
	timer.Render();
	PInput::GetInstance().Render();
	PSoundMgr::GetInstance().Render();

	
	PreRender();
	Render();
	PostRender();
	
	return true;
}

bool PCore::PCoreRelease()
{
	DeleteObject(handle_background_brush);
	ReleaseDC(hWnd, handle_ScreenDC);


	timer.Release();
	PInput::GetInstance().Release();
	PSoundMgr::GetInstance().Release();
	P2DCamera::GetInstance().Release();

	return Release();
}

void PCore::MessageProc(MSG msg)
{
	PInput::GetInstance().MsgProc(msg);
}

bool PCore::Run()
{
	PCoreInit();
	MSG msg = { 0, };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			MessageProc(msg);
		}
		else
		{
			PCoreFrame();
			PCoreRender();
		}
	}

	PCoreRelease();

	return true;
}

void PCore::set_is_wireframe_render(bool is)
{
	is_wireframe_render_ = is;
}

void PCore::ToggleIsWireframeRender()
{
	is_wireframe_render_ = !is_wireframe_render_;
}
