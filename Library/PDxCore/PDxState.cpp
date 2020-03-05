#include "PDxState.h"


namespace DX
{
	ID3D11BlendState* PDxState::blend_state_alphablend_ = 0;
	ID3D11BlendState* PDxState::blend_state_alphablend_disable_ = 0;
	ID3D11RasterizerState* PDxState::rs_state_wireframe_ = 0;
	ID3D11RasterizerState* PDxState::rs_state_solidframe_ = 0;
	ID3D11RasterizerState* PDxState::rs_state_nocull_ = 0;
	ID3D11SamplerState* PDxState::sampler_state_linear_filter = 0;
	ID3D11SamplerState* PDxState::sampler_state_anisotropic = 0;
	ID3D11SamplerState* PDxState::sampler_state_wrap_point = 0;
	ID3D11DepthStencilState* PDxState::depth_stencil_state_enable_ = 0;
	ID3D11DepthStencilState* PDxState::depth_stencil_state_disable_ = 0;
	ID3D11DepthStencilState* PDxState::depth_stencil_enable_nowrite_zbuffer = 0;
	

	void DX::PDxState::SetState(ID3D11Device* current_device)
	{
		if (!current_device) return;

		HRESULT hr;
		
#pragma region ID3D11DepthStencilState
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
		ZeroMemory(&depth_stencil_desc, sizeof(CD3D11_DEPTH_STENCIL_DESC));
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		current_device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state_enable_);
		
		depth_stencil_desc.DepthEnable = FALSE;
		current_device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state_disable_);

		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		current_device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_enable_nowrite_zbuffer);

#pragma endregion



#pragma region BLEND
		D3D11_BLEND_DESC blend_desc;
		ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = TRUE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = current_device->CreateBlendState(&blend_desc, &blend_state_alphablend_);
		if (FAILED(hr))
			assert(false);

		blend_desc.RenderTarget[0].BlendEnable = FALSE;

		hr = current_device->CreateBlendState(&blend_desc, &blend_state_alphablend_disable_);
		if (FAILED(hr))
			assert(false);

		

		
#pragma endregion	BLEND

#pragma region Rasterizer

		D3D11_RASTERIZER_DESC raster_desc;
		ZeroMemory(&raster_desc, sizeof(raster_desc));
		raster_desc.DepthClipEnable = TRUE;
		raster_desc.FillMode = D3D11_FILL_WIREFRAME;
		raster_desc.CullMode = D3D11_CULL_BACK;
		raster_desc.MultisampleEnable = TRUE;
		raster_desc.AntialiasedLineEnable = TRUE;

		hr = current_device->CreateRasterizerState(&raster_desc, &rs_state_wireframe_);
		if (FAILED(hr))
			return;

		raster_desc.FillMode = D3D11_FILL_SOLID;

		hr = current_device->CreateRasterizerState(&raster_desc, &rs_state_solidframe_);
	
		raster_desc.CullMode = D3D11_CULL_NONE;
		hr = current_device->CreateRasterizerState(&raster_desc, &rs_state_nocull_);

		if (FAILED(hr))
			return;


#pragma endregion Rasterizer
	


#pragma region ID3D11SamplerState

		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.MaxLOD = FLT_MAX;
		sd.MinLOD = FLT_MIN;
		sd.MaxAnisotropy = 16;

		hr = current_device->CreateSamplerState(&sd, &sampler_state_linear_filter);
		if (FAILED(hr))
			assert(false);

		sd.Filter = D3D11_FILTER_ANISOTROPIC;
		hr = current_device->CreateSamplerState(&sd, &sampler_state_anisotropic);
		if (FAILED(hr))
			assert(false);

		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		hr = current_device->CreateSamplerState(&sd, &sampler_state_wrap_point);
		if (FAILED(hr))
			assert(false);



#pragma endregin ID3D11SamplerState

	}

	void PDxState::Release()
	{
		if (blend_state_alphablend_)
			blend_state_alphablend_->Release();
		if (blend_state_alphablend_disable_)
			blend_state_alphablend_disable_->Release();

		if (rs_state_wireframe_)
			rs_state_wireframe_->Release();
		if (rs_state_solidframe_)
			rs_state_solidframe_->Release();

		if (depth_stencil_state_enable_)
			depth_stencil_state_enable_->Release();
		if (depth_stencil_state_disable_)
			depth_stencil_state_disable_->Release();

		if (sampler_state_anisotropic)
			sampler_state_anisotropic->Release();
		if (sampler_state_linear_filter)
			sampler_state_linear_filter->Release();
		if (sampler_state_wrap_point)
			sampler_state_wrap_point->Release();

		rs_state_solidframe_ = nullptr;
		rs_state_wireframe_ = nullptr;
		blend_state_alphablend_ = nullptr;
		blend_state_alphablend_disable_ = nullptr;
		depth_stencil_state_enable_ = nullptr;
		depth_stencil_state_disable_ = nullptr;
		sampler_state_anisotropic = nullptr;
		sampler_state_linear_filter = nullptr;
		sampler_state_wrap_point = nullptr;


	}

	DX::PDxState::PDxState()
	{
	}

	DX::PDxState::~PDxState()
	{
		Release();
	}
}
