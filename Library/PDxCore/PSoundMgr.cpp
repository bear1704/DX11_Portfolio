#include "PSoundMgr.h"



PSoundMgr::PSoundMgr()
{
}


PSoundMgr::~PSoundMgr()
{
}

bool PSoundMgr::Init()
{
	FMOD::System_Create(&pSystem);
	pSystem->init(32, FMOD_INIT_NORMAL, 0);
	return true;
}

bool PSoundMgr::Frame()
{
	pSystem->update();
	for (auto itor = soundList.begin(); itor != soundList.end(); itor++)
	{
		PSound* pSound = (*itor).second;
		pSound->Frame();
	}
	return true;
}

bool PSoundMgr::Render()
{
	for (auto itor = soundList.begin(); itor != soundList.end(); itor++)
	{
		PSound* pSound = (*itor).second;
		pSound->Render();
	}
	return true;
}

bool PSoundMgr::Release()
{
	for (auto itor = soundList.begin(); itor != soundList.end();)
	{
		PSound* pSound = (*itor).second;
		pSound->Release();
		delete pSound;
		itor = soundList.erase(itor);
	}
	soundList.clear();

	pSystem->close();
	pSystem->release();
	return true;
}

int PSoundMgr::Load(multibyte_string sound_path)
{
	if (sound_path.empty()) return -1;

	TCHAR drive[MAX_PATH] = { 0, };
	TCHAR directory[MAX_PATH] = { 0, };
	TCHAR filename[MAX_PATH] = { 0, };
	TCHAR file_extension[MAX_PATH] = { 0, };
	_tsplitpath_s(sound_path.c_str(),
		drive, directory, filename, file_extension);

	multibyte_string filename_extension = filename;
	
	filename_extension += file_extension;

	multibyte_string path = drive;
	path += directory;
	// 중복제거
	for (auto itor = soundList.begin(); itor != soundList.end(); itor++)
	{
		PSound* sound = (*itor).second;
		if (sound->name == path)
		{
			return sound->index;
		}
	}


	std::string filepath = unicode_to_multibyte_str(sound_path);
	PSound* pSound = new PSound;
	FMOD_RESULT ret = pSystem->createSound(
		filepath.c_str(),
		FMOD_DEFAULT, 0, &pSound->pSound);
	if (ret != FMOD_OK)
	{
		delete pSound;
		return -1;
	}
	pSound->pSystem = pSystem;
	pSound->name = filename;
	pSound->sound_path = path;
	pSound->index = iNumSound++;
	soundList.insert(std::make_pair(pSound->index, pSound));
	return pSound->index;

}

PSound * PSoundMgr::GetPtr(int key)
{
	auto itor = soundList.find(key);
	if (itor != soundList.end())
	{
		PSound* pSound = (*itor).second;
		return pSound;
	}
	return nullptr;
}

bool PSoundMgr::Delete(int key)
{
	auto itor = soundList.find(key);
	if (itor != soundList.end())
	{
		PSound* pSound = (*itor).second;
		pSound->Release();
		delete pSound;
		soundList.erase(itor);
		return true;
	}
	return false;
}

void PSoundMgr::Play(int key)
{
	auto itor = soundList.find(key);
	if (itor != soundList.end())
	{
		PSound* pSound = (*itor).second;
		pSound->Play();
	}
}
