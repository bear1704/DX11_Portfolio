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
	SwitchAllNodeToMesh(object_list_, mesh_list_);

	FILE* file = nullptr;
	_wfopen_s(&file, filename_.c_str(), _T("wb"));
	_ftprintf(file, _T("%s %d"), _T("PSCExporter100"), object_list_.size());

	_ftprintf(file, _T("\n%s"), L"#HEADER INFO");
	_ftprintf(file, _T("\n%d %d"), mesh_list_.size(), pmtl_list_.size());

	_ftprintf(file, _T("\n%s"), L"#METERIAL INFO");
	for (int i = 0; i < pmtl_list_.size(); i++)
	{
		_ftprintf(file, _T("\n%s %d"), pmtl_list_[i].name, pmtl_list_[i].submaterial_list.size());

		if (pmtl_list_[i].submaterial_list.size() > 0)
		{

			auto current_submaterial_list = pmtl_list_[i].submaterial_list;
			for (int isub = 0; isub < current_submaterial_list.size(); isub++)
			{
				_ftprintf(file, _T("\n%s %d"), pmtl_list_[i].submaterial_list[isub].name,
					pmtl_list_[i].submaterial_list[isub].tex_list.size());

				auto current_texlist = pmtl_list_[i].submaterial_list[isub].tex_list;
				for (int itex = 0; itex < current_texlist.size(); itex++)
				{
					_ftprintf(file, _T("\n%d %s"),
						current_texlist[i].map_id,
						current_texlist[i].name);
				}


			}
		}
		else
		{
			_ftprintf(file, _T("\n%s %d"),
				pmtl_list_[i].name,
				pmtl_list_[i].tex_list.size());

			for (int itex = 0; itex < pmtl_list_[i].tex_list.size(); itex++)
			{
				_ftprintf(file, _T("\n%d %s"),
					pmtl_list_[i].tex_list[itex].map_id,
					pmtl_list_[i].tex_list[itex].name);
			}
		}
	}

	//mesh list
	_ftprintf(file, _T("\n%s"), L"#OJBECT INFO");

	for (int imesh = 0; imesh < mesh_list_.size(); imesh++)
	{
		_ftprintf(file, _T("\n%s %d %d %d"),
			mesh_list_[imesh].name,
			mesh_list_[imesh].material_id,
			mesh_list_[imesh].buffer_list.size(),
			mesh_list_[imesh].tri_list.size());

		auto subtri_list = mesh_list_[imesh].buffer_list;
		for (int iSubTri = 0; iSubTri < subtri_list.size(); iSubTri++)
		{
			vector<TriComponent>& trilist = mesh_list_[imesh].buffer_list[iSubTri];
			for (int itri = 0; itri < trilist.size(); itri++)
			{
				for (int iver = 0; iver < 3; iver++)
				{
					_ftprintf(file, _T("\n%10.4f %10.4f %10.4f"),
						trilist[itri].v[iver].p.x,
						trilist[itri].v[iver].p.y,
						trilist[itri].v[iver].p.z);

					_ftprintf(file, _T("\n%10.4f %10.4f %10.4f"),
						trilist[itri].v[iver].n.x,
						trilist[itri].v[iver].n.y,
						trilist[itri].v[iver].n.z);

					_ftprintf(file, _T("\n%10.4f %10.4f %10.4f"),
						trilist[itri].v[iver].c.x,
						trilist[itri].v[iver].c.y,
						trilist[itri].v[iver].c.z);

					_ftprintf(file, _T("\n%10.4f %10.4f %10.4f"),
						trilist[itri].v[iver].t.x,
						trilist[itri].v[iver].t.y);
				}
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
	AddMaterial(node);
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

void PSCWriter::GetMesh(INode* node, OUT_  PMesh& pmesh)
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
		
		std::vector<TriComponent> trilist;
		trilist.resize(numberof_face);

		for (int iFace = 0; iFace < numberof_face; iFace++)
		{
			int numberof_vert = mesh->getNumVerts();

			if (numberof_vert > 0)
			{
				Point3 v = mesh->verts[mesh->faces[iFace].v[custom_v0]] * tm;
				CopyPoint3(trilist[iFace].v[0].p, v);
				
				v = mesh->verts[mesh->faces[iFace].v[custom_v2]] * tm;
				CopyPoint3(trilist[iFace].v[1].p, v);

				v = mesh->verts[mesh->faces[iFace].v[custom_v1]] * tm;
				CopyPoint3(trilist[iFace].v[2].p, v);
			}

			int numberof_color = mesh->getNumVertCol();
			trilist[iFace].v[0].c = Point4(1, 1, 1, 1);
			trilist[iFace].v[1].c = Point4(1, 1, 1, 1);
			trilist[iFace].v[2].c = Point4(1, 1, 1, 1);
			if (numberof_color > 0)
			{
				trilist[iFace].v[0].c = mesh->vertCol[mesh->vcFace[iFace].t[custom_v0]];
				trilist[iFace].v[0].c = mesh->vertCol[mesh->vcFace[iFace].t[custom_v2]];
				trilist[iFace].v[0].c = mesh->vertCol[mesh->vcFace[iFace].t[custom_v1]];
			}

			int numberof_tex = mesh->getNumTVerts();
			if (numberof_tex)
			{
				Point2 uv = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[custom_v0]];
				trilist[iFace].v[0].t.x = uv.x;
				trilist[iFace].v[0].t.y = 1.0f - uv.y;

				uv = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[custom_v2]];
				trilist[iFace].v[1].t.x = uv.x;
				trilist[iFace].v[1].t.y = 1.0f - uv.y;
				
				uv = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[custom_v1]];
				trilist[iFace].v[2].t.x = uv.x;
				trilist[iFace].v[2].t.y = 1.0f - uv.y;
			}

			mesh->buildNormals();
			int vert = mesh->faces[iFace].getVert(custom_v0);
			RVertex* rvertex = mesh->getRVertPtr(vert);
			Point3 vn = GetVertexNormal(mesh, iFace, rvertex);
			CopyPoint3(trilist[iFace].v[custom_v0].n, vn);

			vert = mesh->faces[iFace].getVert(custom_v1);
			rvertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rvertex);
			CopyPoint3(trilist[iFace].v[custom_v1].n, vn);

			vert = mesh->faces[iFace].getVert(custom_v2);
			rvertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rvertex);
			CopyPoint3(trilist[iFace].v[custom_v2].n, vn);

			trilist[iFace].tri_index = mesh->faces[iFace].getMatID();
		}
		
		pmesh.buffer_list.push_back(trilist);
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
	PMtl material;
	material.name = FixupName(mtl->GetName());// FixupName 들어갈곳
	
	int numberof_submtl = mtl->NumSubMtls();
	if (numberof_submtl > 0)
	{
		for (int i = 0; i < numberof_submtl; i++)
		{
			Mtl* sub_material = mtl->GetSubMtl(i);
			PMtl sub_pmtl;
			sub_pmtl.name = FixupName(sub_material->GetName());
			GetTexture(sub_pmtl, sub_material);
			material.submaterial_list.push_back(sub_pmtl);
		}
	}
	else
		GetTexture(material, mtl);

	pmtl_list_.push_back(material);
	
}

void PSCWriter::GetTexture(PMtl& pmtl, Mtl* mtl)
{
	int numberof_map = mtl->NumSubTexmaps();

	for (int i = 0; i < numberof_map; i++)
	{
		Texmap* tex = mtl->GetSubTexmap(i);

		if (!tex) continue;

		if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0X00))
		{
			PTextureMap texmap;
			texmap.map_id = i;

			TSTR full_name;
			TSTR map_name = ((BitmapTex*)(tex))->GetMapName();
			SplitPathFile(map_name, &full_name, &texmap.name);
			pmtl.tex_list.push_back(texmap);
		}
	}
}

void PSCWriter::CopyPoint3(Point3& dest, Point3& src)
{
	dest.x = src.x;
	dest.y = src.z;
	dest.z = src.y;
}

void PSCWriter::AddMaterial(INode* node)
{
	Mtl* mtl = node->GetMtl();
	
	if (!mtl) return;

	std::vector<Mtl*>::iterator iter = std::find(material_list_.begin(), material_list_.end(), mtl);

	if (iter == material_list_.end())
	{
		material_list_.push_back(mtl);
		GetMaterial(node);
	}


}

int PSCWriter::FindMaterialIndex(INode* node)
{
	Mtl* mtl = node->GetMtl();
	if (!mtl) return -1;

	for (int i = 0; i < material_list_.size(); i++)
	{
		if (material_list_[i] == mtl)
		{
			return i;
		}
	}

	return -1;
}

Mtl* PSCWriter::FindMaterial(INode* node)
{
	Mtl* mtl = node->GetMtl();
	if (!mtl) return nullptr;

	std::vector<Mtl*>::iterator iter = std::find(material_list_.begin(), material_list_.end(), mtl);

	if (iter != material_list_.end())
	{
		return *iter;
	}

	assert(false);
	return nullptr;
}

TCHAR* PSCWriter::FixupName(MSTR name)
{
	TCHAR tmp_buffer[MAX_PATH] = { 0, };
	ZeroMemory(tmp_buffer, sizeof(tmp_buffer));
	
	TCHAR* cPtr;
	_tcscpy_s(tmp_buffer, name);
	cPtr = tmp_buffer;

	while (*cPtr)
	{
		if (*cPtr == '"')* cPtr = SINGLE_QUOTE;
		else if (*cPtr == ' ' || *cPtr <= CTL_CHARS)
			* cPtr = _T('_');
		cPtr++;
	}
	return tmp_buffer;

}

bool PSCWriter::SwitchAllNodeToMesh(std::vector<INode*>& object_list, std::vector<PMesh>& mesh_list)
{
	for (int i = 0; i < object_list.size(); i++)
	{
		INode* node = object_list[i];
		PMesh mesh;
		mesh.name = FixupName(node->GetName());
		GetMesh(node, mesh);
		mesh.material_id = FindMaterialIndex(node);
		mesh_list.push_back(mesh);
	}
	return true;
}
