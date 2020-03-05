#include "PShape.h"

/*            Plane          */
PPlaneObject::PPlaneObject()
{
	width_ = 2.0f;
	height_ = 2.0f;
}

PPlaneObject::~PPlaneObject()
{
}

bool PPlaneObject::Frame()
{
	PModel::Frame();
	sprite_.Frame();
	return true;
}

bool PPlaneObject::Init(ID3D11Device* device, ID3D11DeviceContext* context,
	std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name,
	std::wstring tex_name, std::wstring sprite_name)
{
	PModel::Init(device, context);

	if (sprite_name.compare(L"") != 0)
	{
		be_using_sprite_ = true;
		Create(device_, immediate_context_, vs_file_path, vs_func_name, ps_file_path, ps_func_name, tex_name);
		sprite_.Clone(PSpriteManager::GetInstance().get_sprite_from_map_ex(sprite_name), 1.0f, 1.0f);
	}
	else
	{
		Create(device_, immediate_context_, vs_file_path, vs_func_name, ps_file_path, ps_func_name, tex_name);
	}

	return true;
}


bool PPlaneObject::Render()
{
	if (be_using_sprite_ == true)
	{
		if (sprite_.get_is_dead_() == true)
			return false;

		sprite_.Render(device_, immediate_context_, vertex_list_, dx_helper_, false);
	}
	else
	{
		DX::PTex_uv4 uv4 = texture_->uv_coord();
		ChangeTexValue(vertex_list_, uv4);

		int vertices_count = vertex_list_.size();

		dx_helper_.vertex_size_ = sizeof(Vertex_PNCT);
		dx_helper_.vertex_count_ = vertices_count;
		immediate_context_->UpdateSubresource(dx_helper_.vertex_buffer_.Get(),
			0, NULL, &vertex_list_.at(0), 0, 0);
		dx_helper_.shader_res_view_ = texture_->shader_res_view();
	}

	//if (sprite_.get_is_effect() == true)
	//{
	//	DX::ApplyDepthStencilState(immediate_context_, DX::PDxState::depth_stencil_state_disable_);
	//	constant_data_.color[3] = sprite_.get_alpha_();
	//	PModel::Render();
	//	DX::ApplyDepthStencilState(immediate_context_, DX::PDxState::depth_stencil_state_enable_);
	//}
	//else
	//	PModel::Render();
	PModel::Render();
	return true;
}

HRESULT PPlaneObject::CreateVertexData()
{
	vertex_list_.resize(4);
	vertex_list_[0].pos = D3DXVECTOR3{ -width_, height_, 0.5f };
	vertex_list_[1].pos = D3DXVECTOR3{ width_, height_, 0.5f };
	vertex_list_[2].pos = D3DXVECTOR3{ width_,-height_, 0.5f };
	vertex_list_[3].pos = D3DXVECTOR3{ -width_,-height_, 0.5f };

	vertex_list_[0].normal = D3DXVECTOR3{ 0.0f,0.0f, -1.0f };
	vertex_list_[1].normal = D3DXVECTOR3{ 0.0f,0.0f, -1.0f };
	vertex_list_[2].normal = D3DXVECTOR3{ 0.0f,0.0f, -1.0f };
	vertex_list_[3].normal = D3DXVECTOR3{ 0.0f,0.0f, -1.0f };

	vertex_list_[0].color = D3DXVECTOR4{ 1.0f, 1.0f, 1.0f, 1.0f };
	vertex_list_[1].color = D3DXVECTOR4{ 1.0f, 1.0f, 1.0f, 1.0f };
	vertex_list_[2].color = D3DXVECTOR4{ 1.0f, 1.0f, 1.0f, 1.0f };
	vertex_list_[3].color = D3DXVECTOR4{ 1.0f, 1.0f, 1.0f, 1.0f };

	vertex_list_[0].uv = D3DXVECTOR2{ 0.0f, 0.0f };
	vertex_list_[1].uv = D3DXVECTOR2{ 1.0f, 0.0f };
	vertex_list_[2].uv = D3DXVECTOR2{ 1.0f, 1.0f };
	vertex_list_[3].uv = D3DXVECTOR2{ 0.0f, 1.0f };

	return S_OK;
}

HRESULT PPlaneObject::CreateIndexData()
{
	index_list_.resize(6);
	index_list_[0] = 0; index_list_[1] = 1; index_list_[2] = 2;
	index_list_[3] = 0; index_list_[4] = 2; index_list_[5] = 3;

	return S_OK;
}

void PPlaneObject::CreatePlane(ID3D11Device* device, ID3D11DeviceContext* context, float width, float height, std::wstring sprite_name)
{	
	be_using_sprite_ = true;
	width_ = width;   height_ = height;
	name = sprite_name;
	this->Init(device, context, L"VertexShader.hlsl", "VS_ALPHA", L"PixelShader.hlsl", "PS_ALPHA", L"" , sprite_name);
}


/*            Box          */

PBoxObject::PBoxObject()
{
	is_obb_ = false;
}

PBoxObject::~PBoxObject()
{
}

bool PBoxObject::Init(ID3D11Device* device, ID3D11DeviceContext* context, std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name, std::wstring tex_name, std::wstring sprite_name)
{

	PModel::Init(device, context);

	if (sprite_name.compare(L"") != 0)
	{
		be_using_sprite_ = true;
		Create(device_, immediate_context_, vs_file_path, vs_func_name, ps_file_path, ps_func_name, tex_name);
		sprite_.Clone(PSpriteManager::GetInstance().get_sprite_from_map_ex(sprite_name), 1.0f, 1.0f);
	}
	else
	{
		Create(device_, immediate_context_, vs_file_path, vs_func_name, ps_file_path, ps_func_name, tex_name);
	}

	return true;
}

bool PBoxObject::OBBInit(ID3D11Device* device, ID3D11DeviceContext* context, std::wstring vs_file_path, std::string vs_func_name,
	std::wstring ps_file_path, std::string ps_func_name, float ext_x, float ext_y, float ext_z,
	D3DXVECTOR3 center, D3DXVECTOR3 dir_x, D3DXVECTOR3 dir_y, D3DXVECTOR3 dir_z, std::wstring tex_name, std::wstring sprite_name)
{

	PModel::Init(device, context);

	box_blueprint_.obb_extents[x] = ext_x;
	box_blueprint_.obb_extents[y] = ext_y;
	box_blueprint_.obb_extents[z] = ext_z;

	box_blueprint_.center = center;
	box_blueprint_.obb_axis[x] = dir_x;
	box_blueprint_.obb_axis[y] = dir_y;
	box_blueprint_.obb_axis[z] = dir_z;

	D3DXVECTOR3 x_axis = ext_x * box_blueprint_.obb_axis[x];
	D3DXVECTOR3 y_axis = ext_y * box_blueprint_.obb_axis[y];
	D3DXVECTOR3 z_axis = ext_z * box_blueprint_.obb_axis[z];

	box_blueprint_.box_pos[0] = center - x_axis - y_axis - z_axis;
	box_blueprint_.box_pos[1] = center - x_axis + y_axis - z_axis;
	box_blueprint_.box_pos[2] = center + x_axis + y_axis - z_axis;
	box_blueprint_.box_pos[3] = center + x_axis - y_axis - z_axis;
	box_blueprint_.box_pos[4] = center - x_axis - y_axis + z_axis;
	box_blueprint_.box_pos[5] = center - x_axis + y_axis + z_axis;
	box_blueprint_.box_pos[6] = center + x_axis + y_axis + z_axis;
	box_blueprint_.box_pos[7] = center + x_axis - y_axis + z_axis;

	box_blueprint_.aabb_min = box_blueprint_.box_pos[0];
	box_blueprint_.aabb_max = box_blueprint_.box_pos[0];

	//min, max 비교하여 결정
	for (int vert = 1; vert < 8; vert++)
	{
		if (box_blueprint_.aabb_max.x < box_blueprint_.box_pos[vert].x)
			box_blueprint_.aabb_max.x = box_blueprint_.box_pos[vert].x;
		if (box_blueprint_.aabb_max.y < box_blueprint_.box_pos[vert].y)
			box_blueprint_.aabb_max.y = box_blueprint_.box_pos[vert].y;
		if (box_blueprint_.aabb_max.z < box_blueprint_.box_pos[vert].z)
			box_blueprint_.aabb_max.z = box_blueprint_.box_pos[vert].z;

		if (box_blueprint_.aabb_min.x > box_blueprint_.box_pos[vert].x)
			box_blueprint_.aabb_min.x = box_blueprint_.box_pos[vert].x;
		if (box_blueprint_.aabb_min.y > box_blueprint_.box_pos[vert].y)
			box_blueprint_.aabb_min.y = box_blueprint_.box_pos[vert].y;
		if (box_blueprint_.aabb_min.z > box_blueprint_.box_pos[vert].z)
			box_blueprint_.aabb_min.z = box_blueprint_.box_pos[vert].z;
	}

	D3DXVec3Normalize(&box_blueprint_.obb_axis[x], &box_blueprint_.obb_axis[x]);
	D3DXVec3Normalize(&box_blueprint_.obb_axis[y], &box_blueprint_.obb_axis[y]);
	D3DXVec3Normalize(&box_blueprint_.obb_axis[z], &box_blueprint_.obb_axis[z]);

	D3DXMATRIX mat_scale;
	D3DXMatrixScaling(&mat_scale, box_blueprint_.obb_extents[x], box_blueprint_.obb_extents[y], box_blueprint_.obb_extents[z]);

	matWorld_ *= mat_scale;

	if (sprite_name.compare(L"") != 0)
	{
		be_using_sprite_ = true;
		Create(device_, immediate_context_, vs_file_path, vs_func_name, ps_file_path, ps_func_name, tex_name);
		sprite_.Clone(PSpriteManager::GetInstance().get_sprite_from_map_ex(sprite_name), 1.0f, 1.0f);
	}
	else
	{
		Create(device_, immediate_context_, vs_file_path, vs_func_name, ps_file_path, ps_func_name, tex_name);
	}

	is_obb_ = true;


	matWorld_._41 = box_blueprint_.center.x;
	matWorld_._42 = box_blueprint_.center.y;
	matWorld_._43 = box_blueprint_.center.z;

	return true;
}

bool PBoxObject::Frame()
{
	PModel::Frame();
	sprite_.Frame();

	//회전은 다른 함수에서 적용하므로, 회전결과만 axis에 적용 
	return true;
}

bool PBoxObject::Render()
{
	if (be_using_sprite_ == true)
		sprite_.Render(device_, immediate_context_,vertex_list_, dx_helper_, false);


	DX::PTex_uv4 uv4 = texture_->uv_coord();
	ChangeTexValue(vertex_list_, uv4);

	int vertices_count = vertex_list_.size();

	dx_helper_.vertex_size_ = sizeof(Vertex_PNCT);
	dx_helper_.vertex_count_ = vertices_count;
	//dx_helper_.vertex_buffer_.Attach(DX::CreateVertexBuffer(device_, &vertex_list_.at(0), vertices_count, sizeof(Vertex_PNCT), false));
	immediate_context_->UpdateSubresource(dx_helper_.vertex_buffer_.Get(),
		0, NULL, &vertex_list_.at(0), 0, 0);
	dx_helper_.shader_res_view_ = texture_->shader_res_view();

	immediate_context_->UpdateSubresource(dx_helper_.constant_buffer_.Get(), 0, NULL, &constant_data_, 0, 0);
	PModel::Render();
	return true;
}

HRESULT PBoxObject::CreateVertexData()
{
		vertex_list_.resize(24);
		vertex_list_[0] = Vertex_PNCT(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		vertex_list_[1] = Vertex_PNCT(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		vertex_list_[2] = Vertex_PNCT(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		vertex_list_[3] = Vertex_PNCT(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));
		// 뒷면
		vertex_list_[4] = Vertex_PNCT(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		vertex_list_[5] = Vertex_PNCT(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		vertex_list_[6] = Vertex_PNCT(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		vertex_list_[7] = Vertex_PNCT(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

		// 오른쪽
		vertex_list_[8] = Vertex_PNCT(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		vertex_list_[9] = Vertex_PNCT(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		vertex_list_[10] = Vertex_PNCT(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		vertex_list_[11] = Vertex_PNCT(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

		// 왼쪽
		vertex_list_[12] = Vertex_PNCT(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		vertex_list_[13] = Vertex_PNCT(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		vertex_list_[14] = Vertex_PNCT(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		vertex_list_[15] = Vertex_PNCT(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

		// 윗면
		vertex_list_[16] = Vertex_PNCT(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		vertex_list_[17] = Vertex_PNCT(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		vertex_list_[18] = Vertex_PNCT(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		vertex_list_[19] = Vertex_PNCT(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

		// 아랫면
		vertex_list_[20] = Vertex_PNCT(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		vertex_list_[21] = Vertex_PNCT(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		vertex_list_[22] = Vertex_PNCT(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		vertex_list_[23] = Vertex_PNCT(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	return S_OK;
}

HRESULT PBoxObject::CreateIndexData()
{
	index_list_.resize(36);
	int iIndex = 0;

		index_list_[iIndex++] = 0; 	index_list_[iIndex++] = 1; 	index_list_[iIndex++] = 2; 	index_list_[iIndex++] = 0;	index_list_[iIndex++] = 2; 	index_list_[iIndex++] = 3;
		index_list_[iIndex++] = 4; 	index_list_[iIndex++] = 5; 	index_list_[iIndex++] = 6; 	index_list_[iIndex++] = 4;	index_list_[iIndex++] = 6; 	index_list_[iIndex++] = 7;
		index_list_[iIndex++] = 8; 	index_list_[iIndex++] = 9;	index_list_[iIndex++] = 10;  index_list_[iIndex++] = 8;	index_list_[iIndex++] = 10; index_list_[iIndex++] = 11;
		index_list_[iIndex++] = 12; index_list_[iIndex++] = 13; index_list_[iIndex++] = 14; index_list_[iIndex++] = 12;	index_list_[iIndex++] = 14; index_list_[iIndex++] = 15;
		index_list_[iIndex++] = 16; index_list_[iIndex++] = 17; index_list_[iIndex++] = 18; index_list_[iIndex++] = 16;	index_list_[iIndex++] = 18; index_list_[iIndex++] = 19;
		index_list_[iIndex++] = 20; index_list_[iIndex++] = 21; index_list_[iIndex++] = 22; index_list_[iIndex++] = 20;	index_list_[iIndex++] = 22; index_list_[iIndex++] = 23;

	return S_OK;
}

void PBoxObject::KeyRotate(float x, float y, float z)
{
	D3DXMATRIX mat_rotate;
	D3DXVECTOR3 vec_pos3 = D3DXVECTOR3(matWorld_._41, matWorld_._42, matWorld_._43);
	
	D3DXMatrixRotationYawPitchRoll(&mat_rotate, x * g_SecondPerFrame, y * g_SecondPerFrame, z * g_SecondPerFrame);

	for (int ii = 0; ii < 3; ii++)
	{
		D3DXVec3TransformNormal(&box_blueprint_.obb_axis[ii], &box_blueprint_.obb_axis[ii], &mat_rotate);
	}

	matWorld_._41 = 0.0f;
	matWorld_._42 = 0.0f;
	matWorld_._43 = 0.0f;
	matWorld_ *= mat_rotate;
	matWorld_._41 = vec_pos3.x;
	matWorld_._42 = vec_pos3.y;
	matWorld_._43 = vec_pos3.z;

	UpdatePosition();
}

void PBoxObject::MoveBox(D3DXVECTOR3& dir_vec, float speed)
{
	float variable_speed = speed * g_SecondPerFrame;
	box_blueprint_.center.x += dir_vec.x * speed;
	box_blueprint_.center.y += dir_vec.y * speed;
	box_blueprint_.center.z += dir_vec.z * speed;

	matWorld_._41 = box_blueprint_.center.x;
	matWorld_._42 = box_blueprint_.center.y;
	matWorld_._43 = box_blueprint_.center.z;

	UpdatePosition();

}

void PBoxObject::ScaleBox(D3DXVECTOR3& scl_vec)
{
	D3DXMATRIX mat_scale;

	D3DXVECTOR3 matscl, mattrans;
	D3DXQUATERNION quatrot;
	D3DXMatrixDecompose(&matscl, &quatrot, &mattrans, &matWorld_);

	D3DXMatrixScaling(&mat_scale, 1.0f + g_SecondPerFrame * scl_vec.x, 1.0f + g_SecondPerFrame * scl_vec.y, 1.0f + g_SecondPerFrame * scl_vec.z);
	
	D3DXVECTOR3 v_scale = D3DXVECTOR3(box_blueprint_.obb_extents[0], box_blueprint_.obb_extents[1], box_blueprint_.obb_extents[2]);
	D3DXVec3TransformNormal(&v_scale, &v_scale, &mat_scale);
	box_blueprint_.obb_extents[0] = v_scale.x;
	box_blueprint_.obb_extents[1] = v_scale.y;
	box_blueprint_.obb_extents[2] = v_scale.z;
	

	D3DXVECTOR3 vec_pos3 = D3DXVECTOR3(matWorld_._41, matWorld_._42, matWorld_._43); 

	matWorld_._41 = 0.0f;
	matWorld_._42 = 0.0f;
	matWorld_._43 = 0.0f;

	matWorld_ *= mat_scale;

	matWorld_._41 = vec_pos3.x;
	matWorld_._42 = vec_pos3.y;
	matWorld_._43 = vec_pos3.z;

	UpdatePosition();

}

bool PBoxObject::UpdatePosition()
{
	D3DXVECTOR3 x_axis_extent = box_blueprint_.obb_axis[0] * box_blueprint_.obb_extents[0];
	D3DXVECTOR3 y_axis_extent = box_blueprint_.obb_axis[1] * box_blueprint_.obb_extents[1];
	D3DXVECTOR3 z_axis_extent = box_blueprint_.obb_axis[2] * box_blueprint_.obb_extents[2];

	box_blueprint_.box_pos[0] = box_blueprint_.center - x_axis_extent - y_axis_extent - z_axis_extent;
	box_blueprint_.box_pos[1] = box_blueprint_.center - x_axis_extent + y_axis_extent - z_axis_extent;
	box_blueprint_.box_pos[2] = box_blueprint_.center + x_axis_extent + y_axis_extent - z_axis_extent;
	box_blueprint_.box_pos[3] = box_blueprint_.center + x_axis_extent - y_axis_extent - z_axis_extent;
	box_blueprint_.box_pos[4] = box_blueprint_.center - x_axis_extent - y_axis_extent + z_axis_extent;
	box_blueprint_.box_pos[5] = box_blueprint_.center - x_axis_extent + y_axis_extent + z_axis_extent;
	box_blueprint_.box_pos[6] = box_blueprint_.center + x_axis_extent + y_axis_extent + z_axis_extent;
	box_blueprint_.box_pos[7] = box_blueprint_.center + x_axis_extent - y_axis_extent + z_axis_extent;

	box_blueprint_.aabb_min = box_blueprint_.box_pos[0];
	box_blueprint_.aabb_max = box_blueprint_.box_pos[0];
	//min, max 비교하여 결정
	for (int vert = 1; vert < 8; vert++)
	{
		if (box_blueprint_.aabb_max.x < box_blueprint_.box_pos[vert].x)
			box_blueprint_.aabb_max.x = box_blueprint_.box_pos[vert].x;
		if (box_blueprint_.aabb_max.y < box_blueprint_.box_pos[vert].y)
			box_blueprint_.aabb_max.y = box_blueprint_.box_pos[vert].y;
		if (box_blueprint_.aabb_max.z < box_blueprint_.box_pos[vert].z)
			box_blueprint_.aabb_max.z = box_blueprint_.box_pos[vert].z;

		if (box_blueprint_.aabb_min.x > box_blueprint_.box_pos[vert].x)
			box_blueprint_.aabb_min.x = box_blueprint_.box_pos[vert].x;
		if (box_blueprint_.aabb_min.y > box_blueprint_.box_pos[vert].y)
			box_blueprint_.aabb_min.y = box_blueprint_.box_pos[vert].y;
		if (box_blueprint_.aabb_min.z > box_blueprint_.box_pos[vert].z)
			box_blueprint_.aabb_min.z = box_blueprint_.box_pos[vert].z;
	}
	return true;
}

PImportObject::PImportObject()
{
	elapsed_time_ = 0.0f;
	mytype = FILE_EXTENSION_TYPE::OBJECT;
}

PImportObject::~PImportObject()
{
}

bool PImportObject::Init(ID3D11Device* device, ID3D11DeviceContext* context, std::wstring vs_file_path, 
	std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name, std::wstring object_path, std::wstring texcomp_path)
{
	std::vector<MaxExportInfo> info;
	std::vector<Material> matrl_info;
	MaxScene scene;

	PModel::Init(device, context);
	PParser parse;

	parse.MaxExportParse(info, matrl_info, scene ,object_path, texcomp_path, device, FILE_EXTENSION_TYPE::SKM);

	object_list_.resize(info.size());

	for (int k = 0; k < info.size(); k++)
	{
		object_list_[k].info = std::move(info[k]);
	}

	for (int i = 0; i < info.size(); i++) //오브젝트별 부모 정보 이식
	{
		auto it = std::find_if(object_list_.begin(), object_list_.end(),
			[&](const PGeoMesh& mesh) {return mesh.info.meshinfo.meshlist_name == object_list_[i].info.meshinfo.parent_name; });

		if (it == object_list_.end())
			object_list_[i].parent_geomesh = nullptr;
		else
			object_list_[i].parent_geomesh = &(*it);

	}

	for (int i = 0; i < info.size(); i++) //오브젝트 정보 이식
	{
		
		D3DXMATRIX mat_parent, mat_inv_parent;
		D3DXMatrixIdentity(&mat_parent);

		if (object_list_[i].parent_geomesh != nullptr)
		{
			mat_parent = object_list_[i].parent_geomesh->info.meshinfo.world_mat;
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


	}

	material_list_ = std::move(matrl_info); //머터리얼 정보 이식
	scene_ = std::move(scene);

	Create(device_, immediate_context_, vs_file_path, vs_func_name, ps_file_path, ps_func_name);


	return true;
}

bool PImportObject::Frame()
{
	elapsed_time_ += 1.0f * g_SecondPerFrame * scene_.frame_rate * scene_.tick_per_frame;

	float animation_end_time = scene_.last_frame * scene_.tick_per_frame;

	if (elapsed_time_ >= animation_end_time)
		elapsed_time_ = 0.0f;


	D3DXMATRIX pseudo_mat_parent;
	D3DXMatrixIdentity(&pseudo_mat_parent);
	for (int obj = 0; obj < object_list_.size(); obj++)
	{

		if (object_list_[obj].parent_geomesh) // parent node가 존재할 경우(상속되는 본 애니메이션)
		{
			Interpolate(object_list_[obj], object_list_[obj].parent_geomesh->mat_calculation, elapsed_time_);
		}
		else
		{
			Interpolate(object_list_[obj], pseudo_mat_parent, elapsed_time_);
		}
	}

	return true;
}

HRESULT PImportObject::CreateVertexBuffer()
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
			cur_dxhelper.vertex_size_ = sizeof(Vertex_PNCT);
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

HRESULT PImportObject::CreateIndexBuffer()
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
				sizeof(int),false));

			if (cur_dxhelper.index_buffer_.Get() == nullptr)
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT PImportObject::CreateVertexData()
{
	int object_size = object_list_.size();

	for (int obj = 0; obj < object_size; obj++)
	{
		auto& cur_object = object_list_[obj];
		cur_object.vertices_list.resize(cur_object.info.vertex_list.size());
		for (int i = 0; i < cur_object.info.vertex_list.size(); i++)
		{
			cur_object.vertices_list[i] = std::move(cur_object.info.vertex_list[i]);
		}
	}
	return S_OK;
}

HRESULT PImportObject::CreateIndexData()
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

bool PImportObject::PostRender()
{

	for (int obj = 0; obj < object_list_.size(); obj++)
	{

		//constant_data_.matWorld = object_list_[obj].mat_inverse_world * object_list_[obj].mat_calculation;
		constant_data_.matWorld = object_list_[obj].mat_inverse_world * object_list_[obj].mat_calculation;

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

bool PImportObject::GetAnimationTrack(const float elapsed_time, const std::vector<PAnimTrack>& tracklist, 
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

void PImportObject::Interpolate(PGeoMesh& mesh, D3DXMATRIX& mat_parent, float elapsed_time)
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

bool PLineObject::Init(ID3D11Device* device, ID3D11DeviceContext* context, std::wstring shader_path)
{
	device_ = device;
	immediate_context_ = context;
	Create(device_, immediate_context_, shader_path, "VS", shader_path, "PS");
	return true;
}

HRESULT PLineObject::CreateVertexData()
{
	vertex_list_.resize(2);
	vertex_list_[0].pos =		D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex_list_[0].normal =	D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex_list_[0].color =		D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	vertex_list_[0].uv	=		D3DXVECTOR2(0.0f, 0.0f);

	vertex_list_[1].pos =	 D3DXVECTOR3(1000.0f, 0.0f, 0.0f);
	vertex_list_[1].normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex_list_[1].color =	 D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
	vertex_list_[1].uv =	 D3DXVECTOR2(1.0f, 0.0f);

	return S_OK;
}

bool PLineObject::PostRender()
{
	immediate_context_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	dx_helper_.PostRender(immediate_context_, dx_helper_.index_count_);
	return true;
}

void PLineObject::Draw(D3DXVECTOR3 v0, D3DXVECTOR3 v1, D3DXVECTOR4 color)
{
	vertex_list_[0].pos = v0;
	vertex_list_[0].normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex_list_[0].color = color;
	vertex_list_[0].uv = D3DXVECTOR2(0.0f, 0.0f);

	vertex_list_[1].pos = v1;
	vertex_list_[1].normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex_list_[1].color = color;
	vertex_list_[1].uv = D3DXVECTOR2(1.0f, 0.0f);

	immediate_context_->UpdateSubresource(dx_helper_.vertex_buffer_.Get(), 0,
		NULL, &vertex_list_.at(0), 0, 0);

	float time = g_fGameTimer;
	constant_data_.color[0] = cosf(time);
	constant_data_.color[1] = sinf(time);
	constant_data_.color[2] = 1 - cosf(time);
	constant_data_.color[3] = 1.0f;
	constant_data_.etc[0] = time;
	immediate_context_->UpdateSubresource(
		dx_helper_.constant_buffer_.Get(),
		0, NULL, &constant_data_, 0, 0);

	dx_helper_.PreRender(immediate_context_, dx_helper_.vertex_size_);
	PostRender();

}

PLineObject::PLineObject()
{
}

PLineObject::~PLineObject()
{
}
