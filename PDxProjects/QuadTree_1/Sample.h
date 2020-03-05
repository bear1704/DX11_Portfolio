#pragma once
#include "PCore.h"
#include "PFreeCamera.h"
//#include "PLightObj.h"
#include "PDxRenderTarget.h"
#include "PQuadTree.h"
#include "PNormHeightMap.h"
#include <random>
#include <functional>

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment (lib, "PDxCoreLib64_D.lib")
#else
#pragma comment (lib, "PDxCoreLib64_R.lib")
#endif

#pragma comment (lib, "fmod_vc.lib")
#pragma comment (lib, "winmm.lib")



//PScene* g_current_scene_;

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
	PDxRenderTarget minimap_rt_;
	PPlaneObject rt_screen_;
	PLineObject line_obj_;
	PNormHeightMap map_;


public:
	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;
	void MessageProc(MSG msg);
	
public:
	bool DrawQuadTree(PNode* node);

};
PCORE_RUN(L"QuadTree_1", 0, 0, 1024, 768);


