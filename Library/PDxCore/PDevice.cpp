#include "PDevice.h"

PDevice::PDevice()
{
}

PDevice::~PDevice()
{
}

bool PDevice::InitDevice(HWND hwnd, UINT client_width, UINT client_height)
{
	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE

	};

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	HRESULT hr;
	UINT CreateDeviceFlags = 0;
	UINT driver_type_size = ARRAYSIZE(driver_types);
	UINT feature_level_size = ARRAYSIZE(feature_levels);

	CreateSwapChain(hwnd, client_width, client_height);

	for (int type = 0; type < driver_type_size; type++)
	{
		driver_type_ = driver_types[type];

		hr = D3D11CreateDeviceAndSwapChain(
			NULL, driver_type_, NULL,
			CreateDeviceFlags,
			feature_levels,
			feature_level_size,
			D3D11_SDK_VERSION,
			&swap_chain_desc_,
			&swap_chain_,
			&device_,
			&feature_level_,
			&immediate_device_context_);

		if (SUCCEEDED(hr))
		{
			if (feature_level_ < D3D_FEATURE_LEVEL_11_0)
			{
				swap_chain_->Release();
				immediate_device_context_->Release();
				device_->Release();
				continue;
			}
			break;
		}
		
		
	}
	if (FAILED(hr))
		return false;

	CreateRenderTarget(g_rectangle_client.right, g_rectangle_client.bottom);



	return true;
}

bool PDevice::CreateSwapChain(HWND hwnd, UINT client_width, UINT client_height)
{

	
	ZeroMemory(&swap_chain_desc_, sizeof(swap_chain_desc_));
	swap_chain_desc_.BufferCount = 1;
	swap_chain_desc_.OutputWindow = hwnd;
	swap_chain_desc_.Windowed = true;
	swap_chain_desc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc_.SampleDesc.Count = 1;
	swap_chain_desc_.BufferDesc.Width = client_width;
	swap_chain_desc_.BufferDesc.Height = client_height;
	swap_chain_desc_.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc_.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc_.BufferDesc.RefreshRate.Denominator = 1;

	return true;
}

bool PDevice::CreateRenderTarget(UINT client_width, UINT client_height)
{

	ID3D11Texture2D* back_buffer;
	HRESULT hr = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& back_buffer);
	if (FAILED(hr)) return false;

	hr = device_->CreateRenderTargetView(back_buffer, NULL, &render_target_view_);
	back_buffer->Release();

	if (FAILED(hr)) return false;

	immediate_device_context_->OMSetRenderTargets(1, &render_target_view_, NULL);


	//Depth, Stencil 버퍼 생성
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	D3D11_TEXTURE2D_DESC tex_2d;
	ZeroMemory(&tex_2d, sizeof(D3D11_TEXTURE2D_DESC));
	tex_2d.Width = swap_chain_desc_.BufferDesc.Width;
	tex_2d.Height = swap_chain_desc_.BufferDesc.Height;
	tex_2d.ArraySize = 1;
	tex_2d.MipLevels = 1;
	tex_2d.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex_2d.SampleDesc.Count = 1;
	tex_2d.SampleDesc.Quality = 0;
	tex_2d.Usage = D3D11_USAGE_DEFAULT;
	tex_2d.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = device_->CreateTexture2D(&tex_2d, NULL, texture.GetAddressOf());
	if (FAILED(hr))
		assert(false);

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = device_->CreateDepthStencilView(texture.Get(), &depth_stencil_desc, &depth_stencil_view_);
	immediate_device_context_->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_);


	//viewport 생성
	viewport_.Width = client_width;
	viewport_.Height = client_height;
	viewport_.MinDepth = 0;
	viewport_.MaxDepth = 1;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	immediate_device_context_->RSSetViewports(1, &viewport_); //래스터라이저에서 화면의 구역을 설정
						//래스터라이저는 뷰포트 영역 내의 표현할 픽셀을 찾아서 PS로 보내준다. 
	g_rectangle_client.right = swap_chain_desc_.BufferDesc.Width;
	g_rectangle_client.bottom = swap_chain_desc_.BufferDesc.Height;


	return true;
}



bool PDevice::DeviceRelease()
{
	if (depth_stencil_view_) depth_stencil_view_->Release();
	if (render_target_view_) render_target_view_->Release();
	if (swap_chain_) swap_chain_->Release();
	if (immediate_device_context_) immediate_device_context_->Release();
	if (device_) device_->Release();
	
	depth_stencil_view_ = nullptr;
	render_target_view_ = nullptr;
	swap_chain_ = nullptr;
	immediate_device_context_ = nullptr;
	device_ = nullptr;


	

	return true;
}

ID3D11Device* PDevice::device()
{
	return device_;
}

ID3D11DeviceContext* PDevice::immediate_device_context()
{
	return immediate_device_context_;
}



bool PDevice::DevicePreRender()
{
	float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	//float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	immediate_device_context_->ClearRenderTargetView(render_target_view_, clearColor);
	immediate_device_context_->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);
	return true;
}
//
//bool PDevice::DeviceRender()
//{
//
//	immediate_device_context_->IASetInputLayout(input_layout_);
//	immediate_device_context_->VSSetShader(vertex_shader_, NULL, 0);
//	immediate_device_context_->PSSetShader(pixel_shader_, NULL, 0);
//
//
//	UINT stride = sizeof(PVertexAndUV);
//	UINT offset = 0;
//
//
//	immediate_device_context_->IASetVertexBuffers(0, 1, &vertex_buffer_, &stride, &offset);
//	immediate_device_context_->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);
//	immediate_device_context_->VSSetConstantBuffers(0, 1, &constant_buffer_);
//	immediate_device_context_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	immediate_device_context_->PSSetShaderResources(0, 1, &shader_res_view_);
//	immediate_device_context_->RSSetState(state_solid_frame_);
//
//
//	//immediate_device_context_->Draw(3, 0);
//	immediate_device_context_->DrawIndexed(6, 0, 0);
//	swap_chain_->Present(0, 0);
//	return true;
//}
//
