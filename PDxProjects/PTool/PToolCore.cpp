#include "pch.h"
#include "PToolCore.h"

PToolCore::PToolCore()
{

}

PToolCore::~PToolCore()
{
}

bool PToolCore::Init()
{
	
	ZeroMemory(&blend_desc_, sizeof(D3D11_BLEND_DESC));
	blend_desc_.AlphaToCoverageEnable = FALSE;
	blend_desc_.IndependentBlendEnable = TRUE;
	blend_desc_.RenderTarget[0].BlendEnable = TRUE;
	blend_desc_.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc_.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc_.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	blend_desc_.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc_.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc_.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	blend_desc_.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = device()->CreateBlendState(&blend_desc_, &blend_state_);
	if (FAILED(hr))
		assert(false);

	PSpriteManager::GetInstance().LoadSpriteDataFromScript(L"data/sprite.txt", ObjectLoadType::ETC_SPRITE);
	screen_tex_object_.Init(device_, immediate_device_context_, L"VertexShader.hlsl", "VS", L"PixelShader.hlsl", "PS", L"blue");
	skybox_.Init(device_, immediate_device_context_, L"Skybox.hlsl", "VS", L"Skybox.hlsl", "PS");


	free_camera_.Init();

	D3DXVECTOR3 eye(0.0f, 0.0f, -30.0f);
	D3DXVECTOR3 at(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);


	free_camera_.CreateTargetViewMatrix(eye, at, up);
	free_camera_.CreateProjectionMatrix();

	main_camera_ = &free_camera_;


	dx_rt_.Create(device_, 800, 600);

	model_light_ = new PLightObj();
	light_obj_.Init(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1),
		D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR3(100.0f ,200.0f, 0.0f), 1,device_, 
		immediate_device_context_, main_camera_, true);
	model_light_->Init(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1),
		D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR3(100.0f, 200.0f, 0.0f), 2, device_, immediate_device_context_, 
		main_camera_, true);
	//CreateConstantBuffer();

	return true;
}

bool PToolCore::Frame()
{

	light_obj_.Frame();

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
	if (g_InputActionMap.leftArrowKey == KEYSTAT::KEY_HOLD)
		main_camera_->RotateLeft();
	if (g_InputActionMap.rightArrowKey == KEYSTAT::KEY_HOLD)
		main_camera_->RotateRight();
	if (g_InputActionMap.upArrowKey == KEYSTAT::KEY_HOLD)
		main_camera_->RotateUp();
	if (g_InputActionMap.downArrowKey == KEYSTAT::KEY_HOLD)
		main_camera_->RotateDown();

#pragma endregion


	main_camera_->Frame();
	for (int ii = 0; ii < effect_plane_.eff_list_.size(); ii++)
	{
		D3DXMatrixInverse(&mat_billboard, NULL, &main_camera_->matView_);
		effect_plane_.eff_list_[ii]->set_mat_bilboard(mat_billboard);
		effect_plane_.eff_list_[ii]->Frame();
	}
	for (int obj = 0; obj < object_list_.size(); obj++)
	{
		object_list_[obj]->Frame();
	}
	return true;
}

bool PToolCore::Render()
{
	//light

	light_obj_.Render();

	//sky
	D3DXMATRIX mat_sky_world;
	D3DXMatrixScaling(&mat_sky_world, 10, 10, 10);
	D3DXMATRIX mat_sky_view = main_camera_->matView_;
	mat_sky_view._41 = 0.0f;
	mat_sky_view._42 = 0.0f;
	mat_sky_view._43 = 0.0f;

#pragma region SCREEN
	dx_rt_.Begin(immediate_device_context_);
	{

		if (is_wireframe_render_) //마우스 휠 여부에 따라 와이어프레임, 솔리드를 가르는 코드
			DX::ApplyRasterizerState(immediate_device_context_, DX::PDxState::rs_state_wireframe_);
		else
			DX::ApplyRasterizerState(immediate_device_context_, DX::PDxState::rs_state_solidframe_);
		//sky

		DX::ApplySamplerState(immediate_device_context_, DX::PDxState::sampler_state_wrap_point);
		
		
		skybox_.SetWVPMatrix(&mat_sky_world, &mat_sky_view, &main_camera_->matProj_);
		skybox_.Render();

		for (int obj = 0; obj < object_list_.size(); obj++)
		{
			object_list_[obj]->SetWVPMatrix((D3DXMATRIX*)&object_list_[obj]->matWorld_, 
				(D3DXMATRIX*)& main_camera_->matView_, (D3DXMATRIX*)& main_camera_->matProj_);
			object_list_[obj]->Render();
		}

		//DX::ApplyRasterizerState(immediate_device_context_, DX::PDxState::rs_state_nocull_);
		
		
		for (int jj = 0 ; jj < effect_plane_.eff_list_.size(); jj++)
		{
			//오브젝트별 알파블렌드 적용 
			blend_desc_.RenderTarget[0].SrcBlend = effect_plane_.eff_list_[jj]->src_blend_;
			blend_desc_.RenderTarget[0].DestBlend = effect_plane_.eff_list_[jj]->dest_blend_;
			blend_desc_.RenderTarget[0].BlendEnable = true;

			device_->CreateBlendState(&blend_desc_, &blend_state_);

			DX::ApplyBlendState(immediate_device_context_, blend_state_);
			effect_plane_.eff_list_[jj]->SetWVPMatrix(&effect_plane_.eff_list_[jj]->matWorld_, &main_camera_->matView_, &main_camera_->matProj_);
			effect_plane_.eff_list_[jj]->Render();
		}

		DX::ApplyRasterizerState(immediate_device_context_, DX::PDxState::rs_state_solidframe_);
		DX::ApplyBlendState(immediate_device_context_, DX::PDxState::blend_state_alphablend_);
		DX::ApplySamplerState(immediate_device_context_, DX::PDxState::sampler_state_anisotropic);
	
	
		dx_rt_.End(immediate_device_context_);
	}
#pragma endregion
	
	DX::ApplyBlendState(immediate_device_context_, DX::PDxState::blend_state_alphablend_disable_);
	DX::ApplyRasterizerState(immediate_device_context_, DX::PDxState::rs_state_solidframe_);
	screen_tex_object_.vertex_list()[0].pos = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	screen_tex_object_.vertex_list()[1].pos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	screen_tex_object_.vertex_list()[2].pos = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	screen_tex_object_.vertex_list()[3].pos = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	
	screen_tex_object_.vertex_list()[0].uv = D3DXVECTOR2(0.0f, 0.0f);
	screen_tex_object_.vertex_list()[1].uv = D3DXVECTOR2(1.0f, 0.0f);
	screen_tex_object_.vertex_list()[2].uv = D3DXVECTOR2(1.0f, 1.0f);
	screen_tex_object_.vertex_list()[3].uv = D3DXVECTOR2(0.0f, 1.0f);
	
	immediate_device_context_->UpdateSubresource(screen_tex_object_.dx_helper().vertex_buffer_.Get(),
		0, NULL, &screen_tex_object_.vertex_list().at(0), 0, 0);
	
	
	screen_tex_object_.SetWVPMatrix(NULL, NULL, NULL);
	screen_tex_object_.PreRender();
	immediate_device_context_->PSSetShaderResources(0, 1,
		dx_rt_.shader_res_view_.GetAddressOf());
	screen_tex_object_.PostRender();
	DX::ApplyBlendState(immediate_device_context_, DX::PDxState::blend_state_alphablend_);
	return true;
}

bool PToolCore::Release()
{

	dx_rt_.Release();
	for (int ii = 0; ii < effect_plane_.eff_list_.size(); ii++)
	{
		effect_plane_.eff_list_[ii]->Release();
	}
	for (int obj = 0; obj < object_list_.size(); obj++)
	{
		object_list_[obj]->Release();
	}
	return true;
}

void PToolCore::MessageProc(MSG msg)
{
	PCore::MessageProc(msg);
	//PInput::GetInstance().MsgProc(msg);
	if (main_camera_ == nullptr) return;
	main_camera_->MessageProc(msg);
}
//
//HRESULT PToolCore::CreateConstantBuffer()
//{
//	HRESULT hr;
//	D3D11_BUFFER_DESC cb_everyframe_desc;
//	cb_everyframe_desc.ByteWidth = sizeof(CB_VS_ChangesEveryFrame);
//	cb_everyframe_desc.Usage = D3D11_USAGE_DYNAMIC;
//	cb_everyframe_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cb_everyframe_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	cb_everyframe_desc.MiscFlags = 0;
//	hr = device_->CreateBuffer(&cb_everyframe_desc, NULL, constant_buffer_changes_everyframe_.GetAddressOf());
//
//	if (FAILED(hr))
//		assert(false);
//
//	cb_everyframe_desc.ByteWidth = sizeof(CB_VS_NearlyNotChange);
//	cb_nearly_not_changes_.cb_AmbientLightColor = D3DXVECTOR4(0.3f, 0.3f, 0.3f, 1);
//	cb_nearly_not_changes_.cb_DiffuseLightColor = D3DXVECTOR4(1, 1, 1, 1);
//	cb_nearly_not_changes_.cb_SpecularLightColor = D3DXVECTOR4(1, 1, 1, 30.0f);
//
//	D3D11_SUBRESOURCE_DATA InitData;
//	ZeroMemory(&InitData, sizeof(InitData));
//	InitData.pSysMem = &cb_nearly_not_changes_;
//	hr = device_->CreateBuffer(&cb_everyframe_desc, &InitData, constant_buffer_nearly_not_changes_.GetAddressOf());
//	if (FAILED(hr))
//		assert(false);
//
//	return hr;
//}

bool PToolCore::Load()
{
	FILE_EXTENSION_TYPE file_type = LoadFileDialog(L"*", L"ModelView");

	if (file_type == FILE_EXTENSION_TYPE::ERROR_OCCUR)
		return false;

	PModel* model = nullptr;
	
	
	if (file_type == FILE_EXTENSION_TYPE::KGC)
	{
		model = new PKgcObj;
		model->mytype = file_type;
		model->object_name_ = last_time_objname;
	}
	else if (file_type == FILE_EXTENSION_TYPE::SKM)
	{
		model = new PSkmObj;
		PSkmObj* obj = static_cast<PSkmObj*>(model);
		obj->light_obj_ = model_light_;
		int load_index = loadfiles_dir_.size() - 1;
		model->mytype = file_type;
		model->object_name_ = last_time_objname;
		model->Init(device_, immediate_device_context_, L"ModelView.hlsl", "VS", L"ModelView.hlsl", "PS",
			loadfiles_dir_[load_index], L"data/texture/");
	
	}
	else if (file_type == FILE_EXTENSION_TYPE::MAT)
	{
		model = new PMatObj;
		PMatObj* obj = static_cast<PMatObj*>(model);
		obj->light_obj_ = model_light_;
		int load_index = loadfiles_dir_.size() - 1;
		model->mytype = file_type;
		model->object_name_ = last_time_objname;
		model->Init(device_, immediate_device_context_, L"DiffuseLight.hlsl", "VS", L"DiffuseLight.hlsl", "PS",
			loadfiles_dir_[load_index], L"data/texture/");
	}
	object_list_.push_back(model);
	return true;
}

FILE_EXTENSION_TYPE PToolCore::LoadFileDialog(const TCHAR* extension, const TCHAR* title)
{
	OPENFILENAME    ofn = { 0, };
	TCHAR           szFile[MAX_PATH] = { 0, };
	TCHAR			szFileTitle[MAX_PATH] = { 0, };
	static TCHAR* szFilter;
	
	TCHAR lpCurBuffer[256] = { 0, };
	GetCurrentDirectory(256, lpCurBuffer);
	
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	_tcscpy_s(szFile, _T("*."));
	_tcscat_s(szFile, extension);
	_tcscat_s(szFile, _T("\0"));
	
	TCHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	StrCat(path, L"\\Model");

	ofn.lpstrInitialDir = path;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFilter = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0L;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = extension;
	
	if (!GetOpenFileName(&ofn))
	{
		return FILE_EXTENSION_TYPE::ERROR_OCCUR;
	}
	TCHAR* load = _tcstok(szFile, _T("\n"));
	multibyte_string dir = szFile;
	load = &load[_tcslen(load) + 1];
	if (*load == 0)
	{
		loadfiles_dir_.push_back(dir);
	}
	
	while (*load != 0)
	{
		multibyte_string dir = szFile;
		load = _tcstok(load, _T("\n"));
		dir += _T("\\");
		dir += load;
		loadfiles_dir_.push_back(dir);
		load = &load[_tcslen(load) + 1];
	}
	SetCurrentDirectory(lpCurBuffer);
	
	TCHAR szFileName[256];
	TCHAR Drive[MAX_PATH];
	TCHAR Dir[MAX_PATH];
	TCHAR FName[MAX_PATH];
	TCHAR Ext[MAX_PATH];
	_tsplitpath_s(loadfiles_dir_[loadfiles_dir_.size() - 1].c_str(),
		Drive, Dir, FName, Ext);
	Ext[5] = '\0';
	_stprintf_s(szFileName, _T("%s%s"), FName, Ext);
	
	last_time_objname = szFileName;

	multibyte_string FileExt = Ext;
	multibyte_string FileKgcExt = L".kgc";
	multibyte_string FileSkmExt = L".skm";
	multibyte_string FileMatExt = L".anim";
	if (FileExt == FileKgcExt) return FILE_EXTENSION_TYPE::KGC;
	if (FileExt == FileSkmExt) return FILE_EXTENSION_TYPE::SKM;
	if (FileExt == FileMatExt) return FILE_EXTENSION_TYPE::MAT;

	return FILE_EXTENSION_TYPE::ERROR_OCCUR;

}
