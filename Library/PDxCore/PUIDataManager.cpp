#include "PUIDataManager.h"






PUIDataManager::PUIDataManager()
{
	need_load_data_ = true;
}

PUIDataManager::~PUIDataManager()
{
}

bool PUIDataManager::Init()
{
	return false;
}

bool PUIDataManager::Frame()
{
	return false;
}

bool PUIDataManager::Render()
{
	return false;
}

bool PUIDataManager::Release()
{
	return false;
}

PUIComponent * PUIDataManager::get_ui_composition_list_from_map(std::wstring key)
{

	auto iter = ui_composition_list_.find(key);
	if (iter != ui_composition_list_.end())
	{
		PUIComponent* data = (*iter).second;
		return data;
	}
	return nullptr;
	
}

void PUIDataManager::LoadDataFromScript(multibyte_string filepath)
{
	//#LIST 리스트갯수
	//리스트이름 종속요소갯수 리스트포지션x 리스트포지션y
	//객체이름 상대포지션x 상대포지션y 객체타입(버튼,이미지)
	

	if (!need_load_data_)
		return;

	const std::wstring ui_path = L"data/UI/UI_data.txt";

	PParser parser;
	std::vector<std::pair<string, string>> ret_parse;
	std::string path;
	path.assign(filepath.begin(), filepath.end());
	parser.XmlParse(path, &ret_parse);
	
	//
	for (auto iter = ret_parse.begin(); iter != ret_parse.end(); iter++)
	{

		if (iter->second.compare("composition") == 0)
		{
			std::string composition_name;
			pPoint composition_pos;
			PUIComponent* uicomponent = nullptr;
			PUIComponent* component_list = new PUIComponent();

			while (true)
			{
				iter++;

				if (iter->first.compare("END") == 0)
					break;
				else if (iter->first == "name")
				{
					composition_name = iter->second;
					iter++;
					std::vector<string> vec = parser.SplitString(iter->second, ',');
					composition_pos = { std::stof(vec[0]), std::stof(vec[1]) };
				}
				else
				{
					std::string comp_type = iter->first;
					std::string comp_name = iter->second; //각각의 오브젝트(btn,image) 이름
					iter++; //coord로 이동
					std::vector<string> vec = parser.SplitString(iter->second, ',');
					pPoint pos = { std::stof(vec[0]), std::stof(vec[1]) };
					
					if (comp_type == "button")
						uicomponent = (PButtonControl*) new PButtonControl();
					else if (comp_type == "image")
						uicomponent = (PImageControl*) new PImageControl();

					uicomponent->Set(ui_path,multibyte_to_unicode_str(comp_name), pPoint(pos.x + composition_pos.x , pos.y + composition_pos.y));
					component_list->Add(uicomponent);


				}

			}

			component_list->set_position_(composition_pos);
			ui_composition_list_.insert(std::make_pair(multibyte_to_unicode_str(composition_name), component_list));

		}
	}

	if (need_load_data_)
		need_load_data_ = false;


}
