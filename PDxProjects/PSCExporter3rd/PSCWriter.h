#pragma once
#include <string>
#include <vector>
#include <fstream>

#define OUT_
using namespace std;


typedef struct _D3D_MATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;
		};
		float m[4][4];
	};
}D3D_MATRIX;
//vertex
struct PNCT
{
	Point3 p;
	Point3 n;
	Point4 c;
	Point2 t;

};
//face
struct TriComponent
{
	int tri_index;
	PNCT  v[3];
};

struct PTextureMap
{
	int map_id;
	TSTR name;
};

/*마테리얼 */
struct PMtl
{
	int mtl_id;
	TSTR name;
	std::vector<PTextureMap> tex_list;
	std::vector<PMtl> submaterial_list;

};

/*tick : 해당 애니메이션 위치/회전 당시의 tick값. (160틱의 x,y,z  320틱의 x,y,z....)
  Point3 p : 위치
  Quat q : 회전
*/
struct PAnimTrack
{
	int tick;
	Point3 p;
	Quat q;
};

struct PMesh
{
	TSTR name;
	TSTR parent_name;
	Matrix3 world_tm;
	//GetNodeTm(Parent 포함)
	D3D_MATRIX world_d3d;
	vector<TriComponent> tri_list; //dummy code?
	vector<vector<TriComponent>> buffer_list;
	int material_id;
	int numberof_submesh;

	vector<vector<PNCT>> vertex_list;
	vector<vector<int>> index_list;

	bool animation_enable[3]; //translate, rotate, scale 애니메이션 존재여부
	std::vector<PAnimTrack> anim_pos;
	std::vector<PAnimTrack> anim_rot;
	std::vector<PAnimTrack> anim_scale;


	PMesh()
	{
		name = L"none";
		parent_name = L"none";
		material_id = -1;
		numberof_submesh = 1;

	}

};


struct PScene
{
	int first_frame; // 0
	int last_frame; // 100
	int frame_rate; // 30
	int tick_per_frame; // 160
	int numberof_meshes;
	int numberof_materials;
};


class PSCWriter
{
public:
	PSCWriter();
	~PSCWriter();

protected:
	FILE* file;
	Interface*			interface_max_;
	std::wstring		filename_;
	INode*				rootnode_;
	PScene				scene_;


	std::vector<INode*> object_list_;
	std::vector<PMesh> mesh_list_;

	std::vector<Mtl*> material_list_;
	std::vector<PMtl> pmtl_list_;

	Interval interval_;

public:
	void Set(const TCHAR* name, Interface* interface_max);
	bool Export();
	void PreProcess(INode* node);
	void AddObject(INode* node);
	void GetMesh(INode* node, OUT_  PMesh& pmesh);
	TriObject* GetTriObjectFromNode(INode* node, TimeValue timeval, bool& deleteit);
	bool IsTmNegativeParity(Matrix3 tm);
	Point3 GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex);
	void GetTexture(PMtl& pmtl, Mtl* mtl);
	void CopyPoint3(Point3& dest, Point3& src);
	void GetMaterial(INode* node);
	void AddMaterial(INode* node);
	int FindMaterialIndex(INode* node);
	Mtl* FindMaterial(INode* node);
	TCHAR* FixupName(MSTR name);
	bool SwitchAllNodeToMesh(std::vector<INode*>& object_list, std::vector<PMesh>& mesh_list);

	bool EqualPoint2(Point2 p1, Point2 p2);
	bool EqualPoint3(Point3 p1, Point3 p2);
	bool EqualPoint4(Point4 pq, Point4 p2);
	int IsEqualVertexAndVertexList(PNCT& vertex, std::vector<PNCT>& vertex_list);
	void CopyMatrix3(OUT_ D3D_MATRIX& d3d_world, Matrix3& matWorld);
	void SetUniqueBuffer(PMesh& mesh);

	void GetAnimation(INode* node, PMesh& mesh);
	void ExportAnimation(PMesh& mesh);


};

