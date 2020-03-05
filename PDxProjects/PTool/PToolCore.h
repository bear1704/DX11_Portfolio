#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "PCore.h"
#include "PTexture.h"
#include "PShape.h"
#include "PDxState.h"
#include "PCamera.h"
#include "PMap.h"
#include "PFreeCamera.h"
#include "PDxRenderTarget.h"
#include "PSkyBox.h"
#include "PLightObj.h"
#include "PNormHeightMap.h"
#include "PEffectObject.h"
#include "../ModelViewer/PKgcObj.h"
#include "../ModelViewer/PSkmObj.h"
#include "../ModelViewer/PMatObj.h"


#if defined(DEBUG) || defined(_DEBUG)
#pragma comment (lib, "PDxCoreLib64_D.lib")
#else
#pragma comment (lib, "PDxCoreLib64_R.lib")
#endif

#pragma comment (lib, "fmod_vc.lib")
#pragma comment (lib, "winmm.lib")




class PToolCore : public PCore
{
public:
	PToolCore();
	~PToolCore();

public:
	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;


public:
	D3D11_BLEND_DESC blend_desc_;
	ID3D11BlendState* blend_state_;

	PCamera* main_camera_;
	PCamera  backview_camera_;
	PFreeCamera free_camera_;

	PDxRenderTarget dx_rt_;

	PPlaneObject screen_tex_object_;

	PSkyBox skybox_;

	PLightObj light_obj_;
	PLightObj* model_light_;
	PEffectObject effect_plane_;
	
	std::vector<PModel*> object_list_;
	std::vector<multibyte_string> loadfiles_dir_;
	multibyte_string last_time_objname;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_changes_everyframe_;
	//CB_VS_NearlyNotChange cb_nearly_not_changes_;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_nearly_not_changes_;

	D3DXMATRIX mat_billboard;



public:
	void MessageProc(MSG msg) override;
	//HRESULT CreateConstantBuffer();
	bool Load();
	FILE_EXTENSION_TYPE LoadFileDialog(const TCHAR* extension, const TCHAR* title);
};