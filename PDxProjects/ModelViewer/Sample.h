#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "PCore.h"
#include "PTexture.h"
#include "PShape.h"
#include "PDxState.h"
#include "PFreeCamera.h"
#include "PDxRenderTarget.h"
#include "PLightObj.h"
#include "PKgcObj.h"
#include "PSkmObj.h"
#include "PMatObj.h"


#if defined(DEBUG) || defined(_DEBUG)
#pragma comment (lib, "PDxCoreLib64_D.lib")
#else
#pragma comment (lib, "PDxCoreLib64_R.lib")
#endif

#pragma comment (lib, "fmod_vc.lib")
#pragma comment (lib, "winmm.lib")



//
//struct CB_VS_ChangesEveryFrame
//{
//	D3DXMATRIX mat_normal;
//	D3DXVECTOR3 light_pos;
//	float padding1;
//	D3DXVECTOR3 camera_pos;
//	float padding2;
//	D3DXVECTOR3 vec_look;
//	float padding3;
//};
//struct CB_VS_NearlyNotChange
//{
//	D3DXVECTOR4 cb_AmbientLightColor;
//	D3DXVECTOR4 cb_DiffuseLightColor;
//	D3DXVECTOR4 cb_SpecularLightColor;
//};


class Sample : public PCore
{
public:
	Sample();
	~Sample();
private:
	PCamera* main_camera_;
	PFreeCamera free_camera_;
	std::vector<PModel*> object_list_;
	std::vector<multibyte_string> loadfiles_dir_;
	PBoxObject box;
	PLightObj light_obj_;
	PLightObj* model_light;

public:
	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;


public:
	void MessageProc(MSG msg) override;
	bool Load();
	FILE_EXTENSION_TYPE LoadFileDialog(const TCHAR* extension, const TCHAR* title);

};
PCORE_RUN(L"Model Viewer", 0, 0, 800, 600);
