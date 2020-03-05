#include "Sample.h"


Sample::Sample()
{
	
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
		D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR3(200.0f, 100.0f, 0.0f) , 1 ,device_, immediate_device_context_, main_camera_);
	model_light = new PLightObj();
	model_light->Init(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1),
		D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR3(200.0f, 100.0f, 0.0f), 2 , device_, immediate_device_context_, main_camera_);





	//맨 마지막
	Load();
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
	if (g_InputActionMap.rightClick == KEYSTAT::KEY_PUSH)
	{
		Load();
	}



#pragma endregion

	for (int obj = 0; obj < object_list_.size(); obj++)
	{
		object_list_[obj]->Frame();
	}
	main_camera_->Frame();
	light_obj_.Frame();

	return true;
}

bool Sample::Render()
{
	DX::ApplySamplerState(immediate_device_context_, DX::PDxState::sampler_state_anisotropic);
	light_obj_.Render();


	for (int obj = 0; obj < object_list_.size(); obj++)
	{
		object_list_[obj]->SetWVPMatrix(nullptr, (D3DXMATRIX*)&main_camera_->matView_, (D3DXMATRIX*)&main_camera_->matProj_);
		object_list_[obj]->Render();
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

bool Sample::Load()
{
	FILE_EXTENSION_TYPE file_type = LoadFileDialog(L"*", L"ModelView");
	PModel* model = nullptr;

	if (file_type == FILE_EXTENSION_TYPE::KGC)
		model = new PKgcObj;
	else if (file_type == FILE_EXTENSION_TYPE::SKM)
	{
		model = new PSkmObj;
		PSkmObj* obj = static_cast<PSkmObj*>(model);
		obj->light_obj_ = model_light;
		int load_index = loadfiles_dir_.size() - 1;
		model->Init(device_, immediate_device_context_, L"ModelView.hlsl", "VS", L"ModelView.hlsl", "PS",
			loadfiles_dir_[load_index], L"data/texture/");
		
	}
	else if (file_type == FILE_EXTENSION_TYPE::MAT)
	{
		model = new PMatObj;
		PMatObj* obj = static_cast<PMatObj*>(model);
		obj->light_obj_ = model_light;
		int load_index = loadfiles_dir_.size() - 1;
		model->Init(device_, immediate_device_context_, L"DiffuseLight.hlsl", "VS", L"DiffuseLight.hlsl", "PS",
			loadfiles_dir_[load_index], L"data/texture/");
	}
	object_list_.push_back(model);
	return true;
}

FILE_EXTENSION_TYPE Sample::LoadFileDialog(const TCHAR* extension, const TCHAR* title)
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
	ofn.lpstrInitialDir = _T("../../data/Obj/");
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

	multibyte_string FileExt = Ext;
	multibyte_string FileKgcExt = L".kgc";
	multibyte_string FileSkmExt = L".skm";
	multibyte_string FileMatExt = L".anim";
	if (FileExt == FileKgcExt) return FILE_EXTENSION_TYPE::KGC;
	if (FileExt == FileSkmExt) return FILE_EXTENSION_TYPE::SKM;
	if (FileExt == FileMatExt) return FILE_EXTENSION_TYPE::MAT;
	
	return FILE_EXTENSION_TYPE::ERROR_OCCUR;
	
}
