#include "Sample.h"


Sample::Sample()
{

}

Sample::~Sample()
{

}


bool Sample::Init()
{
	
	free_camera_.Init(device_, immediate_device_context_, L"data/Shader/DiffuseLight.hlsl", "VS", L"data/Shader/DiffuseLight.hlsl", "PS");
	free_camera_.camera_position_ = D3DXVECTOR3(0.0f, 10.0f, -50.0f);
	free_camera_.vec_view_target_ = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	free_camera_.CreateTargetViewMatrix(free_camera_.camera_position_,
		free_camera_.vec_view_target_, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	free_camera_.CreateProjectionMatrix();
	main_camera_ = &free_camera_;
	
	light_obj_.Init(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1),
		D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR3(100.0f, 200.0f, 0.0f), 1, device_, immediate_device_context_, main_camera_);

	rt_screen_.Init(device_, immediate_device_context_, L"data/Shader/DiffuseLight.hlsl", "VS", L"data/Shader/DiffuseLight.hlsl", "PS_NoLight");
	minimap_rt_.Create(device_, 1024, 1024);

	//box_obj_.OBBInit(device_, immediate_device_context_, L"data/Shader/VertexShader.hlsl", "VS", L"data/Shader/PixelShader.hlsl", "PS",
	//	5, 1, 1, D3DXVECTOR3(0.0f, 35.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), 
	//	L"blue");
	//box_obj2_.OBBInit(device_, immediate_device_context_, L"data/Shader/VertexShader.hlsl", "VS", L"data/Shader/PixelShader.hlsl", "PS",
	//	4, 4, 1, D3DXVECTOR3(10.0f, 35.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f),
	//	L"blue");

	box_obj_.OBBInit(device_, immediate_device_context_, L"data/Shader/DiffuseLight.hlsl", "VS", L"data/Shader/DiffuseLight.hlsl", "PS",
		5, 1, 1, D3DXVECTOR3(0.0f, 35.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), 
		L"blue");
	box_obj2_.OBBInit(device_, immediate_device_context_, L"data/Shader/DiffuseLight.hlsl", "VS", L"data/Shader/DiffuseLight.hlsl", "PS",
		4, 4, 1, D3DXVECTOR3(10.0f, 35.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f),
		L"blue");




	//D3DXMATRIX mat_rot;
	//D3DXMatrixRotationYawPitchRoll(&mat_rot, 0.0f, 0.0f, 30.8f);
	//box_obj_.matWorld_ *= mat_rot;

	map_.Init(device_, immediate_device_context_, &light_obj_);
	map_.CreateHeightMap(device_, immediate_device_context_, L"data/texture/heightMap513.bmp");


	PMapDesc md;
	md.vertex_cols = map_.vertex_cols();
	md.vertex_rows = map_.vertex_rows();
	md.cell_disatnce = 1;
	md.vs_path = L"data/Shader/NormalMap.hlsl";
	md.vs_func = "VS";
	md.ps_path = L"data/Shader/NormalMap.hlsl";
	md.ps_func = "PS";
	md.texture_name = L"stone_wall";

	map_.SetNormalTexture(L"test");
	if (map_.Load(md) == false)
		assert(false);

	quadtree_.Build(&map_, 7, 10.0f);



	//프러스텀 표시/비표시
	main_camera_->render_frustum_ = false;

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
	if (g_InputActionMap.DeleteKey == KEYSTAT::KEY_HOLD)
	{
		box_obj_.KeyRotate(4.0f, 0.0f, 0.0f);
	}
	if (g_InputActionMap.EndKey == KEYSTAT::KEY_HOLD)
	{
		box_obj_.KeyRotate(0.0f, 4.0f, 0.0f);
	}
	if (g_InputActionMap.PageDownKey == KEYSTAT::KEY_HOLD)
	{
		box_obj_.KeyRotate(0.0f, 0.0f, 4.0f);
	}
	if (g_InputActionMap.upArrowKey == KEYSTAT::KEY_HOLD)
	{
		box_obj_.MoveBox(main_camera_->vec_up_, kObjectMoveSpeed);
	}
	if (g_InputActionMap.downArrowKey == KEYSTAT::KEY_HOLD)
	{
		D3DXVECTOR3 vec = -(main_camera_->vec_up_);
		box_obj_.MoveBox(vec, kObjectMoveSpeed);
	}
	if (g_InputActionMap.leftArrowKey == KEYSTAT::KEY_HOLD)
	{
		D3DXVECTOR3 vec = -(main_camera_->vec_right_);
		box_obj_.MoveBox(vec, kObjectMoveSpeed);
	}
	if (g_InputActionMap.rightArrowKey == KEYSTAT::KEY_HOLD)
	{
		box_obj_.MoveBox(main_camera_->vec_right_, kObjectMoveSpeed);
	}
	if (g_InputActionMap.PageUpKey == KEYSTAT::KEY_HOLD)
	{
		D3DXVECTOR3 vec = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		box_obj_.ScaleBox(vec);
	}
	if (g_InputActionMap.InsertKey == KEYSTAT::KEY_HOLD)
	{
		D3DXVECTOR3 vec = -(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
		box_obj_.ScaleBox(vec);
	}



#pragma endregion
	light_obj_.Frame();

	box_obj_.Frame();
	box_obj2_.Frame();
	main_camera_->Frame();
	quadtree_.Update(main_camera_);
	quadtree_.Frame();
	map_.Frame(light_obj_.light_direction(), main_camera_->camera_position_, main_camera_->vec_look_);

	return true;
}

bool Sample::Render()
{
	DX::ApplySamplerState(immediate_device_context_, DX::PDxState::sampler_state_anisotropic);
	//line_obj_.SetWVPMatrix(nullptr, (D3DXMATRIX*)&main_camera_->matView_, (D3DXMATRIX*)&main_camera_->matProj_);
	light_obj_.Render();


	map_.SetWVPMatrix(nullptr, (D3DXMATRIX*) &main_camera_->matView_, (D3DXMATRIX*) &main_camera_->matProj_);
	quadtree_.Render(immediate_device_context_);

	if (PCollision::GetInstance().CheckOBBtoObb(box_obj_.box_blueprint_, box_obj2_.box_blueprint_) ==  true)
	{
		box_obj_.texture_ = PTextureManager::GetInstance().GetTextureFromMap(L"green");
		box_obj_.dx_helper_.shader_res_view_.Detach();
		box_obj_.dx_helper_.shader_res_view_.Attach(box_obj_.texture_->shader_res_view());
	}
	else
	{
		box_obj_.texture_ = PTextureManager::GetInstance().GetTextureFromMap(L"blue");
		box_obj_.dx_helper_.shader_res_view_.Detach();
		box_obj_.dx_helper_.shader_res_view_.Attach(box_obj_.texture_->shader_res_view());
	}

	box_obj_.SetWVPMatrix((D3DXMATRIX*)& box_obj_.matWorld_, (D3DXMATRIX*)& main_camera_->matView_, (D3DXMATRIX*)& main_camera_->matProj_);
	box_obj_.Render();

	box_obj2_.SetWVPMatrix((D3DXMATRIX*)& box_obj2_.matWorld_, (D3DXMATRIX*)& main_camera_->matView_, (D3DXMATRIX*)& main_camera_->matProj_);
	box_obj2_.Render();

	main_camera_->Render(immediate_device_context_);

	D3DXMATRIX mat_topview;
	D3DXMatrixLookAtLH(&mat_topview, &D3DXVECTOR3(0, 500, 0), &D3DXVECTOR3(0, 0, 0.1f), &D3DXVECTOR3(0, 1, 0));

	minimap_rt_.Begin(immediate_device_context_);
	{

		map_.SetWVPMatrix(nullptr, (D3DXMATRIX*)&mat_topview, (D3DXMATRIX*)& main_camera_->matProj_);
		quadtree_.Render(immediate_device_context_);
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



	//PreRender();
	//PostRender();
	return true;

}

bool Sample::Release()
{
	rt_screen_.Release();
	minimap_rt_.Release();
	quadtree_.Release();
	line_obj_.Release();
	map_.Release();
	box_obj_.Release();
	box_obj2_.Release();
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
	if (node->depth_ == 6)
		color = D3DXVECTOR4(1, 1, 1, 1);
	if (node->depth_ >= 7)
		color = D3DXVECTOR4(0, 0, 0, 1);

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

