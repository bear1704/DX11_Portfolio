#pragma once
#include "PCore.h"
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")


#if defined(DEBUG) || defined(_DEBUG)
#pragma comment (lib, "PDxCoreLib64_D.lib")
#else
#pragma comment (lib, "PDxCoreLib64_R.lib")
#endif

#pragma comment (lib, "fmod_vc.lib")
#pragma comment (lib, "winmm.lib")


//PScene* g_current_scene_;

class Sample : public PCore
{
public:
	DXGI_SWAP_CHAIN_DESC* pSwapChainDesc;

public:
	Sample();
	~Sample();

public:
	//D3D_DRIVER_TYPE driver_type_;
	//DXGI_SWAP_CHAIN_DESC desc_swap_chain_;
	//D3D_FEATURE_LEVEL feature_level_;
	//IDXGISwapChain* swap_chain_;
	//ID3D11Device* device_;
	//ID3D11DeviceContext* immediate_device_context_;
	//ID3D11RenderTargetView* render_target_view_;
	//D3D11_VIEWPORT viewport_;

public:
	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;


};
PCORE_RUN(L"CreateDevice", 0, 0, 1024, 768);
