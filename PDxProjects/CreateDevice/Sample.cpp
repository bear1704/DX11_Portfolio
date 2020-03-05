#include "Sample.h"


Sample::Sample()
{
}

Sample::~Sample()
{
}

bool Sample::Init()
{



	//D3D_DRIVER_TYPE driver_types[] =
	//{
	//	D3D_DRIVER_TYPE_HARDWARE,
	//	D3D_DRIVER_TYPE_WARP,
	//	D3D_DRIVER_TYPE_REFERENCE

	//};

	//D3D_FEATURE_LEVEL feature_levels[] =
	//{
	//	D3D_FEATURE_LEVEL_11_1,
	//	D3D_FEATURE_LEVEL_11_0,
	//	D3D_FEATURE_LEVEL_10_0
	//};


	//HRESULT hr; //Dx메소드의 반환값은 다 HRESULT이다.
	//UINT CreateDeviceFlags = 0;

	//ZeroMemory(&desc_swap_chain_, sizeof(desc_swap_chain_));
	//desc_swap_chain_.BufferCount = 1;
	//desc_swap_chain_.OutputWindow = hWnd;
	//desc_swap_chain_.Windowed = true;
	//desc_swap_chain_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//desc_swap_chain_.SampleDesc.Count = 1;
	//desc_swap_chain_.BufferDesc.Width = g_rectangle_client.right;
	//desc_swap_chain_.BufferDesc.Height = g_rectangle_client.bottom;
	//desc_swap_chain_.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//desc_swap_chain_.BufferDesc.RefreshRate.Numerator = 60;
	//desc_swap_chain_.BufferDesc.RefreshRate.Denominator = 1;


	//for (int i = 0; i < ARRAYSIZE(driver_types); i++)
	//{
	//	driver_type_ = driver_types[i];


	//		hr = D3D11CreateDeviceAndSwapChain(
	//			NULL, driver_type_, NULL,
	//			CreateDeviceFlags, feature_levels, ARRAYSIZE(feature_levels),
	//			D3D11_SDK_VERSION, &desc_swap_chain_, &swap_chain_,
	//			&device_, &feature_level_, &immediate_device_context_);

	//		if (SUCCEEDED(hr))
	//		{
	//			if (feature_level_ < D3D_FEATURE_LEVEL_11_0)
	//			{ //10.0으로 생성된 스왑체인, 디바이스 컨텍스트, 디바이스를 릴리즈한다.
	//				swap_chain_->Release();
	//				immediate_device_context_->Release();
	//				device_->Release();
	//				continue;
	//			}
	//			break;
	//		}
	//		
	//		
	//}

	//if (FAILED(hr))
	//	return false;

	//ID3D11Texture2D* back_buffer = nullptr;
	//hr = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
	//if (FAILED(hr)) return false;

	//hr = device_->CreateRenderTargetView(back_buffer, NULL, &render_target_view_);
	//back_buffer->Release();

	//if (FAILED(hr)) return false;

	//immediate_device_context_->OMSetRenderTargets(1, &render_target_view_, NULL);

	//viewport_.Width = g_rectangle_client.right;
	//viewport_.Height = g_rectangle_client.bottom;
	//viewport_.MinDepth = 0;
	//viewport_.MaxDepth = 1;
	//viewport_.TopLeftX = 0;
	//viewport_.TopLeftY = 0;
	//immediate_device_context_->RSSetViewports(1, &viewport_);

	
	InitDevice(hWnd, g_rectangle_client.right, g_rectangle_client.bottom);
	CreateRenderTarget(g_rectangle_client.right, g_rectangle_client.bottom);
	
	return true;
}

bool Sample::Frame()
{
	return true;
}

bool Sample::Render()
{
	//float clear_color[] = { 0.23f, 0.5f, 0.463f, 1 };
	//immediate_device_context_->ClearRenderTargetView(render_target_view_, clear_color);

	//swap_chain_->Present(0, 0);
	DevicePreRender();
	DeviceRender();
	return true;
}

bool Sample::Release()
{

	//render_target_view_->Release();
	//swap_chain_->Release();
	//immediate_device_context_->Release();
	//device_->Release();
	DeviceRelease();

	return true;
}
