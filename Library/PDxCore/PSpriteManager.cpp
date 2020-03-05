#include "PSpriteManager.h"


PSpriteManager::~PSpriteManager()
{
}

PSpriteManager::PSpriteManager() : kDamageFontLifetime(1.2f), kDamageFontGap(35.0f)
{
	dmg_font_index_ = 0;
}

bool PSpriteManager::Init()
{
	need_load_character_sprite_data_ = true;
	need_load_UI_sprite_data_ = true;
	need_load_map_sprite_data_ = true;
	return true;
}

bool PSpriteManager::Frame()
{
	for (PSprite& sprite : render_wait_list_)
	{
		if (sprite.get_is_dmg())
		{
			pPoint pos = sprite.get_position_();
			sprite.SetPosition(pos.x, pos.y - 0.5f);
		}

		sprite.Frame();
	}

	return false;
}

bool PSpriteManager::Render()
{
	//for (PSprite& sprite : render_wait_list_)
	//{
	//	sprite.Render();
	//}

	return false;
}

bool PSpriteManager::Release()
{
	for (auto iter = render_wait_list_.begin(); iter != render_wait_list_.end(); )
	{
		PSprite& sprite = *iter;
		if (sprite.get_is_dead_())
		{
			sprite.Release();
			iter = render_wait_list_.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	
	return false;
}


PSprite* PSpriteManager::get_sprite_from_map_ex(std::wstring key)
{
	auto iter = sprite_list_.find(key);
	if (iter != sprite_list_.end())
	{
		PSprite* data = (*iter).second;
		return data;
	}
	return nullptr;
}


void PSpriteManager::LoadSpriteDataFromScript(multibyte_string filepath, ObjectLoadType type)
{

	if (!need_load_character_sprite_data_ && type == ObjectLoadType::CHARACTER)
		return;
	if (!need_load_UI_sprite_data_ && type == ObjectLoadType::UI)
		return;
	if (!need_load_map_sprite_data_ && type == ObjectLoadType::MAP)
		return;

	PParser parser;
	std::vector<std::pair<string, string>> ret_parse;
	std::string str;
	str.assign(filepath.begin(), filepath.end());
	parser.XmlParse(str, &ret_parse);

	for (auto iter = ret_parse.begin() ; iter != ret_parse.end() ; iter++)
	{
		
		if (iter->second.compare("sprite") == 0)
		{
			SpriteDataInfo info;
			std::wstring sprite_name;
			std::vector<DX::PTex_uv4> rt;

			PTexture* texture = nullptr;

			while (true)
			{
				iter++;
				if (iter->first.compare("name") == 0)
					sprite_name.assign(iter->second.begin(), iter->second.end());
				else if (iter->first.compare("max_frame") == 0)
					info.max_frame = std::atoi(iter->second.c_str());
				else if (iter->first.compare("lifetime") == 0)
					info.lifetime = std::atof(iter->second.c_str());
				else if (iter->first.compare("once_playtime") == 0)
					info.once_playtime = std::atof(iter->second.c_str());
				else if (iter->first.compare("tex_name") == 0)
				{
					info.texture_name.assign(iter->second.begin(), iter->second.end());
					texture= PTextureManager::GetInstance().GetTextureFromMap(info.texture_name);
				}
				else if (iter->first.compare("uv_ltop") == 0)
				{
					DX::PTex_uv4 uv4;

					std::vector<string> uv_vec = parser.SplitString(iter->second, ',');
					
					DX::PTex_uv left_top = PTextureManager::GetInstance().ImageCoordinateToTexCoordinate(std::atof(uv_vec[0].c_str()), std::atof(uv_vec[1].c_str()),
						texture->GetImageWidth(), texture->GetImageHeight());

					iter++;

					uv_vec = parser.SplitString(iter->second, ',');
					DX::PTex_uv right_bottom = PTextureManager::GetInstance().ImageCoordinateToTexCoordinate(std::atof(uv_vec[0].c_str()), std::atof(uv_vec[1].c_str()),
						texture->GetImageWidth(), texture->GetImageHeight());

					uv4.u[0] = left_top.u;
					uv4.v[0] = left_top.v;
					uv4.u[1] = right_bottom.u;
					uv4.v[1] = left_top.v;
					uv4.u[2] = right_bottom.u;
					uv4.v[2] = right_bottom.v;
					uv4.u[3] = left_top.u;
					uv4.v[3] = right_bottom.v;

					rt.push_back(uv4);
				}
				else if (iter->first.compare("END") == 0)
					break;
			}

			info.tex_boundary_list = std::move(rt); //오류 예감

			PSprite* sprite = new PSprite();
			sprite->Set(info, 1.0, 1.0);
			sprite_list_.insert(std::make_pair(sprite_name, sprite));

		}
		
		
	}
	if (need_load_character_sprite_data_ && type == ObjectLoadType::CHARACTER)
		need_load_character_sprite_data_ = false;
	if (need_load_UI_sprite_data_ && type == ObjectLoadType::UI)
		need_load_UI_sprite_data_ = false;
	if (need_load_map_sprite_data_ && type == ObjectLoadType::MAP)
		need_load_map_sprite_data_ = false;
}

void PSpriteManager::LoadSpriteDataWithoutScript(std::wstring sprite_name, std::vector<PTexture*> texture, SpriteDataInfo info)
{
	PSprite* sprite = new PSprite;
	
	if (texture.size() > 0)
		sprite->set_texture_list(texture);

	sprite->Set(info, 1.0f, 1.0f);
		
	sprite_list_.insert(make_pair(sprite_name, sprite));
}

bool PSpriteManager::Delete(int key)
{
	return false;
}

void PSpriteManager::AddRenderWaitList(PSprite sprite)
{
	render_wait_list_.push_back(sprite);
}

std::vector<PSprite*> PSpriteManager::GetSpriteListFromMap()
{
	std::vector<PSprite*> ret_vec;

	std::map<std::wstring, PSprite*>::iterator iter;
	for (iter = sprite_list_.begin(); iter != sprite_list_.end(); iter++)
	{
		PSprite* sprite = &(*iter->second);
		ret_vec.push_back(sprite);
	}
	return ret_vec;
}

