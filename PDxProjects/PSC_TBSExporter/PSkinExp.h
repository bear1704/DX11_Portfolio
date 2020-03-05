#pragma once
#include "PSCWriter.h"

struct PNCTWI
{
	Point3 p;
	Point3 n;
	Point4 c;
	Point2 t;
	float index1[4];
	float weight1[4];
	float index2[4];
	float weight2[4];
};

struct PBipedVertex
{
	int numberof_weight;
	//정점의 애니메이션에 영향주는 노드의 인덱스들
	std::vector<BYTE> index_list;
	//각 인덱스별 가중치값
	std::vector<float> weight_list;
};

struct BipedTricomponent
{
	int tri_index;
	PNCTWI v[3];
};


struct PBipedMesh
{
	INode* node;

	TSTR name;
	TSTR parent_name;
	Matrix3 world_tm;
	D3D_MATRIX world_d3d;
	D3D_MATRIX world_d3d_inv;
	vector<BipedTricomponent> tri_list; //dummy code?
	vector<vector<BipedTricomponent>> buffer_list;
	vector<PBipedVertex> biped_list;
	
	int material_id;
	int numberof_submesh;

	vector<vector<PNCTWI>> vertex_list;
	vector<vector<int>> index_list;

	bool animation_enable[3]; //translate, rotate, scale 애니메이션 존재여부
	std::vector<PAnimTrack> anim_pos;
	std::vector<PAnimTrack> anim_rot;
	std::vector<PAnimTrack> anim_scale;

	PBipedMesh()
	{
		name = L"none";
		parent_name = L"none";
		material_id = -1;
		numberof_submesh = 1;

	}

};

class PSkinExp : public PSCWriter
{
private:
	PSkinExp();
	std::vector<PBipedMesh> biped_mesh_list_;

public:
	virtual ~PSkinExp();

	static PSkinExp& GetInstance()
	{
		static PSkinExp exp;
		return exp;
	}

public:
	void Set(const TCHAR* name, Interface* interface_max) override;
	bool Export() override;
	void PreProcess(INode* node) override;
	void GetMesh(INode* node, OUT_  PBipedMesh& pmesh);
	bool SwitchAllNodeToMesh();
	int IsEqualVertexAndVertexList(PNCTWI& vertex, std::vector<PNCTWI>& vertex_list);
	void SetUniqueBuffer(PBipedMesh& mesh);
public:
	void SetBipedInfo(INode* node, PBipedMesh& bmesh);
	Modifier* FindModifier(INode* node, Class_ID classid);
	void ExportPhysiqueData(INode* node, Modifier* modi, PBipedMesh& mesh);
	void ExportSkinData(INode* node, Modifier* modi, PBipedMesh& mesh);
	int GetFindIndex(INode* node);

};


