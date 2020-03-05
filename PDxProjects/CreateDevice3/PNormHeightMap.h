#pragma once
#include "PMap.h"
#include "PNormalMapHelper.h"

class PNormHeightMap :
	public PHeightMap
{
public:
	PNormHeightMap();
	~PNormHeightMap();

public:
	std::vector<D3DXVECTOR3>			 tangent_list_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> tangent_space_vbuffer_;
	PNormalMapHelper					 helper_normalmap_;
	D3DXMATRIX							 mat_normal_; //??
	PTexture* normal_texture_;
	

public:
	bool Render() override;
	bool UpdateBuffer();
	HRESULT CreateInputLayout() override;
	HRESULT CreateVertexBuffer() override;
	HRESULT CreateResource();
	void SetWVPMatrix(D3DXMATRIX* world = nullptr, D3DXMATRIX* view = nullptr, D3DXMATRIX* proj = nullptr) override;
	void SetNormalTexture(std::wstring tex_path);

public:
	//getter
	const D3DXMATRIX& ref_mat_normal();
	Microsoft::WRL::ComPtr<ID3D11Buffer>* tangent_space_vbuffer();
	PTexture* normal_texture();

};

