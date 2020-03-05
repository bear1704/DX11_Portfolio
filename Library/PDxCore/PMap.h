#pragma once
#include "PModel.h"

struct PMapDesc
{
	int vertex_rows;
	int vertex_cols;
	float cell_disatnce;
	std::wstring texture_name;
	std::wstring vs_path = L"";
	std::string vs_func = "";
	std::wstring ps_path = L"";
	std::string ps_func = "";
};


class PMap : public PModel
{
protected:
	int vertex_rows_;
	int vertex_cols_;
	int cell_rows_;
	int cell_cols_;
	int numberof_faces_;
	int numberof_vertices_;
	float cell_distance_;
	std::vector<D3DXVECTOR3> face_normals_;
public:
	virtual HRESULT CreateVertexData() override;
	virtual HRESULT CreateIndexData() override;
	virtual bool Load(PMapDesc& md);
	int vertex_rows();
	int vertex_cols();
	void InitFaceNormals();
	D3DXVECTOR3 ComputeFaceNormal(int idx0, int idx1, int idx2);
	void CalcFaceNormals();

public:
	PMap();
	virtual ~PMap();


};

class PHeightMap : public PMap
{
private:
	const int kReductionRatio = 5.0f;
	vector<float> heightmap_tex_list_;

public:
	inline float GetHeightMapList(int index)
	{
		return heightmap_tex_list_[index] / kReductionRatio;
	}

	bool CreateHeightMap(ID3D11Device* device, ID3D11DeviceContext* context, 
		const std::wstring map_file_path);

public:
	HRESULT CreateVertexData() override;

public:
	PHeightMap();
	virtual ~PHeightMap();

};