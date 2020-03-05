#pragma once
#include "PDxHelper.h"

class PDxRenderTarget
{
public:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_res_view_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_texture_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view_;

	D3D11_VIEWPORT viewport_;
	D3D11_VIEWPORT viewport_old_;

	UINT numberof_view_port_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_old_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view_old_;

public:
	HRESULT Create(ID3D11Device* device, float width, float height);
	bool Begin(ID3D11DeviceContext* context);
	bool End(ID3D11DeviceContext* context);
	bool Release();

public:
	PDxRenderTarget();
	virtual ~PDxRenderTarget();

};

