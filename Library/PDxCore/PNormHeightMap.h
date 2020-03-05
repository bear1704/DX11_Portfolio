#pragma once
#include "PMap.h"
#include "PNormalMapHelper.h"
#include "PLightObj.h"

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
	PLightObj* light_obj_;

public:
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* context, PLightObj* obj);
	bool Frame(D3DXVECTOR3 light_dir, D3DXVECTOR3 camera_position, D3DXVECTOR3 vec_look);
	bool PreRender() override;
	bool Render() override;
	bool UpdateBuffer();
	HRESULT CreateInputLayout() override;
	HRESULT CreateVertexBuffer() override;
	HRESULT CreateResource();
	//HRESULT CreateConstantBuffer() override;
	void SetWVPMatrix(D3DXMATRIX* world = nullptr, D3DXMATRIX* view = nullptr, D3DXMATRIX* proj = nullptr) override;
	void SetNormalTexture(std::wstring tex_path);

public:
	//getter
	const D3DXMATRIX& ref_mat_normal();
	Microsoft::WRL::ComPtr<ID3D11Buffer>* tangent_space_vbuffer();
	PTexture* normal_texture();

//	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_changes_everyframe_;
//	CB_VS_NearlyNotChange cb_nearly_not_changes_;
//	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_nearly_not_changes_;

};

