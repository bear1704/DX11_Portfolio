#pragma once
#include "PStd.h"
#include <string>

class PTimer
{

public:
	float secondPerFrame;
	float accumulationTime;
	int FPS;
	DWORD beforeTick;
	float frameTime;
	DWORD frameCount;
	TCHAR csBuffer [MAX_PATH];
public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
public:
	PTimer();
	virtual ~PTimer();
};

