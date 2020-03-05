#include "PObjectDataManager.h"





PObjectDataManager::~PObjectDataManager()
{
}

bool PObjectDataManager::Init()
{
	need_load_data_ = true;
	return true;
}

bool PObjectDataManager::Frame()
{
	return false;
}

bool PObjectDataManager::Render()
{
	return false;
}

bool PObjectDataManager::Release()
{
	return false;
}

std::vector<PObject*> PObjectDataManager::get_object_list_from_map(std::wstring key)
{
	auto iter = object_composition_list_.find(key);
	if (iter != object_composition_list_.end())
	{
		std::vector<PObject*>& data = (*iter).second;
		return data;
	}

	
}

std::vector<PSprite*> PObjectDataManager::get_animation_list_from_map(std::wstring key)
{
	std::vector<PSprite*> null_vec;

	auto iter = object_animation_list_.find(key);
	if (iter != object_animation_list_.end())
	{
		std::vector<PSprite*>& data = (*iter).second;
		return data;
	}
	assert(iter != object_animation_list_.end()); //테스트 전에 잘못된 코드가 들어가는지 확인용. 지울예정
	return null_vec;
	
}



void PObjectDataManager::LoadDataFromScript(multibyte_string filepath)
{
	//#CHARLIST 리스트갯수
	//리스트이름 요소갯수
	//객체이름 절대포지션x 절대포지션y 객체타입(몬스터, NPC, 캐릭터, MAP)

	if (!need_load_data_)
		return;

	const std::wstring path = L"data/character/character_data.txt";
	const std::wstring map_path = L"data/map/map_data.txt";
	const std::wstring status_path = L"data/character/character_status_data.txt";
	const std::wstring animation_path = L"data/character/sprite/animation_list.txt";



	pPoint map_pos = pPoint(P2DCamera::GetInstance().get_world_size_().x / 2, P2DCamera::GetInstance().get_world_size_().y / 2);


	std::vector<std::pair<std::string, std::string>> ret_parse;  //데이터 컴포지션
	std::string str;
	str.assign(filepath.begin(), filepath.end());
	PParser parser;
	parser.XmlParse(str, &ret_parse);
	
	std::vector<PObject*> object_list;

	for (auto iter = ret_parse.begin(); iter != ret_parse.end(); iter++)
	{


		if (iter->second.compare("CHARLIST") == 0)
		{
			std::string composition_name;
			std::string object_name;
			std::string object_type; 

			while (true)
			{
				PObject* component;
				iter++;
				if (iter->first.compare("END") == 0)
					break;
				else if (iter->first.compare("name") == 0)
					composition_name.assign(iter->second.begin(), iter->second.end());
				else if (iter->first.compare("MONSTER") == 0)
				{
					object_name = iter->second;
					iter++; //COORD 확인용

					std::vector<string> pos_vec = parser.SplitString(iter->second, ',');
					pPoint pos = { std::stof(pos_vec[0].c_str()), std::stof(pos_vec[1].c_str()) };

					component = (PMonster*) new PMonster();
					component->Set(path, string_to_multibyte(object_name), pPoint( pos.x, pos.y));
					LoadAnimationDataFromScriptEx(animation_path); //이후 캐릭터/몬스터 스프라이트 경로를 분할한다면, 애니메이션 경로도 분할해야 함 
					component->set_gravity_(450.0f);
					component->set_type_(Type::MONSTER);
					component->StatusSet(status_path, component->get_object_name()); //아마 아직 미구현
					component->set_animation_list_(PObjectDataManager::get_animation_list_from_map(string_to_multibyte(object_name)));
					component->set_alpha_and_scale_(component->get_alpha_(), component->get_scale_());
					object_list.push_back(component);
				}
				else if (iter->first.compare("MAP") == 0)
				{
					object_name = iter->second;
					iter++; //COORD 확인용

					component =  new PObject();
					component->Set(map_path, string_to_multibyte(object_name), pPoint(map_pos.x, map_pos.y));
					component->set_type_(Type::MAP);
					component->set_alpha_and_scale_(component->get_alpha_(), component->get_scale_());
					object_list.insert(object_list.begin(), component); //map은 가장 먼저 렌더되도록 제일 앞에 위치.
				}
				else if (iter->first.compare("PLAYER") == 0)
				{
					object_name = iter->second;
					iter++; //COORD 확인용

					std::vector<string> pos_vec = parser.SplitString(iter->second, ',');
					pPoint pos = { std::stof(pos_vec[0].c_str()), std::stof(pos_vec[1].c_str()) };

					component = (PPlayerCharacter*) new PPlayerCharacter();
					component->Set(path, string_to_multibyte(object_name), pPoint(pos.x, pos.y));
					LoadAnimationDataFromScriptEx(animation_path); //캐릭터 스프라이트 선행 로드 후에 위치해야 함.
					component->set_gravity_(450.0f);
					component->set_type_(Type::PLAYER);
					component->StatusSet(status_path ,component->get_object_name());
					component->set_animation_list_(PObjectDataManager::get_animation_list_from_map(string_to_multibyte(object_name)));
					component->set_alpha_and_scale_(component->get_alpha_(), component->get_scale_());
					object_list.push_back(component);
				}
				else if (iter->first.compare("OTHER_PLAYER") == 0)
				{
					object_name = iter->second;
					iter++; //COORD 확인용

					std::vector<string> pos_vec = parser.SplitString(iter->second, ',');
					pPoint pos = { std::stof(pos_vec[0].c_str()), std::stof(pos_vec[1].c_str()) };

					component = (PPlayerCharacter*) new PPlayerCharacter();
					component->Set(path, string_to_multibyte(object_name), pPoint(pos.x, pos.y));
					LoadAnimationDataFromScriptEx(animation_path); //캐릭터 스프라이트 선행 로드 후에 위치해야 함.
					component->set_gravity_(450.0f);
					component->set_type_(Type::OTHER_PLAYER);
					component->StatusSet(status_path, component->get_object_name());
					component->set_animation_list_(PObjectDataManager::get_animation_list_from_map(string_to_multibyte(object_name)));
					component->set_alpha_and_scale_(component->get_alpha_(), component->get_scale_());
					object_list.push_back(component);
				}
				else if (iter->first.compare("BOSSMONSTER") == 0)
				{
					object_name = iter->second;
					iter++; //COORD 확인용

					std::vector<string> pos_vec = parser.SplitString(iter->second, ',');
					pPoint pos = { std::stof(pos_vec[0].c_str()), std::stof(pos_vec[1].c_str()) };

					component = (PBossMonster*) new PBossMonster();
					component->Set(path, string_to_multibyte(object_name), pPoint(pos.x, pos.y));
					LoadAnimationDataFromScriptEx(animation_path); //캐릭터 스프라이트 선행 로드 후에 위치해야 함.
					//component->set_gravity_(450.0f);
					component->set_type_(Type::BOSS_MONSTER);
					component->StatusSet(status_path, component->get_object_name());
					component->set_animation_list_(PObjectDataManager::get_animation_list_from_map(string_to_multibyte(object_name)));
					component->set_alpha_and_scale_(component->get_alpha_(), component->get_scale_());
					object_list.push_back(component);
				}
				else if (iter->first.compare("BOSSMONSTER_SERV") == 0)
				{

					const std::wstring path = L"data/character_data.txt";
					const std::wstring status_path = L"data/character_status_data.txt";

					object_name = iter->second;
					iter++; //COORD 확인용

					std::vector<string> pos_vec = parser.SplitString(iter->second, ',');
					pPoint pos = { std::stof(pos_vec[0].c_str()), std::stof(pos_vec[1].c_str()) };

					PBossMonster* component = new PBossMonster();
					component->SetForServer(path, string_to_multibyte(object_name), pPoint(pos.x, pos.y));
					component->set_type_(Type::BOSS_MONSTER);
					component->StatusSetForServer(status_path, component->get_object_name());
					component->set_alpha_and_scale_(component->get_alpha_(), component->get_scale_());
					
					component->set_id(20000); //hard_coded


					PObject* rect_component = component;



					object_list.insert(object_list.begin(), rect_component); //맵 다음으로 보여지도록
				}
			}

			object_composition_list_.insert(std::make_pair(multibyte_to_unicode_str(composition_name), object_list));
		}


	}

	if(need_load_data_)
		need_load_data_ = false;

}


void PObjectDataManager::LoadAnimationDataFromScriptEx(multibyte_string filepath)
{

	std::vector<PSprite*> vec;

	PParser parser;
	std::vector<std::pair<string, string>> ret_parse;
	std::string path;
	path.assign(filepath.begin(), filepath.end());
	parser.XmlParse(path, &ret_parse);

	for (auto iter = ret_parse.begin(); iter != ret_parse.end(); iter++)
	{

		if (iter->second.compare("list") == 0)
		{
			std::vector<PSprite*> vec;
			std::wstring player_name;
			std::wstring animation_type;
			std::wstring animation_name;

			while (true)
			{
				iter++;
				if (iter->first.compare("name") == 0)
					player_name.assign(iter->second.begin(), iter->second.end());
				else if (iter->first.compare("END") == 0)
					break;
				else
				{
					animation_type.assign(iter->first.begin(), iter->first.end());
					std::transform(animation_type.begin(), animation_type.end(), animation_type.begin(), ::toupper); //대문자로
					animation_name.assign(iter->second.begin(), iter->second.end());
					PSprite* sprite = new PSprite();
					sprite->Clone(PSpriteManager::GetInstance().get_sprite_from_map_ex(animation_name), 1.0f, 1.0f);
					sprite->set_animation_type_(WstringToAnimationtype(animation_type));
					vec.push_back(sprite);
				}

			}		

			object_animation_list_.insert(std::make_pair(player_name, vec));
		}

	}

}
