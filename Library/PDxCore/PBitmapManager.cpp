#include "PBitmapManager.h"



PBitmapManager::PBitmapManager()
{
}


PBitmapManager::~PBitmapManager()
{
}

bool PBitmapManager::Init()
{
	return false;
}

bool PBitmapManager::Frame()
{
	return false;
}

bool PBitmapManager::Render()
{
	return false;
}

bool PBitmapManager::Release()
{
	for (auto iter = bitmap_list_.begin(); iter != bitmap_list_.end();)
	{
		PBitmap* bitmap = (*iter).second;
		bitmap->Release();
		delete bitmap;
		iter = bitmap_list_.erase(iter);
	}

	bitmap_list_.clear();
	return true;
}

int PBitmapManager::BitmapLoad(multibyte_string filepath, PLoadMode mode)
{
	if (filepath.empty()) return -1;

	TCHAR drive[MAX_PATH];
	TCHAR directory[MAX_PATH];
	TCHAR filename[MAX_PATH];
	TCHAR file_extension[MAX_PATH];
	_tsplitpath_s(filepath.c_str(), drive, directory, filename, file_extension);

	multibyte_string filename_extension;

	if (mode == PLoadMode::BITMAP)
	{
		filename_extension = filename;
		filename_extension += file_extension; //파일이름 + .확장자
	}
	else if (mode == PLoadMode::BITMAPMASK)
	{
		filename_extension += L"Mask_";
		filename_extension += filename;
		filename_extension += file_extension;
	}

	multibyte_string path_except_filename = drive;
	path_except_filename += directory; //파일 경로

	//중복될 경우, 로드할 필요없이 원래 있던 데이터의 인덱스를 가져옴
	for (auto iter = bitmap_list_.begin(); iter != bitmap_list_.end(); iter++)
	{
		PBitmap* bitmap = (*iter).second;
		if (bitmap->image_name_ == filename_extension)
			return (*iter).first;
	}
	//비트맵 로드 실패(파일이 없다던가)
	PBitmap* bitmap = new PBitmap;
	if (bitmap->Load(path_except_filename + filename_extension) == false)
	{
		delete bitmap;
		return -1;
	}
	//로드 성공시 메모리에 적재
	bitmap->image_name_ = filename_extension;
	bitmap->image_path_ = path_except_filename;
	bitmap_list_.insert(std::make_pair(index_, bitmap));

	return index_++;
}


PBitmap * PBitmapManager::get_bitmap_from_map(int key)
{
	auto iter = bitmap_list_.find(key);
	if (iter != bitmap_list_.end())
	{
		PBitmap* bitmap_data = (*iter).second;
		return bitmap_data;
	}
	return nullptr;
}

bool PBitmapManager::Delete(int key)
{
	auto iter = bitmap_list_.find(key);

	if (iter != bitmap_list_.end())
	{
		PBitmap* bitmap_data = (*iter).second;
		bitmap_data->Release();
		delete bitmap_data;
		bitmap_list_.erase(iter);
		return true;
	}
	return false;
}
