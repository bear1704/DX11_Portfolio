#pragma once
#include <assert.h>
#include "PWindow.h"
#include "PStd.h"
#include <memory>
#include <d3d11.h>
#include <d3dx11.h>
#include <wrl.h>



#pragma comment (lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#define OUT_  

class PDevice : public PWindow
{
public:
	PDevice();
	~PDevice();

protected:
	
	DXGI_SWAP_CHAIN_DESC	swap_chain_desc_;
	D3D_DRIVER_TYPE			driver_type_;
	D3D_FEATURE_LEVEL		feature_level_;
	IDXGISwapChain*			swap_chain_;
	ID3D11Device*			device_;
	ID3D11DeviceContext*	immediate_device_context_;
	ID3D11RenderTargetView* render_target_view_;
	ID3D11DepthStencilView* depth_stencil_view_;
	D3D11_VIEWPORT			viewport_;

	D3D_DRIVER_TYPE driver_types[3];
	D3D_FEATURE_LEVEL feature_levels[2];

	ID3D11RasterizerState* state_solid_frame_;



public:
	bool		InitDevice(HWND hwnd, UINT client_width, UINT client_height);
	bool		CreateSwapChain(HWND hwnd, UINT client_width, UINT client_height);
	bool		CreateRenderTarget(UINT client_width, UINT client_height);
	
	bool		DevicePreRender();
	//bool		DeviceRender();
	bool		DeviceRelease();

	
	ID3D11Device* device();
	ID3D11DeviceContext* immediate_device_context();




};

