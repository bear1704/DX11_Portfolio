#pragma once
#include "PModel.h"
#include "PLightObj.h"

#define MAX_BONE 255

struct CB_BoneAnimation
{
	D3DXMATRIX g_matrix[MAX_BONE];
};


class PSkmObj : public PModel
{
private:
	PParser parse;
	
public:
	PSkmObj();
	virtual ~PSkmObj();

public:
	std::vector<PSkinMesh> object_list_;
	std::vector<Material> material_list_;
	std::vector<D3DXMATRIX> bone_list_;
	std::vector<Vertex_PNCTW8I8> vertex_list_;
	float elapsed_time_;
	MaxScene scene_;
	PLightObj* light_obj_;
public:
	D3DXMATRIX bone_matrix[MAX_BONE];
	CB_BoneAnimation cb_bonedata_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cbuffer_bone_anim_;



public:
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* context,
		std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name,
		std::wstring object_path = L"", std::wstring texcomp_path = L"") override;
	virtual bool Frame() override;
	virtual HRESULT CreateVertexBuffer()	override;
	virtual HRESULT CreateIndexBuffer()		override;
	virtual HRESULT CreateVertexData()		override;
	virtual HRESULT CreateIndexData()		override;
	virtual HRESULT CreateConstantBuffer()	override;
	virtual HRESULT CreateInputLayout()		override;
	virtual bool PreRender()				override;
	virtual bool PostRender()				override;

public:
	bool GetAnimationTrack(const float elapsed_time, const std::vector<PAnimTrack>& tracklist,
		OUT_ PAnimTrack& start_track, OUT_ PAnimTrack& end_track);
	void Interpolate(PSkinMesh& mesh, D3DXMATRIX& mat_parent, float elapsed_time);
	/* 파일이 index와 weight를 가지지 않을 때, 출력을 위해 보정해주는 함수 */
	void ReviseNotUseIdxWeightVert(std::vector <Vertex_PNCTW8I8>& vert);

};

