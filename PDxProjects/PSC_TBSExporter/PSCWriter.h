#pragma once
#include "PExpStd.h"

class PSCWriter
{
public:
	PSCWriter();
	virtual ~PSCWriter();

public:
	static PSCWriter& GetInstance()
	{
		static PSCWriter exp;
		return exp;
	}

public:
	FILE*				file;
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
	virtual void Set(const TCHAR* name, Interface* interface_max);
	virtual bool Export();
	virtual void PreProcess(INode* node);
	void AddObject(INode* node);
	virtual void GetMesh(INode* node, OUT_  PMesh& pmesh);
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
	virtual bool SwitchAllNodeToMesh(std::vector<INode*>& object_list, std::vector<PMesh>& mesh_list);

	bool EqualPoint2(Point2 p1, Point2 p2);
	bool EqualPoint3(Point3 p1, Point3 p2);
	bool EqualPoint4(Point4 pq, Point4 p2);
	virtual int IsEqualVertexAndVertexList(PNCT& vertex, std::vector<PNCT>& vertex_list);
	void CopyMatrix3(OUT_ D3D_MATRIX& d3d_world, Matrix3& matWorld);
	virtual void SetUniqueBuffer(PMesh& mesh);

	void GetAnimation(INode* node, PMesh& mesh);
	void ExportAnimation(PMesh& mesh);

	TCHAR* SaveFileDialog(TCHAR* extension, TCHAR* title);

};

