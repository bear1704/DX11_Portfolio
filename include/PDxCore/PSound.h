#pragma once

#include "PStd.h"
#include <map>
#include <string>
#include "fmod.hpp"

class PSound
{
public:
	PSound();
	virtual ~PSound();
public:
	FMOD::System*	pSystem;
	multibyte_string	name;
	multibyte_string	sound_path;
	int				index;
public:
	FMOD::Sound*	pSound;
	int				soundNum;
	float			soundVolume;
	FMOD::Channel*	pChannel;
	std::string		csBuffer;

public:
	void	Play();
	void	Stop();
	void	Paused();
	void	Volume(float volume = 1.0f, bool isUp = true);
	void	PlayEffect();
	void	SetMode(DWORD mode);

public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();





};

