#include "PDxRenderTarget.h"

HRESULT PDxRenderTarget::Create(ID3D11Device* device, float width, float height)
{
	D3D11_TEXTURE2D_DESC td; //ÅØ½ºÃÄ »ý¼º
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; //·»´õÅ¸°Ù 
	td.ArraySize = 1;

	device->CreateTexture2D(&td, NULL, texture_.GetAddressOf());
	device->CreateShaderResourceView(texture_.Get(), NULL, shader_res_view_.GetAddressOf());
	device->CreateRenderTargetView(texture_.Get(), NULL, render_target_view_.GetAddressOf());

	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	device->CreateTexture2D(&td, NULL, depth_stencil_texture_.GetAddressOf());

	D3D11_DEPTH_STENCIL_VIEW_DESC ds_view_desc;
	ZeroMemory(&ds_view_desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	ds_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	
	device->CreateDepthStencilView(
		depth_stencil_texture_.Get(), &ds_view_desc,
		depth_stencil_view_.GetAddressOf());

	viewport_.Height = height;
	viewport_.Width = width;
	viewport_.MaxDepth = 1;
	viewport_.MinDepth = 0;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;

	return S_OK;
}

bool PDxRenderTarget::Begin(ID3D11DeviceContext* context)
{
	numberof_view_port_ = 1;
	context->RSGetViewports(&numberof_view_port_, &viewport_old_);
	context->OMGetRenderTargets(1, render_target_view_old_.GetAddressOf(),
		depth_stencil_view_old_.GetAddressOf());

	context->ClearRenderTargetView(
		render_target_view_.Get(), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));

	context->ClearDepthStencilView(
		depth_stencil_view_.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	ID3D11RenderTargetView* rtvs = NULL;
	ID3D11DepthStencilView* dsv = NULL;
	context->OMSetRenderTargets(1, &rtvs, dsv);

	context->OMSetRenderTargets(1,
		render_target_view_.GetAddressOf(),
		depth_stencil_view_.Get());

	context->RSSetViewports(1, &viewport_);


	return true;
}

bool PDxRenderTarget::End(ID3D11DeviceContext* context)
{


	ID3D11RenderTargetView* rtvs = NULL;
	ID3D11DepthStencilView* dsv = NULL;
	context->OMSetRenderTargets(1, &rtvs, dsv);

	context->RSSetViewports(numberof_view_port_, &viewport_old_);
	context->OMSetRenderTargets(1, render_target_view_old_.GetAddressOf(),
		depth_stencil_view_old_.Get());

	render_target_view_old_.Reset();
	depth_stencil_view_old_.Reset();
	
	return true;
}

bool PDxRenderTarget::Release()
{
	return true;
}

PDxRenderTarget::PDxRenderTarget()
{
}

PDxRenderTarget::~PDxRenderTarget()
{
}
