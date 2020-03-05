#include "Sample.h"

int g_temp_color = 1;

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
	
	light_obj_.Init(D3DXVECTOR4(0.3f, 0.3f, 0.3f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f),
		D3DXVECTOR4(1, 1, 1, 1),
		D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), 
		D3DXVECTOR3(0.0f, 500.0f, -500.0f), 1, device_, immediate_device_context_, main_camera_, true);

	rt_screen_.Init(device_, immediate_device_context_, L"data/Shader/DiffuseLight.hlsl", "VS", L"data/Shader/DiffuseLight.hlsl", "PS_NoLight");
	minimap_rt_.Create(device_, 1024, 1024);

	box_obj_.OBBInit(device_, immediate_device_context_, L"data/Shader/DiffuseLight.hlsl", "VS", L"data/Shader/DiffuseLight.hlsl", "PS",
		5, 1, 1, D3DXVECTOR3(0.0f, 35.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), 
		L"blue");
	box_obj_.object_name_ = L"box1";
	box_obj2_.OBBInit(device_, immediate_device_context_, L"data/Shader/DiffuseLight.hlsl", "VS", L"data/Shader/DiffuseLight.hlsl", "PS",
		4, 4, 1, D3DXVECTOR3(10.0f, 35.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f),
		L"blue");
	box_obj2_.object_name_ = L"box2";

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

	map_.SetNormalTexture(L"stone_wall");
	if (map_.Load(md) == false)
		assert(false);

	quadtree_.Build(&map_, 5, 10.0f);

	//프러스텀 표시/비표시
	main_camera_->render_frustum_ = false;

	line_obj_.Init(device_, immediate_device_context_, L"data/Shader/LineShader.hlsl");


	//Animation
	std::vector<StringPair> vec;
	parse_.CharacterSheetParse(L"data/Character/reg.sheet", &vec);


	for (auto iter = vec.begin(); iter != vec.end(); iter++)
	{
		if (iter->first == "CharacterName")
			character_.character_name_.assign(iter->second.begin(), iter->second.end());
		else if (iter->first == "ShaderFileName")
			character_.shader_name_.assign(iter->second.begin(), iter->second.end());
		else if (iter->first == "MatrixFileName")
		{
			multibyte_string matrix_file;
			matrix_file.assign(iter->second.begin(), iter->second.end());
			character_.matrix_ = static_cast<PMatObj*>(LoadSheetObject(matrix_file, L"data/Shader/DiffuseLight.hlsl", "VS",
				L"data/Shader/DiffuseLight.hlsl", "PS"));
			character_.matrix_->action_animlist_.insert(make_pair("walk", PAction(0, 425)));
		}
		else if (iter->first == "SkinUpperBody")
		{
			multibyte_string skm_file;
			skm_file.assign(iter->second.begin(), iter->second.end());
			std::vector<string> pstring = parse_.SplitString(skm_file, ',');
			skm_file.assign(pstring[0].begin(), pstring[0].end());
			PSkmObj* obj = static_cast<PSkmObj*>(LoadSheetObject(skm_file, character_.shader_name_,
				"VS", character_.shader_name_, "PS"));
			character_.object_list_.push_back(obj);
		}
		else if (iter->first == "SkinWeapon")
		{
			multibyte_string skm_file;
			skm_file.assign(iter->second.begin(), iter->second.end());
			std::vector<string> pstring = parse_.SplitString(skm_file, ',');
			skm_file.assign(pstring[0].begin(), pstring[0].end());
			PSkmObj* obj = static_cast<PSkmObj*>(LoadSheetObject(skm_file, character_.shader_name_, "VS",
				character_.shader_name_, "PS"));
			character_.object_list_.push_back(obj);
		}
		else if (iter->first == "TexturePath")
			object_file_texture_path_.assign(iter->second.begin(), iter->second.end());
		else if (iter->first == "ObjectFileName")
		{
			multibyte_string obj_file;
			obj_file.assign(iter->second.begin(), iter->second.end());
			std::vector<string> pstring = parse_.SplitString(obj_file, ',');
			obj_file.assign(pstring[0].begin(), pstring[0].end());
			PImportObject* obj = static_cast<PImportObject*>(LoadSheetObject(obj_file, character_.shader_name_, "VS",
				character_.shader_name_, "PS", object_file_texture_path_));
			character_.object_list_.push_back(obj);
		}

	}

	shadow_rt_.depthstencil_format_ = DXGI_FORMAT_R32_TYPELESS;
	shadow_rt_.Create(device_, 1024, 768);
	D3DXMatrixIdentity(&mat_shadow_view_);
	

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
	if (g_InputActionMap.jumpKey == KEYSTAT::KEY_PUSH)
	{
		if (++g_temp_color > 7)
			g_temp_color = 1;
	}


#pragma endregion
	light_obj_.Frame();

	box_obj_.Frame();
	box_obj2_.Frame();
	main_camera_->Frame();
	quadtree_.Update(main_camera_);
	quadtree_.Frame();
	map_.Frame(light_obj_.light_direction(), main_camera_->camera_position_, main_camera_->vec_look_);
	select_.SetMatrix(nullptr, &main_camera_->matView_, &main_camera_->matProj_);

	PMatObj* matobj = character_.matrix_;

	if (matobj)
	{
		auto action_iter = matobj->action_animlist_.find("walk");
		PAction act = (*action_iter).second;

		int start = act.start;
		int end = act.end;

		elapsed_time_ += 1.0f * g_SecondPerFrame * matobj->scene_.frame_rate * matobj->scene_.tick_per_frame;
		float end_time = end * 160.0f;
		float elapse_time = start + elapsed_time_;

		if (elapse_time > end_time)
		{
			elapse_time = 0;
			elapsed_time_ = 0;
		}

		D3DXMATRIX* matrix = matobj->FrameMatrix(start, end, elapse_time);
		for (auto& obj : character_.object_list_)
		{
			obj->Frame(matrix);
		}
	}
	//matobj가 없다는건 오브젝트 애니메이션이라는 의미이다.
	else
	{
		for (auto& obj : character_.object_list_)
		{
			obj->Frame();
		}
	}


	return true;
}

bool Sample::Render()
{
	DX::ApplySamplerState(immediate_device_context_, DX::PDxState::sampler_state_anisotropic);
	//line_obj_.SetWVPMatrix(nullptr, (D3DXMATRIX*)&main_camera_->matView_, (D3DXMATRIX*)&main_camera_->matProj_);
	light_obj_.Render();

	/*for (int i = 0; i < quadtree_.drawnode_list_.size(); i++)
	{
		DrawQuadTree(quadtree_.drawnode_list_[i]);
		
	}*/


	//DrawQuadTree(quadtree_.rootnode_);
	
	
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
	

	if (g_InputActionMap.rightClick == KEYSTAT::KEY_HOLD || g_InputActionMap.rightClick == KEYSTAT::KEY_PUSH)
	{
		if (select_.CheckRaytoOBBCollision(&box_obj_.box_blueprint_))
		{
			TCHAR szBuffer[256];
			_stprintf_s(szBuffer, _T(" %s 박스와 직선의 충돌, 교점=(%10.4f, %10.4f, %10.4f)"),
				box_obj_.object_name_.c_str(), select_.intersection_.x, select_.intersection_.y, select_.intersection_.z);
			MessageBox(0, szBuffer, _T("충돌"), MB_OK);
		}
		if (select_.CheckRaytoOBBCollision(&box_obj2_.box_blueprint_))
		{
			TCHAR szBuffer[256];
			_stprintf_s(szBuffer, _T(" %s 박스와 직선의 충돌, 교점=(%10.4f, %10.4f, %10.4f)"),
				box_obj2_.object_name_.c_str(), select_.intersection_.x, select_.intersection_.y, select_.intersection_.z);
			MessageBox(0, szBuffer, _T("충돌"), MB_OK);
		}

		line_start_ = select_.picking_ray_.origin;
		line_end_ = select_.picking_ray_.origin + select_.picking_ray_.direction * select_.t_min;
	}
	line_obj_.SetWVPMatrix(nullptr, (D3DXMATRIX*)&main_camera_->matView_, (D3DXMATRIX*)&main_camera_->matProj_);
	line_obj_.Draw(line_start_,line_end_, D3DXVECTOR4(1, 0, 0, 1));


	main_camera_->Render(immediate_device_context_);


	//Animation
	for (auto obj : object_list_)
	{
		obj->PreRender();
		
		//애니메이션 오브젝트 위치조정을 위함
		D3DXMATRIX wd;
		D3DXMATRIX scl;
		D3DXMatrixIdentity(&wd);
		D3DXMatrixScaling(&scl, 5.0f, 5.0f, 5.0f);
		wd *= scl;
		wd._42 += 20.0f;
		wd._41 -= 10.0f;

		if (obj->mytype == FILE_EXTENSION_TYPE::SKM)
		{
			
			PSkmObj* skm = (PSkmObj*)obj;
			skm->PreRender();
			skm->SetWVPMatrix((D3DXMATRIX*)&wd, (D3DXMATRIX*)&main_camera_->matView_, (D3DXMATRIX*)&main_camera_->matProj_);
			skm->PostRender();
		}
		else if (obj->mytype == FILE_EXTENSION_TYPE::OBJECT)
		{
			PImportObject* imp = (PImportObject*)obj;
			imp->PreRender();
			imp->SetWVPMatrix((D3DXMATRIX*)&wd, (D3DXMATRIX*)&main_camera_->matView_, (D3DXMATRIX*)&main_camera_->matProj_);
			imp->PostRender();
		}

	}

	//Shadow Render
	shadow_rt_.Begin(immediate_device_context_);
	{
		D3DXVECTOR3 lookat = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXMatrixLookAtLH(&mat_shadow_view_, &light_obj_.light_position(), &lookat, &up);

		box_obj_.SetWVPMatrix((D3DXMATRIX*)&box_obj_.matWorld_, (D3DXMATRIX*)&main_camera_->matView_, (D3DXMATRIX*)&main_camera_->matProj_);
		box_obj_.Render();


		shadow_rt_.End(immediate_device_context_);
	}


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
	else if (node->depth_ == 2)
		color = D3DXVECTOR4(0, 1, 0, 1);
	else if (node->depth_ == 3)
		color = D3DXVECTOR4(0, 0, 1, 1);
	else if (node->depth_ == 4)
		color = D3DXVECTOR4(1, 1, 0, 0);
	else if (node->depth_ == 5)
		color = D3DXVECTOR4(1, 0, 1, 1);
	else if (node->depth_ == 6)
		color = D3DXVECTOR4(1, 1, 1, 1);
	else if (node->depth_ >= 7)
		color = D3DXVECTOR4(0, 0, 0, 1);

	D3DXVECTOR3 max = node->box_blueprint_.aabb_max;
	D3DXVECTOR3 min = node->box_blueprint_.aabb_min;

	D3DXVECTOR3 start = D3DXVECTOR3(min.x, node->depth_ * 0.2f, max.z);
	D3DXVECTOR3 end = D3DXVECTOR3(max.x, node->depth_ * 0.2f, max.z);
	start.y += 30.0f;
	end.y += 30.0f;
	if (node->depth_ == g_temp_color)
	{
		line_obj_.Draw(start, end, color);
	}
	start = D3DXVECTOR3(max.x, node->depth_ * 0.2f, max.z);
	end = D3DXVECTOR3(max.x, node->depth_ * 0.2f, min.z);
	start.y += 29.0f;
	end.y += 29.0f;
	if (node->depth_ == g_temp_color)
	{
		line_obj_.Draw(start, end, color);
	}

	start = D3DXVECTOR3(max.x, node->depth_ * 0.2f, min.z);
	end = D3DXVECTOR3(min.x, node->depth_ * 0.2f, min.z);
	start.y += 28.0f;
	end.y += 28.0f;
	if (node->depth_ == g_temp_color)
	{
		line_obj_.Draw(start, end, color);
	}

	start = D3DXVECTOR3(min.x, node->depth_ * 0.2f, min.z);
	end = D3DXVECTOR3(min.x, node->depth_ * 0.2f, max.z);
	start.y += 27.0f;
	end.y += 27.0f;
	if (node->depth_ == g_temp_color)
	{
		line_obj_.Draw(start, end, color);
	}

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



PModel* Sample::LoadSheetObject(multibyte_string filename, std::wstring vs_shader_path, std::string vs_func_name,
	std::wstring ps_shader_path, std::string ps_func_name, std::wstring texture_path)
{
	FILE_EXTENSION_TYPE file_type = FILE_EXTENSION_TYPE::KGC;
	TCHAR szFileName[256];
	TCHAR Drive[MAX_PATH];
	TCHAR Dir[MAX_PATH];
	TCHAR FName[MAX_PATH];
	TCHAR Ext[MAX_PATH];
	_tsplitpath_s(filename.c_str(),
		Drive, Dir, FName, Ext);

	Ext[5] = 0;
	_stprintf_s(szFileName, _T("%s%s"), FName, Ext);

	multibyte_string file_ext = Ext;
	if (file_ext == L".kgc")
		file_type = FILE_EXTENSION_TYPE::KGC;
	else if (file_ext == L".skm")
		file_type = FILE_EXTENSION_TYPE::SKM;
	else if (file_ext == L".anim")
		file_type = FILE_EXTENSION_TYPE::MAT;
	else if (file_ext == L".png")
		file_type = FILE_EXTENSION_TYPE::OBJECT;

	PModel* model = nullptr;

	if (file_type == FILE_EXTENSION_TYPE::KGC)
		model = new PKgcObj;
	else if (file_type == FILE_EXTENSION_TYPE::SKM)
	{
		model = new PSkmObj;
		model->Init(device_, immediate_device_context_, vs_shader_path, vs_func_name, ps_shader_path, ps_func_name,
			filename.c_str(), L"data/texture/");
	}
	else if (file_type == FILE_EXTENSION_TYPE::MAT)
	{
		model = new PMatObj;
		model->Init(device_, immediate_device_context_, vs_shader_path, vs_func_name, ps_shader_path, ps_func_name,
			filename.c_str(), L"data/texture/");
	}
	else if (file_type == FILE_EXTENSION_TYPE::OBJECT)
	{
		model = new PImportObject;
		model->Init(device_, immediate_device_context_, vs_shader_path, vs_func_name, ps_shader_path, ps_func_name,
			filename.c_str(), texture_path);
	}
	object_list_.push_back(model);
	return model;
}

PSCharacter::PSCharacter()
{
}

PSCharacter::~PSCharacter()
{
}
