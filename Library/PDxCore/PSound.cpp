#include "PSound.h"



PSound::PSound()
{
	soundVolume = 0.5f;
	pChannel = nullptr;
	pSound = nullptr;
}


PSound::~PSound()
{
}

void PSound::Play()
{
	bool isPlaying = false;

	if (pChannel)
	{
		pChannel->isPlaying(&isPlaying);
	}
	if (!isPlaying)
	{
		pSystem->playSound(pSound, 0, false, &pChannel);
		pChannel->setVolume(soundVolume);
	}

}

void PSound::Stop()
{
	if (pChannel)
	{
		pChannel->stop();
	}
}

void PSound::Paused()
{
	if (pChannel == nullptr) return;
	bool isPaused;

	pChannel->getPaused(&isPaused);
	pChannel->setPaused(isPaused);
}

void PSound::Volume(float volume, bool isUp)
{
	if (pChannel == nullptr) return;
	float currentVolume;
	pChannel->getVolume(&currentVolume);
	if (isUp == true)
	{
		currentVolume += soundVolume * g_SecondPerFrame * 10.0f;
		currentVolume = min(1.0f, currentVolume);
	}
	else
	{
		currentVolume -= soundVolume * g_SecondPerFrame* 10.0f;
		currentVolume = max(0.0f, currentVolume);
	}

	pChannel->setVolume(currentVolume);
}

void PSound::PlayEffect()
{
	FMOD::Channel* pChannel = nullptr;
	pChannel->setVolume(soundVolume);
	pChannel->setMode(FMOD_LOOP_OFF);

	pSystem->playSound(pSound, 0, false, &pChannel);
}

void PSound::SetMode(DWORD mode)
{
	if (pChannel)
	{
		pChannel->setMode(mode);
	}

}

bool PSound::Init()
{
	return true;
}

bool PSound::Frame()
{
	return true;
}

bool PSound::Render()
{
	return true;
}

bool PSound::Release()
{
	if (pSound == nullptr) return false;
		pSound->release();
	return true;

}
