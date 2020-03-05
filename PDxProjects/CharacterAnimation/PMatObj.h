#pragma once
#include "PModel.h"

class PMatObj : public PModel
{

public:
	PMatObj();
	virtual ~PMatObj();
public:
	std::vector<PGeoMesh> object_list_;
	std::vector<Material> material_list_;
	float elapsed_time_;
	MaxScene scene_;
	PTexture* texture_;
	std::map<string, PAction> action_animlist_; //애니메이션 액션 리스트(걷기, 뛰기 등..)
	D3DXMATRIX matrix_[255];

public:
	bool Init(ID3D11Device* device, ID3D11DeviceContext* context,
		std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name,
		std::wstring object_path = L"", std::wstring texcomp_path = L"");
	virtual bool Frame() override;
	virtual bool Frame(D3DXMATRIX* matrix) override;
	virtual D3DXMATRIX* FrameMatrix(int start_frame, int end_frame, float elapsed_time) override;
	virtual HRESULT CreateVertexBuffer() override;
	virtual HRESULT CreateIndexBuffer() override;
	virtual HRESULT CreateVertexData() override;
	virtual HRESULT CreateIndexData() override;
	virtual bool PostRender() override;

public:
	bool GetAnimationTrack(const float elapsed_time, const std::vector<PAnimTrack>& tracklist,
		OUT_ PAnimTrack& start_track, OUT_ PAnimTrack& end_track);
	void Interpolate(PGeoMesh& mesh, D3DXMATRIX& mat_parent, float elapsed_time);

};

