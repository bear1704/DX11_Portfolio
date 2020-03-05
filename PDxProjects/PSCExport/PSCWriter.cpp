#include "pch.h"
#include "PSCWriter.h"

PSCWriter::PSCWriter()
{
}

PSCWriter::~PSCWriter()
{
}

void PSCWriter::Set(const TCHAR* name, Interface* interface_max)
{
	interface_max_ = interface_max;
	filename_ = name;
	rootnode_ = interface_max_->GetRootNode();
	PreProcess(rootnode_);
}

bool PSCWriter::Export()
{
	FILE* file = nullptr;
	_wfopen_s(&file, filename_.c_str(), _T("wb"));
	_ftprintf(file, _T("%s %d"), _T("PSCExporter100"), object_list_.size());

	for (int i = 0; i < object_list_.size(); i++)
	{
		INode* node = object_list_[i];

		GetMesh(node);
		_ftprintf(file, _T("\n%s %d"), node->GetName(),
			tri_list_.size());

		for (int iTri = 0; iTri < tri_list_.size(); iTri++)
		{
			for (int iVer = 0; iVer < 3; iVer++)
			{
				_ftprintf(file, _T("\n%10.4f %10.4f %10.4f"),
					tri_list_[iTri].v[iVer].p.x,
					tri_list_[iTri].v[iVer].p.y,
					tri_list_[iTri].v[iVer].p.z);

				_ftprintf(file, _T("\n%10.4f %10.4f %10.4f"),
					tri_list_[iTri].v[iVer].n.x,
					tri_list_[iTri].v[iVer].n.y,
					tri_list_[iTri].v[iVer].n.z);

				_ftprintf(file, _T("\n%10.4f %10.4f %10.4f"),
					tri_list_[iTri].v[iVer].c.x,
					tri_list_[iTri].v[iVer].c.y,
					tri_list_[iTri].v[iVer].c.z);

				_ftprintf(file, _T("\n%10.4f %10.4f %10.4f"),
					tri_list_[iTri].v[iVer].t.x,
					tri_list_[iTri].v[iVer].t.y);

			}
		}
	}
	
	fclose(file);

	MessageBox(GetActiveWindow(), filename_.c_str(),
		_T("Succeed!"), MB_OK);

	return true;
}
   
void PSCWriter::PreProcess(INode* node)
{
	if (node == NULL) return;
	AddObject(node);
	int numberof_children = node->NumberOfChildren();
	for (int i = 0; i < numberof_children; i++)
	{
		INode* child = node->GetChildNode(i);
		PreProcess(child);
	}

}

void PSCWriter::AddObject(INode* node)
{
	ObjectState os = node->EvalWorldState(0);

	if (!os.obj)
		return;

	if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
		return;
	else
	{
		switch (os.obj->SuperClassID())
		{
			case GEOMOBJECT_CLASS_ID:
				object_list_.push_back(node);
				break;
			default:
				break;
		}
	}

}

void PSCWriter::GetMesh(INode* node)
{
	Matrix3 tm = node->GetObjTMAfterWSM(0);
	
	bool deleteit = false;
	TriObject* triobj = GetTriObjectFromNode(node, 0, deleteit);
	
	if (!triobj) return;

	Mesh* mesh = &triobj->GetMesh();
	bool is_negscale = IsTmNegativeParity(tm);
	int custom_v0, custom_v1, custom_v2;

	if (is_negscale) { custom_v0 = 2, custom_v1 = 1; custom_v2 = 0; }
	else { custom_v0 = 0; custom_v1 = 1; custom_v2 = 2; }

	if (mesh)
	{
		int numberof_face = mesh->getNumFaces();
		tri_list_.resize(numberof_face);

		for (int iFace = 0; iFace < numberof_face; iFace++)
		{
			int numberof_vert = mesh->getNumVerts();

			if (numberof_vert > 0)
			{
				Point3 v = mesh->verts[mesh->faces[iFace].v[custom_v0]] * tm;
				CopyPoint3(tri_list_[iFace].v[0].p, v);
				
				v = mesh->verts[mesh->faces[iFace].v[custom_v1]] * tm;
				CopyPoint3(tri_list_[iFace].v[1].p, v);

				v = mesh->verts[mesh->faces[iFace].v[custom_v2]] * tm;
				CopyPoint3(tri_list_[iFace].v[2].p, v);
			}

			int numberof_color = mesh->getNumVertCol();
			tri_list_[iFace].v[0].c = Point4(1, 1, 1, 1);
			tri_list_[iFace].v[1].c = Point4(1, 1, 1, 1);
			tri_list_[iFace].v[2].c = Point4(1, 1, 1, 1);
			if (numberof_color > 0)
			{
				tri_list_[iFace].v[0].c = mesh->vertCol[mesh->vcFace[iFace].t[custom_v0]];
				tri_list_[iFace].v[0].c = mesh->vertCol[mesh->vcFace[iFace].t[custom_v2]];
				tri_list_[iFace].v[0].c = mesh->vertCol[mesh->vcFace[iFace].t[custom_v1]];
			}

			int numberof_tex = mesh->getNumTVerts();
			if (numberof_tex)
			{
				Point2 uv = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[custom_v0]];
				tri_list_[iFace].v[0].t.x = uv.x;
				tri_list_[iFace].v[0].t.y = 1.0f - uv.y;

				uv = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[custom_v2]];
				tri_list_[iFace].v[1].t.x = uv.x;
				tri_list_[iFace].v[1].t.y = 1.0f - uv.y;
				
				uv = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[custom_v1]];
				tri_list_[iFace].v[2].t.x = uv.x;
				tri_list_[iFace].v[2].t.y = 1.0f - uv.y;
			}

			mesh->buildNormals();
			int vert = mesh->faces[iFace].getVert(custom_v0);
			RVertex* rvertex = mesh->getRVertPtr(vert);
			Point3 vn = GetVertexNormal(mesh, iFace, rvertex);
			CopyPoint3(tri_list_[iFace].v[custom_v0].n, vn);

			vert = mesh->faces[iFace].getVert(custom_v1);
			rvertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rvertex);
			CopyPoint3(tri_list_[iFace].v[custom_v1].n, vn);

			vert = mesh->faces[iFace].getVert(custom_v2);
			rvertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rvertex);
			CopyPoint3(tri_list_[iFace].v[custom_v2].n, vn);


		}

	}

	if (deleteit) delete triobj;

}

TriObject* PSCWriter::GetTriObjectFromNode(INode* node, TimeValue timeval, bool& deleteit)
{
	Object* obj = node->EvalWorldState(timeval).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
	{
		TriObject* tri = (TriObject*)obj->ConvertToType(timeval, Class_ID(TRIOBJ_CLASS_ID, 0));
		if (obj != tri) deleteit = true;
		return tri;
	}

	return nullptr;
}

bool PSCWriter::IsTmNegativeParity(Matrix3 tm)
{
	Point3 v0 = tm.GetRow(0);
	Point3 v1 = tm.GetRow(1);
	Point3 v2 = tm.GetRow(2);
	Point3 vec_cross = CrossProd(v0, v1);

	return (DotProd(vec_cross, v2) < 0.0f) ? true : false;
}

Point3 PSCWriter::GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex)
{
	Face* face = &mesh->faces[iFace];
	int smoothing_group = face->smGroup;

	int numberof_normals = rVertex->rFlags & NORCT_MASK;

	Point3 vertex_normal;
	if (rVertex->rFlags & SPECIFIED_NORMAL)
	{
		vertex_normal = rVertex->rn.getNormal();
	}
	else if (numberof_normals & smoothing_group)
	{
		if (numberof_normals == 1)
		{
			vertex_normal = rVertex->rn.getNormal();
		}
		else
		{
			for (int i = 0; i < numberof_normals; i++)
			{
				if (rVertex->ern[i].getSmGroup() & smoothing_group)
				{
					vertex_normal = rVertex->ern[i].getNormal();
				}
			}
		}
	}
	else
		vertex_normal = mesh->getFaceNormal(iFace);

	return vertex_normal;

	return Point3();
}

void PSCWriter::GetMaterial(INode* node)
{
	Mtl* mtl = node->GetMtl();
	GetTexture(mtl);
}

void PSCWriter::GetTexture(Mtl* mtl)
{
	int numberof_map = mtl->NumSubTexmaps();

	for (int i = 0; i < numberof_map; i++)
	{
		Texmap* tex = mtl->GetSubTexmap(i);

		if (!tex) break;

		if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0X00))
		{
			PMtl mtl;
			mtl.mtl_id = i;

			TSTR full_name;
			TSTR map_name = ((BitmapTex*)(tex))->GetMapName();
			SplitPathFile(map_name, &full_name, &mtl.name);
			pmaterial_list_.push_back(mtl);
		}


	}

}

void PSCWriter::CopyPoint3(Point3& dest, Point3& src)
{
	dest.x = src.x;
	dest.y = src.z;
	dest.z = src.y;
}
