#include "PWallAndPlatform.h"
#include <assert.h>


PWallAndPlatform::PWallAndPlatform()
{
	need_load_data_ = true;
}


PWallAndPlatform::~PWallAndPlatform()
{
}

bool PWallAndPlatform::Init()
{
	platform_list_.reserve(10);
	monster_wall_list_.reserve(6);
	wall_list_.reserve(2);

	return true;
}

bool PWallAndPlatform::Frame()
{
	return true;
}

bool PWallAndPlatform::Render()
{
	return true;
}

bool PWallAndPlatform::Release()
{
	return true;
}

void PWallAndPlatform::Set()
{
	LoadPlatformData(data_path_);
}

std::vector<FLOAT_RECT>& PWallAndPlatform::get_platform_list_()
{
	return platform_list_;
}

std::vector<FLOAT_RECT>& PWallAndPlatform::get_monster_wall_list_()
{
	return monster_wall_list_;
}

std::vector<FLOAT_RECT>& PWallAndPlatform::get_wall_list_()
{
	return wall_list_;
}



void PWallAndPlatform::LoadPlatformData(multibyte_string filepath)
{
	if (!need_load_data_)
		return;

	PParser parser;
	std::vector<std::pair<string, string>> ret_parse;
	std::string path;
	path.assign(filepath.begin(), filepath.end());
	parser.XmlParse(path, &ret_parse);

	for (auto iter = ret_parse.begin(); iter != ret_parse.end(); iter++)
	{

		if (iter->second.compare("wall_and_platform") == 0)
		{
			std::vector<string> coord;
			while (true)
			{
				FLOAT_RECT rect;

				iter++;

				if (iter->first.compare("END") == 0)
					break;
				else if (iter->first.compare("START") == 0) {}
				else
				{
					coord = parser.SplitString(iter->second, ',');
					rect.left = std::stof(coord[0]);  rect.top = std::stof(coord[1]);
					rect.right = std::stof(coord[2]); rect.bottom = std::stof(coord[3]);

					if (iter->first == "platform")
						platform_list_.push_back(rect);
					else if (iter->first == "wall")
						wall_list_.push_back(rect);
					else if (iter->first == "monsterwall")
						monster_wall_list_.push_back(rect);
				}

			}

		}
	}

	if (need_load_data_)
		need_load_data_ = false;
}


void PWallAndPlatform::set_data_path_(std::wstring str)
{
	data_path_ = str;
}
