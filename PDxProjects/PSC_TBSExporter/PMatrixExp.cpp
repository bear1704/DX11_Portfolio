#include "pch.h"
#include "PMatrixExp.h"

void PMatrixExp::PreProcess(INode* node)
{
	if (node == nullptr) return;
	AddObject(node);

	int numberof_children = node->NumberOfChildren();
	for (int i = 0; i < numberof_children; i++)
	{
		INode* child = node->GetChildNode(i);
		PreProcess(child);
	}

}

bool PMatrixExp::Export()
{
	SwitchAllNodeToMesh();

	scene_.numberof_materials = pmtl_list_.size();
	scene_.numberof_meshes = mesh_list_.size();

	_wfopen_s(&file, filename_.c_str(), _T("wb"));
	_ftprintf(file, _T("%s %d"), _T("MatrixExporter_v2"), object_list_.size());

	_ftprintf(file, _T("\n%s"), L"#HEADER INFO  [FirstFrame/LastFrame/FrameRate/TickPerFrame/MeshListSize/PMaterialListSize]  ");
	_ftprintf(file, _T("\n%d %d %d %d %d %d"),
		scene_.first_frame,
		scene_.last_frame,
		scene_.frame_rate,
		scene_.tick_per_frame,
		scene_.numberof_meshes,
		scene_.numberof_materials);

	for (int imesh = 0; imesh < mesh_list_.size(); imesh++)
	{
		//mesh list
		_ftprintf(file, _T("\n%s"), L"#OBJECT INFO [MeshListName/ParentName/TRILISTSIZE/Box max xzy/Box min xzy]");
		_ftprintf(file, _T("\n%s %s %d %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f"),
			mesh_list_[imesh].name,
			mesh_list_[imesh].parent_name,
			mesh_list_[imesh].tri_list.size(),
			mesh_list_[imesh].bounding_box.pmax.x,
			mesh_list_[imesh].bounding_box.pmax.z,
			mesh_list_[imesh].bounding_box.pmax.y, 
			mesh_list_[imesh].bounding_box.pmin.x,
			mesh_list_[imesh].bounding_box.pmin.z, 
			mesh_list_[imesh].bounding_box.pmin.y);

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

		mesh_list_[imesh].tri_list;

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

			for (int index = 0; index < index_list.size(); index += 3)
			{
				_ftprintf(file, _T("\n%d %d %d"),
					index_list[index + 0],
					index_list[index + 1],
					index_list[index + 2]);
			}
		}
		ExportAnimation(mesh_list_[imesh]);
	}
	fclose(file);

	MessageBox(GetActiveWindow(), filename_.c_str(),
		_T("Succeed!"), MB_OK);


	return false;
}

bool PMatrixExp::SwitchAllNodeToMesh()
{
	for (int i = 0; i < object_list_.size(); i++)
	{
		INode* node = object_list_[i];
		PMesh mesh;
		mesh.name = FixupName(node->GetName());
		INode* parent_node = node->GetParentNode();

		if (parent_node && parent_node->IsRootNode() == false)
			mesh.parent_name = FixupName(parent_node->GetName());

		Matrix3 world_3dsmax = node->GetNodeTM(interval_.Start());
		CopyMatrix3(mesh.world_d3d, world_3dsmax);

		mesh.material_id = FindMaterialIndex(node);

		
		Object* object = node->GetObjectRef();
		Control* control = node->GetTMController();

		object->GetDeformBBox(0, mesh.bounding_box, &node->GetObjectTM(0));

		mesh.type = OBJECT_TYPE::CLASS_GEOM;
		if (object && object->ClassID() == Class_ID(BONE_CLASS_ID, 0))
			mesh.type = OBJECT_TYPE::CLASS_BONE;
		else if (object && object->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
			mesh.type = OBJECT_TYPE::CLASS_DUMMY;
		else if (control && control->ClassID() == BIPBODY_CONTROL_CLASS_ID)
			mesh.type = OBJECT_TYPE::CLASS_BIPED;
		else if (control && control->ClassID() == BIPSLAVE_CONTROL_CLASS_ID)
			mesh.type = OBJECT_TYPE::CLASS_BIPED;

		if (mesh.type != OBJECT_TYPE::CLASS_GEOM)
		{
			GetMesh(node, mesh);
		}
		GetAnimation(node, mesh);
		mesh_list_.push_back(mesh);
	}
	return true;

}

void PMatrixExp::SetUniqueBuffer(PMesh& mesh)
{

	mesh.vertex_list.resize(1);
	mesh.index_list.resize(1);
	
	std::vector<TriComponent>& tri_array = mesh.tri_list;
	std::vector<PNCT>& vertex_list = mesh.vertex_list[0];
	std::vector<int>& index_list = mesh.index_list[0];
	for (int iFace = 0; iFace < mesh.tri_list.size(); iFace++)
	{
		TriComponent& comp = tri_array[iFace];
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

PMatrixExp::PMatrixExp()
{
}

PMatrixExp::~PMatrixExp()
{
}
