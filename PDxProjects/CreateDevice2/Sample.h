#pragma once
#include "PCore.h"
#include "PTexture.h"

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment (lib, "PDxCoreLib64_D.lib")
#else
#pragma comment (lib, "PDxCoreLib64_R.lib")
#endif

#pragma comment (lib, "fmod_vc.lib")
#pragma comment (lib, "winmm.lib")


PScene* g_current_scene_;


class Sample : public PCore
{
public:
	 Sample();
	~Sample();

public:
	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;



};

PCORE_RUN(L"vertexbuffer", 0, 0, 1024, 768);
