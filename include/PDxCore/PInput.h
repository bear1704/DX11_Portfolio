#pragma once
#include "PStd.h"




class PInput : public PSingleton<PInput>
{

	friend class PSingleton<PInput>;
private:
	PInput();
	KEYSTAT keyboardKeyState[256];
	KEYSTAT mouseKeyState[3];
	KEYSTAT mouseBeforeState[3];
	POINT mousePos;
public:
	bool Init();
	bool Frame();
	bool Release();
	bool Render();
	LRESULT MsgProc(MSG msg);

public:
	virtual ~PInput();
	KEYSTAT KeyCheck(DWORD key);
	DWORD Key(DWORD key_);
};

