#pragma once
#include "PSound.h"

const int g_iMaxSound = 10;

class PSoundMgr : public PSingleton<PSoundMgr>
{
	friend class PSingleton<PSoundMgr>;
	
	std::map<int, PSound*> soundList;

public:
	~PSoundMgr();
public:
	FMOD::System*	pSystem;
	float			fVolume;
	int				iNumSound;

public:
	bool	Init() override; 
	bool	Frame() override;
	bool	Render() override;
	bool	Release() override;
	int		Load(multibyte_string sound_path);
	PSound* GetPtr(int key);
	bool	Delete(int key);
	void	Play(int key);

private:
	PSoundMgr();







};

