#pragma once
#include "PTextureManager.h"	
#include "PShape.h"

const int kMaxSkyboxTextureCount = 6;

class PSkyBox : public PBoxObject
{
public:
	PSkyBox();
	virtual ~PSkyBox();

public:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_skybox_texture_[kMaxSkyboxTextureCount];

public:
	//virtual bool Create(ID3D11Device* device, std::wstring shader_file_path, std::wstring );
	virtual bool PostRender() override;
	virtual bool Render() override;
	virtual HRESULT CreateIndexData() override;

	virtual HRESULT LoadTextures(std::wstring tex_name = nullptr) override;

};

