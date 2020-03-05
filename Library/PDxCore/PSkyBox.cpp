#include "PSkyBox.h"


PSkyBox::PSkyBox()
{
}

PSkyBox::~PSkyBox()
{
}

bool PSkyBox::PostRender()
{
	DX::ApplySamplerState(immediate_context_, DX::PDxState::sampler_state_wrap_point);

	immediate_context_->PSSetShaderResources(1, 6, srv_skybox_texture_[0].GetAddressOf());
	immediate_context_->DrawIndexed(36, 0, 0);
	DX::ApplySamplerState(immediate_context_, DX::PDxState::sampler_state_anisotropic);

	return true;
}

bool PSkyBox::Render()
{
	PModel::Render();
	return true;
}

HRESULT PSkyBox::LoadTextures(std::wstring tex_name)
{

	//tex_name 의미없음

	PTextureManager::GetInstance().LoadTextureFromScript(L"data/skybox.txt", device_);
	PTexture* texture;

	std::wstring list[kMaxSkyboxTextureCount] =
	{
		L"ft",
		L"bk",
		L"rt",
		L"lt",
		L"up",
		L"dn"
	};

	
	for (int i = 0; i < kMaxSkyboxTextureCount; i++)
	{
		texture = PTextureManager::GetInstance().GetTextureFromMap(list[i]);
		srv_skybox_texture_[i].Attach(texture->shader_res_view());
	}

	if (texture_ != nullptr)
		dx_helper_.shader_res_view_.Attach(texture_->shader_res_view());

	return S_OK;
}

HRESULT PSkyBox::CreateIndexData()
{
	index_list_.resize(36);
	int iIndex = 0;
	index_list_[iIndex++] = 0; 	index_list_[iIndex++] = 2; 	index_list_[iIndex++] = 1; 	index_list_[iIndex++] = 0;	index_list_[iIndex++] = 3; 	index_list_[iIndex++] = 2;
	index_list_[iIndex++] = 4; 	index_list_[iIndex++] = 6; 	index_list_[iIndex++] = 5; 	index_list_[iIndex++] = 4;	index_list_[iIndex++] = 7; 	index_list_[iIndex++] = 6;
	index_list_[iIndex++] = 8; 	index_list_[iIndex++] = 10;	index_list_[iIndex++] = 9;  index_list_[iIndex++] = 8;	index_list_[iIndex++] = 11; index_list_[iIndex++] = 10;
	index_list_[iIndex++] = 12; index_list_[iIndex++] = 14; index_list_[iIndex++] = 13; index_list_[iIndex++] = 12;	index_list_[iIndex++] = 15; index_list_[iIndex++] = 14;
	index_list_[iIndex++] = 16; index_list_[iIndex++] = 18; index_list_[iIndex++] = 17; index_list_[iIndex++] = 16;	index_list_[iIndex++] = 19; index_list_[iIndex++] = 18;
	index_list_[iIndex++] = 20; index_list_[iIndex++] = 22; index_list_[iIndex++] = 21; index_list_[iIndex++] = 20;	index_list_[iIndex++] = 23; index_list_[iIndex++] = 22;

	return S_OK;
}