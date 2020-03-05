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