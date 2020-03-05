#include "PObjectInfoManager.h"



PObjectInfoManager::PObjectInfoManager()
{

}


PObjectInfoManager::~PObjectInfoManager()
{
}

bool PObjectInfoManager::Init()
{
	need_load_character_data_ = true;
	need_load_UI_data_ = true;
	need_load_map_data_ = true;
	need_load_status_data_ = true;
	return true;
}

bool PObjectInfoManager::Frame()
{
	return false;
}

bool PObjectInfoManager::Render()
{
	return false;
}

bool PObjectInfoManager::Release()
{
	return false;
}

ObjectInfo* PObjectInfoManager::get_object_info_list_from_map(std::wstring key)
{
	auto iter = object_info_list.find(key);
	if (iter != object_info_list.end())
	{
		ObjectInfo* status = (*iter).second;
		return status;
	}

	return nullptr;
}

ObjectStatus * PObjectInfoManager::get_object_status_from_map(std::wstring key)
{

	auto iter = object_status_list.find(key);
	if (iter != object_status_list.end())
	{
		ObjectStatus* status = (*iter).second;
		return status;
	}
	return nullptr;
}

//void PObjectInfoManager::LoadDataFromScript(multibyte_string filepath)
//{
//	//이름, 이동속도, 박스 LTRB, 알파, 스케일, 스프라이트 경로, 스프라이트 이름
//	FILE* fp = nullptr;
//
//	_wfopen_s(&fp, filepath.c_str(), _T("rt"));
//	assert(fp != nullptr);
//
//	TCHAR buffer[256] = { 0, };
//	TCHAR temp_buffer[256] = { 0, };
//	TCHAR sprite_path_buffer[256] = { 0, };
//	TCHAR sprite_name_buffer[64] = { 0, };
//	TCHAR map_data_buffer[256] = { 0, }; //맵 로드일 경우, 맵의 콜리전 정보(있으면)
//
//	int number_of_data = -1;
//
//	_fgetts(buffer, _countof(buffer), fp); //한줄 받아오기(캐릭터 데이터 갯수)
//	_stscanf_s(buffer, _T("%s%d"), temp_buffer, _countof(temp_buffer), &number_of_data);
//
//	for (int index_data = 0; index_data < number_of_data; index_data++)
//	{
//	
//		ObjectInfo* status = new ObjectInfo();
//		FLOAT_RECT collision_box;
//		float move_speed;
//		float alpha; float scale;
//
//		_fgetts(buffer, _countof(buffer), fp);
//		_stscanf_s(buffer, _T("%s%f%f%f%f%f%f%f%s%s%s"), temp_buffer, _countof(temp_buffer),
//			&move_speed ,&collision_box.left, &collision_box.top, &collision_box.right, &collision_box.bottom, &alpha, &scale, 
//			sprite_path_buffer, _countof(sprite_path_buffer), sprite_name_buffer, _countof(sprite_name_buffer),
//			map_data_buffer, _countof(map_data_buffer));
//		
//
//		std::wstring character_name(temp_buffer);
//		std::wstring path(sprite_path_buffer);
//		std::wstring sprite_name(sprite_name_buffer);
//		std::wstring map_data(map_data_buffer);
//
//		if (map_data.compare(L"") != 0) //Map data가 있다는 의미
//		{
//			PWallAndPlatform::GetInstance().set_data_path_(map_data);
//			PWallAndPlatform::GetInstance().Set();
//		}
//
//
//		status->collision_box_ = collision_box;
//		status->move_speed_ = move_speed;
//		status->object_name_ = character_name;
//		status->scale_ = scale;
//		status->alpha_ = alpha;
//		status->sprite_path = path;
//		status->sprite_name = sprite_name;
//
//		object_info_list.insert(std::make_pair(character_name, status));
//	}
//	fclose(fp);
//}

void PObjectInfoManager::LoadDataFromScript(multibyte_string filepath, ObjectLoadType type)
{
	//이름, 이동속도, 박스 LTRB, 알파, 스케일, 스프라이트 경로, 스프라이트 이름
	
	if (!need_load_character_data_ && type == ObjectLoadType::CHARACTER) //로드할 필요 없음 
		return;
	if (!need_load_UI_data_ && type == ObjectLoadType::UI) //로드할 필요 없음 
		return;
	if (!need_load_map_data_ && type == ObjectLoadType::MAP) //로드할 필요 없음 
		return;

	PParser parse;
	std::vector<std::pair<string, string>> ret_parse;
	parse.XmlParse(std::string(filepath.begin(), filepath.end()), &ret_parse);
	
	
	for (auto iter = ret_parse.begin(); iter != ret_parse.end(); iter++)
	{

		if (iter->second == "character_data" || 
			iter->second == "ui_data" ||
			iter->second == "map_data")
		{

			std::string object_name; //UI, character, map..
			std::string sprite_path;
			std::string init_sprite_name;
			std::string map_collision_data_path; //map데이터 읽는 중일 경우
			
			ObjectInfo* info = new ObjectInfo();
			FLOAT_RECT collision_box = { 0,0,0,0 };
			float character_move_speed = 0;
			float alpha = 1.0f; float scale = 1.0f;

			
			while (true)
			{
				iter++; 

				if (iter->first == "END")
					break;

				if (iter->first == "name")
					object_name = iter->second;
				else if (iter->first == "movespeed")
					character_move_speed = std::stof(iter->second);
				else if (iter->first == "collision_box" || iter->first == "image_size") //image_size : map, uidata의 경우 적용
				{
					std::vector<string> vec = parse.SplitString(iter->second, ',');
					collision_box.left = std::stof(vec[0]); collision_box.top = std::stof(vec[1]);
					collision_box.right = std::stof(vec[2]); collision_box.bottom = std::stof(vec[3]);
				}
				else if (iter->first == "alpha")
					alpha = std::stof(iter->second);
				else if (iter->first == "scale")
					scale = std::stof(iter->second);
				else if (iter->first == "init_sprite")
					init_sprite_name = iter->second;
				else if (iter->first == "sprite_path")
					sprite_path = iter->second;
				else if (iter->first == "collision_data_path")
					map_collision_data_path = iter->second;

			}

		if (map_collision_data_path.compare("") != 0) //Map data가 있다는 의미
		{
			
			PWallAndPlatform::GetInstance().set_data_path_(string_to_multibyte(map_collision_data_path));
			PWallAndPlatform::GetInstance().Set();
		}
		
		info->collision_box_ = collision_box;
		info->move_speed_ = character_move_speed;
		info->object_name_ = string_to_multibyte(object_name);
		info->scale_ = scale;
		info->alpha_ = alpha;
		info->sprite_path = string_to_multibyte(sprite_path);
		info->sprite_name = string_to_multibyte(init_sprite_name);
		
		object_info_list.insert(std::make_pair(string_to_multibyte(object_name), info));
		}
	}

	if (need_load_character_data_ && type == ObjectLoadType::CHARACTER)
		need_load_character_data_ = false;
	if (need_load_UI_data_ && type == ObjectLoadType::UI)
		need_load_UI_data_ = false;
	if (need_load_map_data_ && type == ObjectLoadType::MAP)
		need_load_map_data_ = false;
}

void PObjectInfoManager::LoadStatusDataFromScript(multibyte_string filepath)
{
	//이름, 레벨, MHP, MMP, CurHP, CurMP, CurEXP, STR, DEX, 잡힐경우 경험치(몬스터대상), 데미지(몬스터대상)

	if (!need_load_status_data_)
		return;



	PParser parse;
	std::vector<std::pair<string, string>> ret_parse;
	parse.XmlParse(std::string(filepath.begin(), filepath.end()), &ret_parse);


	for (auto iter = ret_parse.begin(); iter != ret_parse.end(); iter++)
	{
		
		if (iter->second == "status")
		{
			ObjectStatus* status = new ObjectStatus();
			while (1)
			{
				iter++;

				if (iter->first == "END")
					break;

				if (iter->first == "name")
					status->name_ = iter->second;
				else if (iter->first == "type")
					status->type_ = StringToObjectType(iter->second);
				else if (iter->first == "level")
					status->level_ = std::stoi(iter->second);
				else if (iter->first == "max_hp")
					status->max_hp_ = std::stof(iter->second);
				else if (iter->first == "max_mp")
					status->max_mp_ = std::stof(iter->second);
				else if (iter->first == "current_hp")
					status->current_hp_ = std::stof(iter->second);
				else if (iter->first == "current_mp")
					status->current_mp_ = std::stof(iter->second);
				else if (iter->first == "str")
					status->str_ = std::stoi(iter->second);
				else if (iter->first == "dex")
					status->dex_ = std::stoi(iter->second);
				else if (iter->first == "reward_exp")
					status->reward_exp_ = std::stoi(iter->second);
				else if (iter->first == "body_dmg")
					status->monster_damage_ = std::stoi(iter->second);

			}
			object_status_list.insert(std::make_pair(multibyte_to_unicode_str(status->name_), status));
		}
		
	}

	need_load_status_data_ = false;

}

