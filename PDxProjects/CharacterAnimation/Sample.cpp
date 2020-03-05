#include "Sample.h"


Sample::Sample()
{
	elapsed_time_ = 0.0f;
}

Sample::~Sample()
{

}


bool Sample::Init()
{

	free_camera_.Init();
	free_camera_.camera_position_ = D3DXVECTOR3(0.0f, 10.0f, -50.0f);
	free_camera_.vec_view_target_ = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	free_camera_.CreateTargetViewMatrix(free_camera_.camera_position_, 
		free_camera_.vec_view_target_, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	free_camera_.CreateProjectionMatrix();
	main_camera_ = &free_camera_;
	light_obj_.Init(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1),
		D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), device_, immediate_device_context_, main_camera_);
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
			character_.matrix_ =static_cast<PMatObj*>(LoadSheetObject(matrix_file, L"DiffuseLight.hlsl", "VS", L"DiffuseLight.hlsl", "PS"));
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
	//object_list_[obj]->Frame();

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




	main_camera_->Frame();

	return true;
}

bool Sample::Render()
{
	DX::ApplySamplerState(immediate_device_context_, DX::PDxState::sampler_state_anisotropic);
	light_obj_.Render();


	for (auto obj : object_list_)
	{
		obj->PreRender();

		if (obj->mytype == FILE_EXTENSION_TYPE::SKM)
		{
			PSkmObj* skm = (PSkmObj*)obj;
			skm->PreRender();
			skm->SetWVPMatrix(nullptr, (D3DXMATRIX*)&main_camera_->matView_, (D3DXMATRIX*)&main_camera_->matProj_);
			skm->PostRender();
		}
		else if (obj->mytype == FILE_EXTENSION_TYPE::OBJECT)
		{
			PImportObject* imp = (PImportObject*)obj;
			imp->PreRender();
			imp->SetWVPMatrix(nullptr, (D3DXMATRIX*)& main_camera_->matView_, (D3DXMATRIX*)& main_camera_->matProj_);
			imp->PostRender();
		}



	}



	return true;

}

bool Sample::Release()
{

	return true;
}

void Sample::MessageProc(MSG msg)
{
	PCore::MessageProc(msg);
	//PInput::GetInstance().MsgProc(msg);
	if (main_camera_ == nullptr) return;
	main_camera_->MessageProc(msg);
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
		model->Init(device_, immediate_device_context_, vs_shader_path, vs_func_name,ps_shader_path, ps_func_name,
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
