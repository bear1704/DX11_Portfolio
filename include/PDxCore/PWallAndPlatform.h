#pragma once
#include "PStd.h"
#include "PParser.h"

class PWallAndPlatform : public PSingleton<PWallAndPlatform>
{
private:
	PWallAndPlatform();
public:
	friend class PSingleton<PWallAndPlatform>;
	~PWallAndPlatform();

public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
private:
	std::vector<FLOAT_RECT> platform_list_;
	std::vector<FLOAT_RECT> monster_wall_list_;
	std::vector<FLOAT_RECT> wall_list_;
	std::wstring data_path_;
	bool need_load_data_;
public:
	void LoadPlatformData(multibyte_string filepath);
	void set_data_path_(std::wstring str);
	void Set();
	std::vector<FLOAT_RECT>& get_platform_list_();
	std::vector<FLOAT_RECT>& get_monster_wall_list_();
	std::vector<FLOAT_RECT>& get_wall_list_();
};

