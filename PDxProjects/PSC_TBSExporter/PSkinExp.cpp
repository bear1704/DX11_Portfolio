#include "pch.h"
#include "PSkinExp.h"
#include "PMatrixExp.h"

PSkinExp::PSkinExp()
{
}

PSkinExp::~PSkinExp()
{
}

struct AscendingSort
{
	bool operator()(BipedTricomponent& rpStart, BipedTricomponent& rpEnd)
	{
		return rpStart.tri_index < rpEnd.tri_index;
	}
};
static int g_search_index = 0;
struct IsSameInt // find_to와 같은지 판단해 주는 함수
{
	bool operator()(BipedTricomponent& value)
	{
		return value.tri_index == g_search_index;
	}
};


void PSkinExp::Set(const TCHAR* name, Interface* interface_max)
{
	interface_max_ = interface_max;
	filename_ = name;
	rootnode_ = interface_max_->GetRootNode();
	interval_ = interface_max_->GetAnimRange();

	scene_.first_frame = interval_.Start() / GetTicksPerFrame();
	scene_.last_frame = interval_.End() / GetTicksPerFrame();
	scene_.frame_rate = GetFrameRate();
	scene_.tick_per_frame = GetTicksPerFrame();

	biped_mesh_list_.clear();
}

bool PSkinExp::Export()
{
	SwitchAllNodeToMesh();

	scene_.numberof_materials = pmtl_list_.size();
	scene_.numberof_meshes = biped_mesh_list_.size();

	_wfopen_s(&file, filename_.c_str(), _T("wb"));
	_ftprintf(file, _T("%s %d"), _T("ExporterSkin"), object_list_.size());

	_ftprintf(file, _T("\n%s"), L"#HEADER INFO  [FirstFrame/LastFrame/FrameRate/TickPerFrame/MeshListSize/PMaterialListSize]  ");
	_ftprintf(file, _T("\n%d %d %d %d %d %d"), 
		scene_.first_frame, 
		scene_.last_frame, 
		scene_.frame_rate, 
		scene_.tick_per_frame,
		scene_.numberof_meshes,
		scene_.numberof_materials);


	for (int i = 0; i < pmtl_list_.size(); i++)
	{
		_ftprintf(file, _T("\n%s"), L"#MATERIAL INFO  [PMaterialListName/SubMaterialListSize] Sub-> [SubMaterialName/TexSize] // [TexmapID/TexmapName] ");
		_ftprintf(file, _T("\n%s %d"),
			pmtl_list_[i].name, pmtl_list_[i].submaterial_list.size());

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
		else
		{
			_ftprintf(file, _T("\n%s %d"),
				L"NotExistSubMtrl",
				pmtl_list_[i].tex_list.size());

			for (int itex = 0; itex < pmtl_list_[i].tex_list.size(); itex++)
			{
				_ftprintf(file, _T("\n%d %s"),
					pmtl_list_[i].tex_list[itex].map_id,
					pmtl_list_[i].tex_list[itex].name);
			}
		}
	}


	for (int imesh = 0; imesh < biped_mesh_list_.size(); imesh++)
	{
		//mesh list
		_ftprintf(file, _T("\n%s"), L"#OBJECT INFO [MeshListName/ParentName/MaterialID/BufferListSize/TriListSize]");
		_ftprintf(file, _T("\n%s %s %d %d %d"),
			biped_mesh_list_[imesh].name,
			biped_mesh_list_[imesh].parent_name,
			biped_mesh_list_[imesh].material_id,
			biped_mesh_list_[imesh].buffer_list.size(),
			biped_mesh_list_[imesh].tri_list.size());
		_ftprintf(file, _T("\n#WORLD MATRIX"));
		_ftprintf(file, _T("\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f"),
			biped_mesh_list_[imesh].world_d3d._11,
			biped_mesh_list_[imesh].world_d3d._12,
			biped_mesh_list_[imesh].world_d3d._13,
			biped_mesh_list_[imesh].world_d3d._14,

			biped_mesh_list_[imesh].world_d3d._21,
			biped_mesh_list_[imesh].world_d3d._22,
			biped_mesh_list_[imesh].world_d3d._23,
			biped_mesh_list_[imesh].world_d3d._24,

			biped_mesh_list_[imesh].world_d3d._31,
			biped_mesh_list_[imesh].world_d3d._32,
			biped_mesh_list_[imesh].world_d3d._33,
			biped_mesh_list_[imesh].world_d3d._34,

			biped_mesh_list_[imesh].world_d3d._41,
			biped_mesh_list_[imesh].world_d3d._42,
			biped_mesh_list_[imesh].world_d3d._43,
			biped_mesh_list_[imesh].world_d3d._44);

		auto subtri_list = biped_mesh_list_[imesh].buffer_list;
		for (int iSubTri = 0; iSubTri < subtri_list.size(); iSubTri++)
		{
			std::vector<PNCTWI>& vertex_list = biped_mesh_list_[imesh].vertex_list[iSubTri];
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

				_ftprintf(file, _T(" %10.4f %10.4f %10.4f %10.4f"),
					vertex_list[iver].index1[0],
					vertex_list[iver].index1[1],
					vertex_list[iver].index1[2],
					vertex_list[iver].index1[3]);

				_ftprintf(file, _T(" %10.4f %10.4f %10.4f %10.4f"),
					vertex_list[iver].weight1[0],
					vertex_list[iver].weight1[1],
					vertex_list[iver].weight1[2],
					vertex_list[iver].weight1[3]);
				
				_ftprintf(file, _T(" %10.4f %10.4f %10.4f %10.4f"),
					vertex_list[iver].index2[0],
					vertex_list[iver].index2[1],
					vertex_list[iver].index2[2],
					vertex_list[iver].index2[3]);

				_ftprintf(file, _T(" %10.4f %10.4f %10.4f %10.4f"),
					vertex_list[iver].weight2[0],
					vertex_list[iver].weight2[1],
					vertex_list[iver].weight2[2],
					vertex_list[iver].weight2[3]);
			}

			std::vector<int> index_list = biped_mesh_list_[imesh].index_list[iSubTri];
			_ftprintf(file, _T("\nIndexList %d"), index_list.size());

			for (int index = 0; index < index_list.size(); index += 3)
			{
				_ftprintf(file, _T("\n%d %d %d"),
					index_list[index + 0],
					index_list[index + 1],
					index_list[index + 2]);
			}
		}
	}


	_ftprintf(file, _T("\n%s %d"), L"#GET_NODETM_INVERSE", PMatrixExp::GetInstance().object_list_.size());
	for (int obj = 0; obj < PMatrixExp::GetInstance().object_list_.size(); obj++)
	{

		INode* node = PMatrixExp::GetInstance().object_list_[obj];
		Matrix3 tm = node->GetNodeTM(interval_.Start());
		Matrix3 tm_inv = Inverse(tm);

		D3D_MATRIX world_inv_startframe;
		CopyMatrix3(world_inv_startframe, tm_inv);

		_ftprintf(file,
			_T("\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f"),
			world_inv_startframe._11,
			world_inv_startframe._12,
			world_inv_startframe._13,
			world_inv_startframe._14,

			world_inv_startframe._21,
			world_inv_startframe._22,
			world_inv_startframe._23,
			world_inv_startframe._24,

			world_inv_startframe._31,
			world_inv_startframe._32,
			world_inv_startframe._33,
			world_inv_startframe._34,

			world_inv_startframe._41,
			world_inv_startframe._42,
			world_inv_startframe._43,
			world_inv_startframe._44);
	}

	::fclose(file);

	MessageBox(GetActiveWindow(), filename_.c_str(),
		_T("Succeed!"), MB_OK);

	return true;
}

void PSkinExp::PreProcess(INode* node)
{
	if (node == NULL) return;
	AddObject(node);
	//AddMaterial(node);
	int numberof_children = node->NumberOfChildren();
	for (int i = 0; i < numberof_children; i++)
	{
		INode* child = node->GetChildNode(i);
		PreProcess(child);
	}

}

void PSkinExp::GetMesh(INode* node, OUT_  PBipedMesh& pmesh)
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

		std::vector<BipedTricomponent>& trilist = pmesh.tri_list;
		trilist.resize(numberof_face);
		pmesh.buffer_list.resize(pmesh.numberof_submesh);

		for (int iFace = 0; iFace < numberof_face; iFace++)
		{
			int numberof_vert = mesh->getNumVerts();

			int V0 = mesh->faces[iFace].v[custom_v0];
			int V1 = mesh->faces[iFace].v[custom_v2];
			int V2 = mesh->faces[iFace].v[custom_v1];

			if (numberof_vert > 0)
			{
				Point3 v = mesh->verts[V0] * tm;
				CopyPoint3(trilist[iFace].v[0].p, v);

				v = mesh->verts[V1] * tm;
				CopyPoint3(trilist[iFace].v[1].p, v);

				v = mesh->verts[V2] * tm;
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


			if (pmesh.biped_list.size() > 0)
			{

				//aff_count = affected_count index 갯수
				for (int aff_count = 0; aff_count < pmesh.biped_list[V0].numberof_weight; aff_count++)
				{
					if (aff_count < 4)
					{
						trilist[iFace].v[custom_v0].index1[aff_count] =
							pmesh.biped_list[V0].index_list[aff_count];
						trilist[iFace].v[custom_v0].weight1[aff_count] =
							pmesh.biped_list[V0].weight_list[aff_count];
					}
					else
					{
						trilist[iFace].v[custom_v0].index2[aff_count - 4] =
							pmesh.biped_list[V0].index_list[aff_count];
						trilist[iFace].v[custom_v0].weight2[aff_count - 4] =
							pmesh.biped_list[V0].weight_list[aff_count];
					}
				}

			for (int aff_count = 0; aff_count < pmesh.biped_list[V1].numberof_weight; aff_count++)
			{
				if (aff_count < 4)
				{
					trilist[iFace].v[custom_v1].index1[aff_count] =
						pmesh.biped_list[V1].index_list[aff_count];
					trilist[iFace].v[custom_v1].weight1[aff_count] =
						pmesh.biped_list[V1].weight_list[aff_count];
				}
				else
				{
					trilist[iFace].v[custom_v1].index2[aff_count - 4] =
						pmesh.biped_list[V1].index_list[aff_count];
					trilist[iFace].v[custom_v1].weight2[aff_count - 4] =
						pmesh.biped_list[V1].weight_list[aff_count];
				}
			}
			for (int aff_count = 0; aff_count < pmesh.biped_list[V2].numberof_weight; aff_count++)
			{
				if (aff_count < 4)
				{
					trilist[iFace].v[custom_v2].index1[aff_count] =
						pmesh.biped_list[V2].index_list[aff_count];
					trilist[iFace].v[custom_v2].weight1[aff_count] =
						pmesh.biped_list[V2].weight_list[aff_count];
				}
				else
				{
					trilist[iFace].v[custom_v2].index2[aff_count - 4] =
						pmesh.biped_list[V2].index_list[aff_count];
					trilist[iFace].v[custom_v2].weight2[aff_count - 4] =
						pmesh.biped_list[V2].weight_list[aff_count];
				}
			}

		}

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

bool PSkinExp::SwitchAllNodeToMesh()
{
	for (int i = 0; i < object_list_.size(); i++)
	{
		INode* node = object_list_[i];
		PBipedMesh mesh;
		mesh.name = FixupName(node->GetName());
		INode* parent_node = node->GetParentNode();

		if (parent_node && parent_node->IsRootNode() == false)
			mesh.parent_name = FixupName(parent_node->GetName());

		SetBipedInfo(node, mesh);
		
		Matrix3 world_3dsmax = node->GetNodeTM(interval_.Start());
		CopyMatrix3(mesh.world_d3d, world_3dsmax);
		Matrix3 world_3dsmax_inv = Inverse(world_3dsmax);
		CopyMatrix3(mesh.world_d3d_inv, world_3dsmax_inv);

		Mtl* mtl = node->GetMtl();
		
		if (mtl)
			mesh.material_id = FindMaterialIndex(node);

		if (mesh.material_id >= 0 && pmtl_list_[mesh.material_id].submaterial_list.size() > 0) //해당 ID를 가진 메테리얼이 서브메테리얼을 가지고 있을 경우
		{
			mesh.numberof_submesh = pmtl_list_[mesh.material_id].submaterial_list.size();
		}

		GetMesh(node, mesh);
		biped_mesh_list_.push_back(mesh);
	}
	return true;
}


int PSkinExp::IsEqualVertexAndVertexList(PNCTWI& vertex, std::vector<PNCTWI>& vertex_list)
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


void PSkinExp::SetUniqueBuffer(PBipedMesh& mesh)
{
	mesh.vertex_list.resize(mesh.buffer_list.size());
	mesh.index_list.resize(mesh.buffer_list.size());

	for (int i = 0; i < mesh.buffer_list.size(); i++)
	{
		std::vector<BipedTricomponent>& tri_array = mesh.buffer_list[i];
		std::vector<PNCTWI>& vertex_list = mesh.vertex_list[i];
		std::vector<int>& index_list = mesh.index_list[i];
		for (int iFace = 0; iFace < mesh.buffer_list[i].size(); iFace++)
		{
			BipedTricomponent comp = tri_array[iFace];
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


void PSkinExp::SetBipedInfo(INode* node, PBipedMesh& bmesh)
{
	Modifier* phy_mod = FindModifier(node, 
		Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));
	Modifier* skin_mod = FindModifier(node, 
		SKIN_CLASSID);

	if (phy_mod)
		ExportPhysiqueData(node, phy_mod, bmesh);
	else if (skin_mod)
		ExportSkinData(node, skin_mod, bmesh);


}

Modifier* PSkinExp::FindModifier(INode* node, Class_ID classid)
{
	Object* object_ptr = node->GetObjectRef();
	if (!object_ptr) return nullptr;

	while (object_ptr->SuperClassID() == GEN_DERIVOB_CLASS_ID && object_ptr)
	{
		IDerivedObject* DerivedObjectPtr = (IDerivedObject*)(object_ptr);

		int ModStackIndex = 0;
		while (ModStackIndex < DerivedObjectPtr->NumModifiers())
		{
			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);

			if (ModifierPtr->ClassID() == classid)
			{
				return ModifierPtr;
			}

			ModStackIndex++;
		}
		object_ptr = DerivedObjectPtr->GetObjRef();
	}

	return nullptr;
}

void PSkinExp::ExportPhysiqueData(INode* node, Modifier* modi, PBipedMesh& mesh)
{
	IPhysiqueExport* phy_export = (IPhysiqueExport*)modi->GetInterface(I_PHYINTERFACE);
	IPhyContextExport* context_export = (IPhyContextExport*)phy_export->GetContextInterface(node);

	context_export->ConvertToRigid(true);
	context_export->AllowBlending(true);

	int numberof_affected_vertices = context_export->GetNumberVertices();
	mesh.biped_list.resize(numberof_affected_vertices);

	for (int i = 0; i < numberof_affected_vertices; i++)
	{
		IPhyVertexExport* vertex_export = 
			(IPhyVertexExport*)context_export->GetVertexInterface(i);

		if (vertex_export)
		{
			int vertex_type = vertex_export->GetVertexType();

			switch (vertex_type)
			{
			case RIGID_NON_BLENDED_TYPE:
			{
				IPhyRigidVertex* vertex = (IPhyRigidVertex*)vertex_export;
				INode* node = vertex->GetNode();
				int index = GetFindIndex(node);

				if (index < 0) assert(false);

				mesh.biped_list[i].index_list.push_back(index);
				mesh.biped_list[i].weight_list.push_back(1.0f);
				mesh.biped_list[i].numberof_weight = 1;
			}break;
			case RIGID_BLENDED_TYPE:
			{
				IPhyBlendedRigidVertex* vertex =
					(IPhyBlendedRigidVertex*)vertex_export;

				for (int node_index = 0; node_index < vertex->GetNumberNodes(); node_index++)
				{
					INode* node = vertex->GetNode(node_index);
					int index = GetFindIndex(node);
					mesh.biped_list[i].index_list.push_back(index);
					mesh.biped_list[i].weight_list.push_back(vertex->GetWeight(node_index));
				}
				mesh.biped_list[i].numberof_weight = mesh.biped_list[i].index_list.size();
			}break;
			
			default:
				break;
			}

		}
		context_export->ReleaseVertexInterface(vertex_export);

	}
	phy_export->ReleaseContextInterface(context_export);
	modi->ReleaseInterface(I_PHYINTERFACE, phy_export);
}

void PSkinExp::ExportSkinData(INode* node, Modifier* modi, PBipedMesh& mesh)
{
	ISkin* skin = (ISkin*)modi->GetInterface(I_SKIN);
	ISkinContextData* skin_data = skin->GetContextInterface(node);

	if (!skin || !skin_data) return;

	//affvtx = affectex vertex : (주변 노드들에)영향받는 버텍스
	int numberof_affvtx = skin_data->GetNumPoints();
	mesh.biped_list.resize(numberof_affvtx);

	for (int i = 0; i < numberof_affvtx; i++)
	{
		mesh.biped_list[i].numberof_weight = skin_data->GetNumAssignedBones(i);
		for (int bones_count = 0; bones_count < mesh.biped_list[i].numberof_weight; bones_count++)
		{
			int bone_id = skin_data->GetAssignedBone(i, bones_count);
			INode* node = skin->GetBone(bone_id);
			mesh.biped_list[i].index_list.push_back(GetFindIndex(node));
			mesh.biped_list[i].weight_list.push_back(skin_data->GetBoneWeight(i, bones_count));
		}


	}

}

int PSkinExp::GetFindIndex(INode* node)
{
	for (int i = 0; i < PMatrixExp::GetInstance().object_list_.size(); i++)
	{
		if (PMatrixExp::GetInstance().object_list_[i] == node)
			return i;
	}
	return -1;
}
