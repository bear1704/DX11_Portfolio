#include "Sample.h"


Sample::Sample()
{

}

Sample::~Sample()
{

}


bool Sample::Init()
{
	
	free_camera_.Init(device_, immediate_device_context_, L"DiffuseLight.hlsl", "VS", L"DiffuseLight.hlsl", "PS");
	free_camera_.camera_position_ = D3DXVECTOR3(0.0f, 10.0f, -50.0f);
	free_camera_.vec_view_target_ = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	free_camera_.CreateTargetViewMatrix(free_camera_.camera_position_,
		free_camera_.vec_view_target_, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	free_camera_.CreateProjectionMatrix();
	main_camera_ = &free_camera_;
	light_obj_.Init(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1),
		D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), device_, immediate_device_context_, main_camera_);

	rt_screen_.Init(device_, immediate_device_context_, L"DiffuseLight.hlsl", "VS", L"DiffuseLight.hlsl", "PS_NoLight");
	minimap_rt_.Create(device_, 1024, 1024);

	box_obj_.Init(device_, immediate_device_context_, L"DiffuseLight.hlsl", "VS", L"DiffuseLight.hlsl", "PS", L"blue");
	line_obj_.Init(device_, immediate_device_context_, L"LineShader.hlsl");
	
	quadtree_.Build(50.0f, 50.0f, 5, 1.0f);

	std::random_device rd;
	std::mt19937 mt(rd());
	uniform_int_distribution<int> distribution(1, 40);          // 생성 범위
	auto generator = bind(distribution, mt);

	for (int i = 0; i < 10; i++)
	{
		P_BOX* box = (P_BOX*) &random_map_obj_[i];
		box->pos.x = -20.0f + generator();
		box->pos.z = -20.0f + generator();
		box->pos.y = 0.0f;

		box->aabb_min.x = box->pos.x - 1.0f;
		box->aabb_min.y = box->pos.y - 1.0f;
		box->aabb_min.z = box->pos.z - 1.0f;
		
		box->aabb_max.x = box->pos.x + 1.0f;
		box->aabb_max.y = box->pos.y + 1.0f;
		box->aabb_max.z = box->pos.z + 1.0f;
		D3DXMatrixTranslation(&box->mat_world, box->pos.x, box->pos.y, box->pos.z);

		quadtree_.AddObject(box);
	}
	return true;
}

bool Sample::Frame()
{


#pragma region KEY
	if (g_InputActionMap.qKey == KEYSTAT::KEY_HOLD)
	{
		main_camera_->UpWard();
	}
	if (g_InputActionMap.eKey == KEYSTAT::KEY_HOLD)
	{
		main_camera_->DownWard();
	}
	if (g_InputActionMap.aKey == KEYSTAT::KEY_HOLD)
	{
		main_camera_->MoveLeft();
	}
	if (g_InputActionMap.dKey == KEYSTAT::KEY_HOLD)
	{
		main_camera_->MoveRight();
	}
	if (g_InputActionMap.wKey == KEYSTAT::KEY_HOLD)
	{
		main_camera_->MoveFoward();
	}
	if (g_InputActionMap.sKey == KEYSTAT::KEY_HOLD)
	{
		main_camera_->MoveBackward();
	}
	if (g_InputActionMap.middleClick == KEYSTAT::KEY_PUSH)
	{
		ToggleIsWireframeRender();
	}

#pragma endregion
	light_obj_.Frame();

	main_camera_->Frame();
	quadtree_.Update(main_camera_);
	quadtree_.Frame();
	return true;
}

bool Sample::Render()
{
	DX::ApplySamplerState(immediate_device_context_, DX::PDxState::sampler_state_anisotropic);
	light_obj_.Render();
	line_obj_.SetWVPMatrix(nullptr, (D3DXMATRIX*)&main_camera_->matView_, (D3DXMATRIX*)&main_camera_->matProj_);
	
	
	for (int i = 0; i < quadtree_.drawnode_list_.size(); i++)
	{
		DrawQuadTree(quadtree_.drawnode_list_[i]);
	}

	for (int i = 0; i < quadtree_.drawobj_list_.size(); i++)
	{
		P_BOX* pbox = (P_BOX*)quadtree_.drawobj_list_[i];
		box_obj_.SetWVPMatrix(&pbox->mat_world, (D3DXMATRIX*)& main_camera_->matView_, (D3DXMATRIX*)& main_camera_->matProj_);
		box_obj_.Render();
	}

	D3DXMATRIX mat_topview;
	D3DXMatrixLookAtLH(&mat_topview, &D3DXVECTOR3(0, 100, 0), &D3DXVECTOR3(0, 0, 0.1f), &D3DXVECTOR3(0, 1, 0));

	minimap_rt_.Begin(immediate_device_context_);
	{
		for (int i = 0; i < quadtree_.drawobj_list_.size(); i++)
		{
			P_BOX* pbox = (P_BOX*)quadtree_.drawobj_list_[i];
			box_obj_.SetWVPMatrix(&pbox->mat_world, &mat_topview, (D3DXMATRIX*)& main_camera_->matProj_);
			box_obj_.Render();
		}
		main_camera_->SetWVPMatrix(nullptr,
			&mat_topview,
			&main_camera_->matProj_);
		main_camera_->Render(immediate_device_context_);

		minimap_rt_.End(immediate_device_context_);
	}

	DX::ApplyDepthStencilState(immediate_device_context_, DX::PDxState::depth_stencil_state_disable_);
	DX::ApplyBlendState(immediate_device_context_, DX::PDxState::blend_state_alphablend_disable_);
	rt_screen_.vertex_list_[0].pos = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	rt_screen_.vertex_list_[1].pos = D3DXVECTOR3(-0.5f, 1.0f, 0.0f);
	rt_screen_.vertex_list_[2].pos = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
	rt_screen_.vertex_list_[3].pos = D3DXVECTOR3(-1.0f, 0.5f, 0.0f);

	immediate_device_context_->UpdateSubresource(rt_screen_.dx_helper_.vertex_buffer_.Get(), 
		0, NULL, &rt_screen_.vertex_list_.at(0), 0, 0);
	rt_screen_.SetWVPMatrix(nullptr, nullptr, nullptr);
	rt_screen_.PreRender();
	immediate_device_context_->PSSetShaderResources(0, 1, minimap_rt_.shader_res_view_.GetAddressOf());
	rt_screen_.PostRender();


	return true;

}

bool Sample::Release()
{
	rt_screen_.Release();
	minimap_rt_.Release();
	quadtree_.Release();
	line_obj_.Release();
	return true;
}

void Sample::MessageProc(MSG msg)
{
	PCore::MessageProc(msg);
	//PInput::GetInstance().MsgProc(msg);
	if (main_camera_ == nullptr) return;
	main_camera_->MessageProc(msg);
}

bool Sample::DrawQuadTree(PNode* node)
{
	if (node == nullptr) return true;

	D3DXVECTOR4 color = D3DXVECTOR4(1, 1, 1, 1);
	if (node->depth_ == 1)
		color = D3DXVECTOR4(1, 0, 0, 1);
	if (node->depth_ == 2)
		color = D3DXVECTOR4(0, 1, 0, 1);
	if (node->depth_ == 3)
		color = D3DXVECTOR4(0, 0, 1, 1);
	if (node->depth_ == 4)
		color = D3DXVECTOR4(1, 1, 0, 0);
	if (node->depth_ == 5)
		color = D3DXVECTOR4(1, 0, 1, 1);

	D3DXVECTOR3 max = node->box_blueprint_.aabb_max;
	D3DXVECTOR3 min = node->box_blueprint_.aabb_min;

	D3DXVECTOR3 start = D3DXVECTOR3(min.x, node->depth_ * 0.1f, max.z);
	D3DXVECTOR3 end = D3DXVECTOR3(max.x, node->depth_ * 0.1f, max.z);
	line_obj_.Draw(start, end, color);

	start = D3DXVECTOR3(max.x, node->depth_ * 0.1f, max.z);
	end = D3DXVECTOR3(max.x, node->depth_ * 0.1f, min.z);
	line_obj_.Draw(start, end, color);

	start = D3DXVECTOR3(max.x, node->depth_ * 0.1f, min.z);
	end = D3DXVECTOR3(min.x, node->depth_ * 0.1f, min.z);
	line_obj_.Draw(start, end, color);

	start = D3DXVECTOR3(min.x, node->depth_ * 0.1f, min.z);
	end = D3DXVECTOR3(min.x, node->depth_ * 0.1f, max.z);
	line_obj_.Draw(start, end, color);

	for (int i = 0; i < node->object_list_.size(); i++)
	{
		P_BOX* box_bp = (P_BOX*)node->object_list_[i];
		box_obj_.SetWVPMatrix(
			&box_bp->mat_world,
			(D3DXMATRIX*)& main_camera_->matView_,
			(D3DXMATRIX*)& main_camera_->matProj_);
		box_obj_.Render();
	}

	for (int inode = 0; inode < node->child_list_.size(); inode++)
	{
		DrawQuadTree(node->child_list_[inode]);
	}

}

