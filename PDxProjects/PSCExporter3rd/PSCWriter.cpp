#include "pch.h"
#include "PSCWriter.h"

struct AscendingSort
{
	bool operator()(TriComponent& rpStart, TriComponent& rpEnd)
	{
		return rpStart.tri_index < rpEnd.tri_index;
	}
};
static int g_search_index = 0;
struct IsSameInt // find_to와 같은지 판단해 주는 함수
{
	bool operator()(TriComponent& value)
	{
		return value.tri_index == g_search_index;
	}
};
PSCWriter::PSCWriter()
{
	file = nullptr;
}

PSCWriter::~PSCWriter()
{
}

//맥스에서 애니메이션 정보등을 읽어온다.
void PSCWriter::Set(const TCHAR* name, Interface* interface_max)
{
	interface_max_ = interface_max;
	filename_ = name;
	rootnode_ = interface_max_->GetRootNode();
	interval_ = interface_max_->GetAnimRange();

	scene_.first_frame = interval_.Start() / GetTicksPerFrame();
	scene_.last_frame = interval_.End() / GetTicksPerFrame();
	scene_.frame_rate = GetFrameRate();
	scene_.tick_per_frame = GetTicksPerFrame();

	PreProcess(rootnode_);
}
/*
skm, mat 파일을 export하는 메인 함수입니다.
*/
bool PSCWriter::Export()
{
	SwitchAllNodeToMesh(object_list_, mesh_list_);

	scene_.numberof_materials = pmtl_list_.size();
	scene_.numberof_meshes = mesh_list_.size();

	_wfopen_s(&file, filename_.c_str(), _T("wb"));
	_ftprintf(file, _T("%s %d"), _T("ExporterObj"), object_list_.size());

	_ftprintf(file, _T("\n%s"), L"#HEADER INFO  [FirstFrame/LastFrame/FrameRate/TickPerFrame/MeshListSize/PMaterialListSize]  ");
	_ftprintf(file, _T("\n%d %d %d %d %d %d"), scene_.first_frame, scene_.last_frame, scene_.frame_rate, scene_.tick_per_frame 
		,mesh_list_.size(), pmtl_list_.size());

	//마테리얼이 있을 경우, 마테리얼 출력
	for (int i = 0; i < pmtl_list_.size(); i++)
	{
		_ftprintf(file, _T("\n%s"), L"#MATERIAL INFO  [PMaterialListName/SubMaterialListSize] Sub-> [SubMaterialName/TexSize] // [TexmapID/TexmapName] ");
		_ftprintf(file, _T("\n%s %d"),
			pmtl_list_[i].name, pmtl_list_[i].submaterial_list.size());

		//서브메테리얼이 있을 경우
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
						current_texlist[itex].map_id,
						current_texlist[itex].name);
				}
			}
		}
		//서브마테리얼이 없을 경우, 서브마테리얼 이름을 NotExistSubMtrl로 만든다. 
		else
		{
			_ftprintf(file, _T("\n%s %d"),
				L"NotExistSubMtrl",
				pmtl_list_[i].tex_list.size());

			//메인마테리얼의 TexList는 있을 수 있으므로 
			for (int itex = 0; itex < pmtl_list_[i].tex_list.size(); itex++)
			{
				_ftprintf(file, _T("\n%d %s"),
					pmtl_list_[i].tex_list[itex].map_id,
					pmtl_list_[i].tex_list[itex].name);
			}
		}
	}


	for (int imesh = 0; imesh < mesh_list_.size(); imesh++)
	{
		//mesh list
		_ftprintf(file, _T("\n%s"), L"#OBJECT INFO [MeshListName/ParentName/MaterialID/BufferListSize/TriListSize]");
		_ftprintf(file, _T("\n%s %s %d %d %d"),
			mesh_list_[imesh].name,
			mesh_list_[imesh].parent_name,
			mesh_list_[imesh].material_id,
			mesh_list_[imesh].buffer_list.size(),
			mesh_list_[imesh].tri_list.size());
		_ftprintf(file, _T("\n#WORLD MATRIX"));
		_ftprintf(file, _T("\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f"),
			mesh_list_[imesh].world_d3d._11,
			mesh_list_[imesh].world_d3d._12,
			mesh_list_[imesh].world_d3d._13,
			mesh_list_[imesh].world_d3d._14,

			mesh_list_[imesh].world_d3d._21,
			mesh_list_[imesh].world_d3d._22,
			mesh_list_[imesh].world_d3d._23,
			mesh_list_[imesh].world_d3d._24,

			mesh_list_[imesh].world_d3d._31,
			mesh_list_[imesh].world_d3d._32,
			mesh_list_[imesh].world_d3d._33,
			mesh_list_[imesh].world_d3d._34,

			mesh_list_[imesh].world_d3d._41,
			mesh_list_[imesh].world_d3d._42,
			mesh_list_[imesh].world_d3d._43,
			mesh_list_[imesh].world_d3d._44);

		auto subtri_list = mesh_list_[imesh].buffer_list;
		for (int iSubTri = 0; iSubTri < subtri_list.size(); iSubTri++)
		{
			std::vector<PNCT>& vertex_list = mesh_list_[imesh].vertex_list[iSubTri];
			_ftprintf(file, _T("\nVertexList %d"), vertex_list.size());

			for (int iver = 0; iver < vertex_list.size(); iver++)
			{
	
					_ftprintf(file, _T("\n%10.4f %10.4f %10.4f"),
						vertex_list[iver].p.x,
						vertex_list[iver].p.y,
						vertex_list[iver].p.z);

					_ftprintf(file, _T(" %10.4f %10.4f %10.4f"),
						vertex_list[iver].n.x,
						vertex_list[iver].n.y,
						vertex_list[iver].n.z);

					_ftprintf(file, _T(" %10.4f %10.4f %10.4f %10.4f"),
						vertex_list[iver].c.x,
						vertex_list[iver].c.y,
						vertex_list[iver].c.z,
						vertex_list[iver].c.w);

					_ftprintf(file, _T(" %10.4f %10.4f"),
						vertex_list[iver].t.x,
						vertex_list[iver].t.y);
			}

			std::vector<int> index_list = mesh_list_[imesh].index_list[iSubTri];
			_ftprintf(file, _T("\nIndexList %d"), index_list.size());

			for (int index = 0; index < index_list.size(); index+=3)
			{
				_ftprintf(file, _T("\n%d %d %d"),
					index_list[index + 0],
					index_list[index + 1],
					index_list[index + 2]);
			}
		}
		ExportAnimation(mesh_list_[imesh]);
	}

	::fclose(file);

	MessageBox(GetActiveWindow(), filename_.c_str(),
		_T("Succeed!"), MB_OK);

	return true;
}
 
/*재귀호출을 통해, 맥스의 모든 노드를 루트부터 시작해서 모든 자식노드를 순회하며 세팅한다.*/
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
/*지오메트리 오브젝트와 헬퍼 오브젝트를 오브젝트 리스트에 추가한다.
지오메트리에는 nodeTm, 버텍스, 노말 등의 정보가 있다.
헬퍼에는 더미 등이 있다. */
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
			case HELPER_CLASS_ID:
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
	TriObject* triobj = GetTriObjectFromNode(node, interval_.Start(), deleteit);
	
	if (!triobj) return;

	Mesh* mesh = &triobj->GetMesh();
	bool is_negscale = IsTmNegativeParity(tm);
	int custom_v0, custom_v1, custom_v2;

	if (is_negscale) { custom_v0 = 2, custom_v1 = 1; custom_v2 = 0; }
	else { custom_v0 = 0; custom_v1 = 1; custom_v2 = 2; }

	if (mesh)
	{
		int numberof_face = mesh->getNumFaces();
		
		std::vector<TriComponent>& trilist = pmesh.tri_list;
		trilist.resize(numberof_face);
		pmesh.buffer_list.resize(pmesh.numberof_submesh);

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
				trilist[iFace].v[1].c = mesh->vertCol[mesh->vcFace[iFace].t[custom_v2]];
				trilist[iFace].v[2].c = mesh->vertCol[mesh->vcFace[iFace].t[custom_v1]];
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

			vert = mesh->faces[iFace].getVert(custom_v2);
			rvertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rvertex);
			CopyPoint3(trilist[iFace].v[custom_v1].n, vn);

			vert = mesh->faces[iFace].getVert(custom_v1);
			rvertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rvertex);
			CopyPoint3(trilist[iFace].v[custom_v2].n, vn);

			trilist[iFace].tri_index = mesh->faces[iFace].getMatID();

			if (pmesh.material_id < 0 || pmtl_list_[pmesh.material_id].submaterial_list.size() <= 0)
			{
				trilist[iFace].tri_index = 0;
			}

			if (trilist[iFace].tri_index > pmesh.buffer_list.size())
			{
				continue;
				//trilist[iFace].tri_index = 0;
			}

			pmesh.buffer_list[trilist[iFace].tri_index].push_back(trilist[iFace]);


		}
		
		std::sort(trilist.begin(), trilist.end(), AscendingSort());
		int iFace;

		for (int i = 0; i < pmesh.numberof_submesh; i++)
		{
			g_search_index = i;
			iFace = count_if(trilist.begin(), trilist.end(), IsSameInt());
		}

	//	pmesh.buffer_list.push_back(trilist);
		SetUniqueBuffer(pmesh);
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
	else if (numberof_normals && smoothing_group)
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

/* 텍스쳐 등을 담고있는 마테리얼들을 리스트에 추가합니다.
    마테리얼은 여러개의 서브마테리얼을 담고 있을 수 있습니다. */
void PSCWriter::AddMaterial(INode* node)
{
	Mtl* mtl = node->GetMtl();
	
	if (!mtl) return;

	std::vector<Mtl*>::iterator iter = std::find(material_list_.begin(), material_list_.end(), mtl);

	if (iter == material_list_.end()) //중복이 없다면,
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

/*
object_list 모든 INode를 직접 정의한 PMesh타입으로 바꿔준다. 
정확히는 INode의 정보를 사용해 PMesh를 만든다. 
마테리얼은 마테리얼리스트에 저장되고 관리되므로, PMesh는 마테리얼 id 인덱스를 사용하여 관리한다.
*/
bool PSCWriter::SwitchAllNodeToMesh(std::vector<INode*>& object_list, std::vector<PMesh>& mesh_list)
{
	for (int i = 0; i < object_list.size(); i++)
	{
		INode* node = object_list[i];
		PMesh mesh;
		mesh.name = FixupName(node->GetName());
		INode* parent_node = node->GetParentNode();

		if (parent_node && parent_node->IsRootNode() == false) 
			mesh.parent_name = FixupName(parent_node->GetName());
		
		Matrix3 world_3dsmax = node->GetNodeTM(interval_.Start());
		CopyMatrix3(mesh.world_d3d, world_3dsmax);

		mesh.material_id = FindMaterialIndex(node);
		
		//해당 ID를 가진 메테리얼이 서브메테리얼을 가지고 있을 경우
		if(mesh.material_id >= 0 && pmtl_list_[mesh.material_id].submaterial_list.size() > 0) 
		{
			mesh.numberof_submesh = pmtl_list_[mesh.material_id].submaterial_list.size();
		}
		GetMesh(node, mesh);
		GetAnimation(node, mesh);
		mesh_list.push_back(mesh);
	}
	return true;
}

bool PSCWriter::EqualPoint2(Point2 p1, Point2 p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return false;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return false;
	return true;
}

bool PSCWriter::EqualPoint3(Point3 p1, Point3 p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return false;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return false;
	if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		return false;
	return true;
}

bool PSCWriter::EqualPoint4(Point4 p1, Point4 p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO) 
		return false;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return false;
	if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		return false;
	if (fabs(p1.w - p2.w) > ALMOST_ZERO)
		return false;
	return true;
}

/*버텍스리스트의 버텍스와, 입력할 버텍스를 중복체크하여 중복이면 걸러내기 위함이다.*/
int PSCWriter::IsEqualVertexAndVertexList(PNCT& vertex, std::vector<PNCT>& vertex_list)
{
	for (int i = 0; i < vertex_list.size(); i++)
	{
		if (EqualPoint3(vertex.p, vertex_list[i].p) &&
			EqualPoint3(vertex.n, vertex_list[i].n) &&
			EqualPoint4(vertex.c, vertex_list[i].c) &&
			EqualPoint2(vertex.t, vertex_list[i].t))
		{
			return i; //vertex_list의 I번째 버텍스와 중복이 있다.
		}
	}
	

	return -1;  //중복하지 않는다
}

void PSCWriter::CopyMatrix3(OUT_ D3D_MATRIX& d3d_world, Matrix3& matWorld)
{
	Point3 row;
	row = matWorld.GetRow(0);
	d3d_world._11 = row.x;
	d3d_world._12 = row.z;
	d3d_world._13 = row.y;
	d3d_world._14 = 0.0f;
	row = matWorld.GetRow(2);
	d3d_world._21 = row.x;
	d3d_world._22 = row.z;
	d3d_world._23 = row.y;
	d3d_world._24 = 0.0f;
	row = matWorld.GetRow(1);
	d3d_world._31 = row.x;
	d3d_world._32 = row.z;
	d3d_world._33 = row.y;
	d3d_world._34 = 0.0f;
	row = matWorld.GetRow(3);
	d3d_world._41 = row.x;
	d3d_world._42 = row.z;
	d3d_world._43 = row.y;
	d3d_world._44 = 1.0f;

}

/*메시에 있는 face들로부터, face를 이루는 버텍스와 인덱스를 구성하여 
VertexList, IndexList를 만든다.  VertexList의 경우 face끼리 겹치는 정점을 제거하기 위해 중복제거 함수를 이용한다.*/
void PSCWriter::SetUniqueBuffer(PMesh& mesh)
{
	mesh.vertex_list.resize(mesh.buffer_list.size());
	mesh.index_list.resize(mesh.buffer_list.size());

	for (int i = 0; i < mesh.buffer_list.size(); i++)
	{
		std::vector<TriComponent>& tri_array = mesh.buffer_list[i];
		std::vector<PNCT>& vertex_list = mesh.vertex_list[i];
		std::vector<int>& index_list = mesh.index_list[i];
		for (int iFace = 0; iFace < mesh.buffer_list[i].size(); iFace++)
		{
			TriComponent comp = tri_array[iFace];
			for (int iver = 0; iver < 3; iver++)
			{
				int pos = IsEqualVertexAndVertexList(comp.v[iver], vertex_list);
				if (pos < 0) //겹치는게 없는 경우
				{
					vertex_list.push_back(comp.v[iver]);  //버텍스리스트에 버텍스 추가 
					pos = vertex_list.size() - 1; //index = 현재까지 버텍스 들어간 갯수 - 1(1이면 0, 2이면 1..)
				}
				index_list.push_back(pos); //index 추가  (3개씩)
			}

		}

	}

}
/*


*/
void PSCWriter::GetAnimation(INode* node, PMesh& mesh)
{
	//0 -> translate 1 -> rotation  2-> scale
	mesh.animation_enable[0] = false;
	mesh.animation_enable[1] = false;
	mesh.animation_enable[2] = false;


	TimeValue start_frame = interval_.Start();

	/*tm = selfTm * parentTm * Inverse(parentTm)
		Tm이란, 시작 프레임(대개 0프레임)의 월드행렬*/
	Matrix3 tm = node->GetNodeTM(start_frame) * Inverse(node->GetParentTM(start_frame));

	//행렬 분해(SRT)
	AffineParts start_ap;
	decomp_affine(tm, &start_ap);
	
	//quarternion -> 축, 앵글로 변환
	Point3 start_rotate_axis;
	float  start_rotate_value; 

	AngAxisFromQ(start_ap.q, &start_rotate_value, start_rotate_axis);

	PAnimTrack start_animtrack;
	start_animtrack.tick = start_frame;
	start_animtrack.p = start_ap.t;
	start_animtrack.q = start_ap.q;

	mesh.anim_pos.push_back(start_animtrack);
	mesh.anim_rot.push_back(start_animtrack);
	
	start_animtrack.p = start_ap.k;
	start_animtrack.q = start_ap.u;
	mesh.anim_scale.push_back(start_animtrack);

	TimeValue start = interval_.Start() + GetTicksPerFrame(); //start + 1프레임(160tick)
	TimeValue end = interval_.End();

	for (TimeValue t = start; t <= end; t += GetTicksPerFrame())
	{
		Matrix3 tm = node->GetNodeTM(t) * Inverse(node->GetParentTM(t));

		AffineParts frame_ap;
		decomp_affine(tm, &frame_ap);
		PAnimTrack anim;
		ZeroMemory(&anim, sizeof(PAnimTrack));

		anim.tick = t;
		anim.p = frame_ap.t;
		anim.q = frame_ap.q;
		mesh.anim_pos.push_back(anim);
		mesh.anim_rot.push_back(anim);

		anim.p = frame_ap.k;
		anim.q = frame_ap.u;
		mesh.anim_scale.push_back(anim);

		Point3 frame_rotate_axis;
		float  frame_rotate_value;
		AngAxisFromQ(frame_ap.q, &frame_rotate_value, frame_rotate_axis);

		
		//위치, 회전, 스케일 animation이 존재하는지 체크
		if (mesh.animation_enable[0] == false)
		{
			if (EqualPoint3(start_ap.t, frame_ap.t) == false)
			{
				mesh.animation_enable[0] = true;
			}
		}

		if (mesh.animation_enable[1] == false)
		{
			if (EqualPoint3(start_rotate_axis, frame_rotate_axis) == false)
			{
				mesh.animation_enable[1] = true;
			}
			else
			{ //두 숫자(0프레임의 로테이션, 현재프레임의 로테이션)이 서로 다르다면(애니메이션 존재)
				if (fabs(start_rotate_value - frame_rotate_value) >= ALMOST_ZERO)
					mesh.animation_enable[1] = true;
			}
		}

		if (mesh.animation_enable[2] == false)
		{
			if (EqualPoint3(start_ap.k, frame_ap.k) == false)
			{
				mesh.animation_enable[2] = true;
			}
		}

	}

}

void PSCWriter::ExportAnimation(PMesh& mesh)
{
	_ftprintf(file, _T("\n#ANIMATION DATA [translate_size/rot_size/scale_size] // [cur_track / cur_tick / values by components]"));
	_ftprintf(file, _T("\n%d %d %d"),
		(mesh.animation_enable[0]) ? mesh.anim_pos.size() : 0,
		(mesh.animation_enable[1]) ? mesh.anim_rot.size() : 0,
		(mesh.animation_enable[2]) ? mesh.anim_scale.size() : 0);


	if (mesh.animation_enable[0])
	{
		for (int i = 0; i < mesh.anim_pos.size(); i++)
		{
			_ftprintf(file, _T("\n%d %d %10.4f %10.4f %10.4f"),
				i,
				mesh.anim_pos[i].tick,
				mesh.anim_pos[i].p.x,
				mesh.anim_pos[i].p.z,
				mesh.anim_pos[i].p.y);
		}
	}
	if (mesh.animation_enable[1])
	{
		for (int i = 0; i < mesh.anim_rot.size(); i++)
		{
			_ftprintf(file, _T("\n%d %d %10.4f %10.4f %10.4f %10.4f"),
				i,
				mesh.anim_rot[i].tick,
				mesh.anim_rot[i].q.x,
				mesh.anim_rot[i].q.z,
				mesh.anim_rot[i].q.y,
				mesh.anim_rot[i].q.w);
		}
	}
	if (mesh.animation_enable[2])
	{
		for (int i = 0; i < mesh.anim_scale.size(); i++)
		{
			_ftprintf(file, _T("\n%d %d %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f"),
				i,
				mesh.anim_scale[i].tick,
				mesh.anim_scale[i].p.x,
				mesh.anim_scale[i].p.z,
				mesh.anim_scale[i].p.y,
				mesh.anim_scale[i].q.x,
				mesh.anim_scale[i].q.z,
				mesh.anim_scale[i].q.y,
				mesh.anim_scale[i].q.w);
		}
	}
}
