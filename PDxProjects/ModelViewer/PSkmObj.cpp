#include "PSkmObj.h"

PSkmObj::PSkmObj()
{
}

PSkmObj::~PSkmObj()
{
	//delete parse;
}

bool PSkmObj::Init(ID3D11Device* device, ID3D11DeviceContext* context, std::wstring vs_file_path,
	std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name, std::wstring object_path, std::wstring texcomp_path)
{
	PModel::Init(device, context);

	std::vector<MaxExportSkinInfo> info;
	std::vector<Material> matrl_info;
	MaxScene scene;
	//parse = new PParser();

	parse.MaxExportParse(info, matrl_info, scene, object_path, texcomp_path, device, FILE_EXTENSION_TYPE::SKM);

	object_list_.resize(info.size());
	bone_list_.resize(info.size());

	for (int k = 0; k < info.size(); k++)
	{
		object_list_[k].info = std::move(info[k]);
	}

	for (int i = 0; i < info.size(); i++) //오브젝트별 부모 정보 이식
	{
		auto it = std::find_if(object_list_.begin(), object_list_.end(),
			[&](const PSkinMesh& mesh) {return mesh.info.meshinfo.meshlist_name == object_list_[i].info.meshinfo.parent_name; });

		if (it == object_list_.end())
			object_list_[i].parent_skinmesh = nullptr;
		else
			object_list_[i].parent_skinmesh = &(*it);

	}

	for (int i = 0; i < info.size(); i++) //오브젝트 정보 이식
	{

		D3DXMATRIX mat_parent, mat_inv_parent;
		D3DXMatrixIdentity(&mat_parent);

		if (object_list_[i].parent_skinmesh != nullptr)
		{
			mat_parent = object_list_[i].parent_skinmesh->info.meshinfo.world_mat;
		}

		D3DXMatrixInverse(&mat_inv_parent, NULL, &mat_parent);

		auto& cur_obj = object_list_[i];

		cur_obj.mat_world_self = cur_obj.info.meshinfo.world_mat * mat_inv_parent;
		D3DXMatrixInverse(&cur_obj.mat_inverse_world, NULL, &cur_obj.info.meshinfo.world_mat);

		D3DXMatrixDecompose(
			&cur_obj.scale_track,
			&cur_obj.rot_track,
			&cur_obj.pos_track,
			&cur_obj.mat_world_self
		);

		D3DXMatrixTranslation(&cur_obj.mat_pos,
			cur_obj.pos_track.x,
			cur_obj.pos_track.y,
			cur_obj.pos_track.z);

		D3DXMatrixScaling(&cur_obj.mat_scale,
			cur_obj.scale_track.x,
			cur_obj.scale_track.y,
			cur_obj.scale_track.z);

		D3DXMatrixRotationQuaternion(
			&cur_obj.mat_rot,
			&cur_obj.rot_track
		);

		bone_list_ = std::move(cur_obj.info.bone_list);
	}

	material_list_ = std::move(matrl_info); //머터리얼 정보 이식
	scene_ = std::move(scene);

	Create(device_, immediate_context_, vs_file_path, vs_func_name, ps_file_path, ps_func_name);



	return true;
}

bool PSkmObj::Frame()
{
	elapsed_time_ += 1.0f * g_SecondPerFrame * scene_.frame_rate * scene_.tick_per_frame;

	float animation_end_time = scene_.last_frame * scene_.tick_per_frame;

	if (elapsed_time_ >= animation_end_time)
		elapsed_time_ = 0.0f;


	D3DXMATRIX pseudo_mat_parent;
	D3DXMatrixIdentity(&pseudo_mat_parent);
	for (int obj = 0; obj < object_list_.size(); obj++)
	{

		if (object_list_[obj].parent_skinmesh) // parent node가 존재할 경우(상속되는 본 애니메이션)
		{
			Interpolate(object_list_[obj], object_list_[obj].parent_skinmesh->mat_calculation, elapsed_time_);
		}
		else
		{
			Interpolate(object_list_[obj], pseudo_mat_parent, elapsed_time_);
		}
	}

	for (int obj = 0; obj < bone_list_.size(); obj++)
	{
		D3DXMatrixIdentity(&bone_matrix[obj]);
		D3DXMatrixTranspose(&cb_bonedata_.g_matrix[obj], &bone_matrix[obj]);
	}

	if (light_obj_ != nullptr)
		light_obj_->Frame();

	return true;
}


HRESULT PSkmObj::CreateVertexBuffer()
{
	for (int obj = 0; obj < object_list_.size(); obj++)
	{
		object_list_[obj].helper_list.resize(object_list_[obj].info.vertex_list.size());

		for (int i = 0; i < object_list_[obj].info.vertex_list.size(); i++)
		{
			if (object_list_[obj].vertices_list[i].size() == 0)
			{
				continue;
			}

			auto& cur_dxhelper = object_list_[obj].helper_list[i];
			cur_dxhelper.vertex_size_ = sizeof(Vertex_PNCTW8I8);
			cur_dxhelper.vertex_count_ = object_list_[obj].vertices_list[i].size();


			cur_dxhelper.vertex_buffer_.Attach(DX::CreateVertexBuffer(device_,
				&object_list_[obj].vertices_list[i].at(0),
				cur_dxhelper.vertex_count_,
				cur_dxhelper.vertex_size_, false));

			if (cur_dxhelper.vertex_buffer_.Get() == nullptr)
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT PSkmObj::CreateIndexBuffer()
{
	for (int obj = 0; obj < object_list_.size(); obj++)
	{

		for (int i = 0; i < object_list_[obj].info.index_list.size(); i++)
		{
			if (object_list_[obj].indices_list[i].size() == 0)
			{
				continue;
			}

			auto& cur_dxhelper = object_list_[obj].helper_list[i];
			cur_dxhelper.index_count_ = object_list_[obj].indices_list[i].size();
			cur_dxhelper.index_buffer_.Attach(DX::CreateIndexBuffer(device_,
				&object_list_[obj].indices_list[i].at(0),
				cur_dxhelper.index_count_,
				sizeof(int), false));

			if (cur_dxhelper.index_buffer_.Get() == nullptr)
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT PSkmObj::CreateVertexData()
{
	int object_size = object_list_.size();

	for (int obj = 0; obj < object_size; obj++)
	{
		auto& cur_object = object_list_[obj];
		cur_object.vertices_list.resize(cur_object.info.vertex_list.size());
		for (int i = 0; i < cur_object.info.vertex_list.size(); i++)
		{
			cur_object.vertices_list[i] = std::move(cur_object.info.vertex_list[i]);
			ReviseNotUseIdxWeightVert(cur_object.vertices_list[i]);
		}
	}
	return S_OK;
}

HRESULT PSkmObj::CreateIndexData()
{
	int object_size = object_list_.size();

	for (int obj = 0; obj < object_size; obj++)
	{
		auto& cur_object = object_list_[obj];
		cur_object.indices_list.resize(cur_object.info.index_list.size());
		for (int i = 0; i < cur_object.info.index_list.size(); i++)
		{
			cur_object.indices_list[i] = std::move(cur_object.info.index_list[i]);
		}
	}
	return S_OK;
}

HRESULT PSkmObj::CreateConstantBuffer()
{
	PModel::CreateConstantBuffer();

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(CB_BoneAnimation);
	HRESULT hr = device_->CreateBuffer(&desc, NULL, cbuffer_bone_anim_.GetAddressOf());

	if (hr == E_FAIL)
		assert(false);

	return hr;
}

HRESULT PSkmObj::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0  },

		//index,weight data  : 즉 8개의 index, 8개의 weight이 정점에 영향을 끼칠 수 있다.
		{"TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 96, D3D11_INPUT_PER_VERTEX_DATA, 0  },

	};

	int element_count = sizeof(layout) / sizeof(layout[0]);
	dx_helper_.input_layout_.Attach(DX::CreateInputLayout(device_,
		dx_helper_.vertex_blob_->GetBufferSize(), dx_helper_.vertex_blob_->GetBufferPointer(),
		layout, element_count));

	return S_OK;
}

bool PSkmObj::PreRender()
{
	dx_helper_.PreRender(immediate_context_, dx_helper_.vertex_size_); 
	
	D3D11_MAPPED_SUBRESOURCE map_subres;
	immediate_context_->Map(cbuffer_bone_anim_.Get(), 0, D3D11_MAP_WRITE_DISCARD,
		0, &map_subres);
	memcpy(map_subres.pData, &cb_bonedata_, sizeof(CB_BoneAnimation));
	immediate_context_->Unmap(cbuffer_bone_anim_.Get(), 0);

	immediate_context_->VSSetConstantBuffers(1, 1, cbuffer_bone_anim_.GetAddressOf());

	if (light_obj_ != nullptr)
		light_obj_->Render();

	return true;
}

bool PSkmObj::PostRender()
{

	for (int obj = 0; obj < object_list_.size(); obj++)
	{


		if (object_list_.size() == 1)
		{
		}
		else
		{
			//모델뷰어에서는 오브젝트가 무조건 한 개일 수 밖에 없음(애니메이션 미적용에, skm이므로. 그런데, 예상 못할 오류를 대비해 이 코드를 남겨놓음.
			constant_data_.matWorld = object_list_[obj].mat_inverse_world * object_list_[obj].mat_calculation;
			assert(false);
		}
		D3DXMatrixTranspose(&constant_data_.matWorld, &constant_data_.matWorld);

		immediate_context_->UpdateSubresource(dx_helper_.constant_buffer_.Get(), 0, NULL, &constant_data_, 0, 0);

		int root_index = object_list_[obj].info.meshinfo.material_id;

		if (root_index == -1) //material이 없음 (더미, 본)
			continue;

		if (root_index >= 0 && material_list_[root_index].sub_material_list.size() > 0)
		{
			for (int submatl = 0; submatl < object_list_[obj].info.meshinfo.numberof_submesh; submatl++)
			{
				if (object_list_[obj].helper_list[submatl].vertex_size_ == 0)
					continue;

				std::wstring key = material_list_[root_index].sub_material_list[submatl].tex_list[0].texname;
				ID3D11ShaderResourceView** srv = PTextureManager::GetInstance().GetTextureFromMap(key)->shader_res_view_double_ptr();
				immediate_context_->PSSetShaderResources(0, 1, srv);

				UINT stride = object_list_[obj].helper_list[submatl].vertex_size_;
				UINT offset = 0;

				immediate_context_->IASetVertexBuffers(0, 1, object_list_[obj].helper_list[submatl].vertex_buffer_.GetAddressOf(),
					&stride, &offset);

				immediate_context_->IASetIndexBuffer(object_list_[obj].helper_list[submatl].index_buffer_.Get(),
					DXGI_FORMAT_R32_UINT, 0);

				immediate_context_->DrawIndexed(object_list_[obj].indices_list[submatl].size(), 0, 0);

			}

		}
		else
		{
			std::wstring key = material_list_[0].tex_list[0].texname;
			ID3D11ShaderResourceView** srv = PTextureManager::GetInstance().GetTextureFromMap(key)->shader_res_view_double_ptr();
			immediate_context_->PSSetShaderResources(0, 1, srv);
			UINT stride = object_list_[obj].helper_list[0].vertex_size_;
			if (stride <= 0)
				assert(false);
			UINT offset = 0;

			immediate_context_->IASetVertexBuffers(0, 1, object_list_[obj].helper_list[0].vertex_buffer_.GetAddressOf(),
				&stride, &offset);
			immediate_context_->IASetIndexBuffer(object_list_[obj].helper_list[0].index_buffer_.Get(),
				DXGI_FORMAT_R32_UINT, 0);

			immediate_context_->DrawIndexed(object_list_[obj].indices_list[0].size(), 0, 0);
		}



	}


	return true;
}

bool PSkmObj::GetAnimationTrack(const float elapsed_time, const std::vector<PAnimTrack>& tracklist,
	OUT_ PAnimTrack& start_track, OUT_ PAnimTrack& end_track)
{

	for (auto& track : tracklist)
	{
		if (track.tick > elapsed_time)
		{
			end_track = track;
			return true;
		}
		start_track = track;
	}
	return false;
}

void PSkmObj::Interpolate(PSkinMesh& mesh, D3DXMATRIX& mat_parent, float elapsed_time)
{
	D3DXMATRIX mat_anim_scale, mat_anim_rot, mat_anim_pos;
	mat_anim_scale = mesh.mat_scale;
	mat_anim_rot = mesh.mat_rot;
	mat_anim_pos = mesh.mat_pos;

	D3DXMatrixIdentity(&mesh.mat_calculation);

	PAnimTrack start_track, end_track;
	D3DXQUATERNION quat_rotation;
	D3DXVECTOR3 trans;

	if (mesh.info.animlist_rot.size() > 0)
	{
		if (GetAnimationTrack(elapsed_time, mesh.info.animlist_rot, start_track, end_track))
		{
			float t = (elapsed_time - start_track.tick) / (end_track.tick - start_track.tick);
			D3DXQuaternionSlerp(&quat_rotation, &start_track.q, &end_track.q, t);
		}
		D3DXMatrixRotationQuaternion(&mat_anim_rot, &quat_rotation);
	}


	if (mesh.info.animlist_scale.size() > 0)
	{
		if (GetAnimationTrack(elapsed_time, mesh.info.animlist_scale, start_track, end_track))
		{
			float t = (elapsed_time - start_track.tick) / (end_track.tick - start_track.tick);
			D3DXQuaternionSlerp(&quat_rotation, &start_track.q, &end_track.q, t);
			D3DXVec3Lerp(&trans, &start_track.p, &end_track.p, t);

		}

		D3DXMATRIX mat_scale_vector;
		D3DXMATRIX mat_scale_rot, mat_scale_rot_inv;
		D3DXMatrixScaling(&mat_scale_vector, trans.x, trans.y, trans.z);
		D3DXMatrixRotationQuaternion(&mat_scale_rot, &quat_rotation);
		D3DXMatrixInverse(&mat_scale_rot_inv, NULL, &mat_scale_rot);

		mat_anim_scale = mat_scale_rot_inv * mat_scale_vector * mat_scale_rot;
	}

	if (mesh.info.animlist_pos.size() > 0)
	{
		if (GetAnimationTrack(elapsed_time, mesh.info.animlist_pos, start_track, end_track))
		{
			float t = (elapsed_time - start_track.tick) / (end_track.tick - start_track.tick);
			D3DXVec3Lerp(&trans, &start_track.p, &end_track.p, t);
		}
		D3DXMatrixTranslation(&mat_anim_pos, trans.x, trans.y, trans.z);
	}

	D3DXMATRIX mat_animation;
	mat_animation = mat_anim_scale * mat_anim_rot;
	mat_animation._41 = mat_anim_pos._41;
	mat_animation._42 = mat_anim_pos._42;
	mat_animation._43 = mat_anim_pos._43;
	mesh.mat_calculation = mat_animation * mat_parent;
}

void PSkmObj::ReviseNotUseIdxWeightVert(std::vector<Vertex_PNCTW8I8>& vert)
{
	for (int ii = 0; ii < vert.size(); ii++)
	{
		if (vert[ii].i0.x == 0 && vert[ii].i0.y == 0 && vert[ii].i0.z == 0 && vert[ii].i0.w == 0)
		{
			if (vert[ii].i1.x == 0 && vert[ii].i1.y == 0 && vert[ii].i1.z == 0 && vert[ii].i1.w == 0)
			{
				vert[ii].i0.x = 0.0f;
				vert[ii].w0.x = 1.0f;
			}
		}
	}
}

