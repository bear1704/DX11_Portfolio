#pragma once
#include "PModel.h"
class PKgcObj :
	public PModel
{
public:
	PKgcObj();
	virtual ~PKgcObj();

public:
	std::vector<PGeoMesh> object_list_;
	std::vector<Material> material_list_;
	float elapsed_time_;
	MaxScene scene_;

public:
	bool Init(ID3D11Device* device, ID3D11DeviceContext* context,
		std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name,
		std::wstring object_path = L"", std::wstring texcomp_path = L"");
	virtual bool Frame() override;
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

