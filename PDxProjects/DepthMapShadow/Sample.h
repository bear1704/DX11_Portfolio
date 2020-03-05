#pragma once
#include "PCore.h"
#include "PFreeCamera.h"
#include "PLightObj.h"
#include "PDxRenderTarget.h"
#include "PQuadTree.h"
#include "PNormHeightMap.h"
#include "PSelect.h"
#include "PMatObj.h"
#include "PSkmObj.h"
#include "PKgcObj.h"
#include <random>
#include <functional>

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment (lib, "PDxCoreLib64_D.lib")
#else
#pragma comment (lib, "PDxCoreLib64_R.lib")
#endif

#pragma comment (lib, "fmod_vc.lib")
#pragma comment (lib, "winmm.lib")

const float kObjectMoveSpeed = 0.01f;


class PSCharacter
{
public:
	PSCharacter();
	~PSCharacter();
public:
	std::vector<PModel*> object_list_;
	multibyte_string character_name_;
	multibyte_string shader_name_;
	PMatObj* matrix_;
};

class Sample : public PCore
{
public:
	Sample();
	~Sample();
public:
	PCamera*		main_camera_;
	PFreeCamera		free_camera_;
	PLightObj		light_obj_;
	PQuadTreeIndex	quadtree_;
	int				render_depth_;
	P_BOX			random_map_obj_[10];
	PBoxObject		box_obj_;
	PBoxObject		box_obj2_;
	PDxRenderTarget minimap_rt_;
	PPlaneObject	rt_screen_;
	PLineObject		line_obj_;
	PNormHeightMap	map_;
	PSelect			select_;

	D3DXVECTOR3 line_start_;
	D3DXVECTOR3 line_end_;
	
	//Animation
	PParser parse_;
	PSCharacter character_;

	std::wstring object_file_texture_path_;
	std::vector<PModel*> object_list_;
	float elapsed_time_;


	//Shadow
	D3DXMATRIX mat_shadow_view_;
	PDxRenderTarget shadow_rt_;

public:
	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;
	void MessageProc(MSG msg);
	
public:
	bool DrawQuadTree(PNode* node);
	PModel* LoadSheetObject(multibyte_string filename, std::wstring vs_shader_path, std::string vs_func_name,
		std::wstring ps_shader_path, std::string ps_func_name, std::wstring texture_path = L"");

};
PCORE_RUN(L"DepthMap_Shadow", 0, 0, 1024, 768);


