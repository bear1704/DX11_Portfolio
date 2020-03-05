#pragma once
#include <string>
#include <vector>

#define OUT_
using namespace std;

struct PNCT
{
	Point3 p;
	Point3 n;
	Point4 c;
	Point2 t;

};

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

struct PMtl
{
	int mtl_id;
	TSTR name;
	std::vector<PTextureMap> tex_list;
	std::vector<PMtl> submaterial_list;

};


struct PMesh
{
	TSTR name;
	vector<TriComponent> tri_list; //dummy code?
	vector<vector<TriComponent>> buffer_list;
	int material_id;
};


class PSCWriter
{
public:
	PSCWriter();
	~PSCWriter();

protected:
	Interface*			interface_max_;
	std::wstring		filename_;
	INode*				rootnode_;
	std::vector<INode*> object_list_;
	std::vector<PMesh> mesh_list_;

	std::vector<Mtl*> material_list_;
	std::vector<PMtl> pmtl_list_;


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
	


};

