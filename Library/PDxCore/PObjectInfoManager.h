#pragma once
#include "PStd.h"
#include "PWallAndPlatform.h"
#include "PSprite.h"
#include <assert.h>

struct ObjectInfo
{
	multibyte_string object_name_;
	float move_speed_;
	FLOAT_RECT collision_box_;
	float alpha_;
	float scale_;
	multibyte_string sprite_path;
	multibyte_string sprite_name;
};

struct ObjectStatus
{
	int level_; ObjectType type_;  std::string name_;
	float max_hp_;	float max_mp_;	float current_hp_;	float current_mp_;
	int current_exp_;
	int str_;	int dex_;	int reward_exp_;	int monster_damage_;

};

class PObjectInfoManager : public PSingleton<PObjectInfoManager>
{//name(key) movespeed collisionboxsize alpha scale 
private:
	PObjectInfoManager();
	~PObjectInfoManager();

private:
	friend class PSingleton<PObjectInfoManager>;
	std::map<std::wstring, ObjectInfo*> object_info_list;
	std::map<std::wstring, ObjectStatus*> object_status_list;
	bool need_load_character_data_;
	bool need_load_UI_data_;
	bool need_load_map_data_;
	bool need_load_status_data_;
public:
	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;

	ObjectInfo* get_object_info_list_from_map(std::wstring key);
	ObjectStatus* get_object_status_from_map(std::wstring key);
	void LoadDataFromScript(multibyte_string filepath, ObjectLoadType type);
	void LoadStatusDataFromScript(multibyte_string filepath);
};

