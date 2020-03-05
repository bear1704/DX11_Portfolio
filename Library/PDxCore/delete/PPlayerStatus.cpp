#include "PPlayerStatus.h"
#include <random>
#include <functional>


PPlayerStatus::PPlayerStatus(PObject* owner)
{
	owner_ = owner;
}
PPlayerStatus::PPlayerStatus()
{
}


PPlayerStatus::~PPlayerStatus()
{
}

void PPlayerStatus::ModifyHP(int hp)
{
	int hp_revised = min(hp, max_hp_);
	hp_revised = max(hp_revised, -0.1f);

	if (abs(past_hp_ - hp_) < 0.1f)
	{
		past_hp_ = hp_;
		hp_ = hp_revised;
	}
	else
	{
		hp_ = hp_revised;
	}
}

void PPlayerStatus::ModifyMP(int mp)
{
	int mp_revised = min(mp, max_mp_);
	mp_revised = max(mp_revised, 0);

	if (abs(past_mp_ - mp_) < 0.1f)
	{
		past_mp_ = mp_;
		mp_ = mp_revised;
	}
	else
	{
		mp_ = mp_revised;
	}
}
void PPlayerStatus::ModifyEXP(int exp)
{
	int exp_revised = min(exp, max_exp_);
	exp_revised = max(exp_revised, 0);

	if (abs(past_exp_ - exp_) < 0.01f)
	{
		past_exp_ = exp_;
		exp_ = exp_revised;
	}
	else
	{
		exp_ = exp_revised;
	}
}
void PPlayerStatus::DecreaseHP(int minus)
{
	hp_ -= minus;
}
void PPlayerStatus::DecreaseMP(int minus)
{
	mp_ -= minus;
}
void PPlayerStatus::DecreaseEXP(int minus)
{
	exp_ -= minus;
}
void PPlayerStatus::IncreaseHP(int plus)
{
	if (hp_ + plus > max_hp_)
		hp_ = max_hp_;
	else
		hp_ += plus;
}
void PPlayerStatus::IncreaseMP(int plus)
{
	mp_ += plus;
}
void PPlayerStatus::IncreaseEXP(int plus)
{
	exp_ += plus;
}
bool PPlayerStatus::Init()
{
	/*if (owner_->get_type_() == Type::OTHER_PLAYER)
	{
		return false;
	}*/

	if (owner_->get_type_() == Type::BOSS_MONSTER)
		hp_increase_speed_ = 5000.0f;


	if (owner_->get_type_() == Type::PLAYER || owner_->get_type_() == Type::OTHER_PLAYER)
	{
		hp_increase_speed_ = 500.0f;
		mp_increase_speed_ = 20.0f;
		exp_increase_speed_ = 500.0f;

		FLOAT_RECT& exp_rect = exp_image->get_max_rect_size();
		FLOAT_RECT current_guage_rect = { 0,0,0,0 };
		current_guage_rect.right = exp_rect.right * exp_ / max_exp_;
		current_guage_rect.bottom = exp_rect.bottom;
		exp_image->SetRectListSize(current_guage_rect);
	}

	return true;
}

bool PPlayerStatus::Frame()
{
	player_ui_component->Frame();
	return true;
}

bool PPlayerStatus::Render()
{

	/*if (owner_->get_type_() == Type::OTHER_PLAYER)
		return false;*/

	float past_hp_rate = past_hp_ / max_hp_;
	float current_hp_rate = hp_ / max_hp_;

	float past_mp_rate = past_mp_ / std::max<float>(max_mp_, 1.0f);
	float current_mp_rate = mp_ / std::max<float>(max_mp_, 1.0f);

	float past_exp_rate = past_exp_ / max_exp_;
	float current_exp_rate = exp_ / max_exp_;

 	if (past_hp_ > hp_ && abs(past_hp_ - hp_) > 0.1f) //HP
	{
		FLOAT_RECT& hp_rect = hp_image->get_max_rect_size();
		FLOAT_RECT current_guage_rect = { 0,0,0,0 };
		current_guage_rect.right = hp_rect.right * past_hp_rate;
		current_guage_rect.bottom = hp_rect.bottom;
		hp_image->SetRectListSize(current_guage_rect);
		past_hp_ -= hp_increase_speed_ * g_SecondPerFrame;
	}
	else if(past_hp_ < hp_ )
	{
		FLOAT_RECT& hp_rect = hp_image->get_max_rect_size();
		FLOAT_RECT current_guage_rect = { 0,0,0,0 };
		current_guage_rect.right = hp_rect.right * past_hp_rate;
		current_guage_rect.bottom = hp_rect.bottom;
		hp_image->SetRectListSize(current_guage_rect);
		past_hp_ += hp_increase_speed_ * g_SecondPerFrame;
	}
	if (owner_->get_type_() == Type::PLAYER || owner_->get_type_() == Type::OTHER_PLAYER) //보스몬스터 이미지없어서 터지는거 방지 
	{
		if (past_mp_ > mp_ && abs(past_mp_ - mp_) > 0.1f) //MP
		{
			FLOAT_RECT& mp_rect = mp_image->get_max_rect_size();
			FLOAT_RECT current_guage_rect = { 0,0,0,0 };
			current_guage_rect.right = mp_rect.right * past_mp_rate;
			current_guage_rect.bottom = mp_rect.bottom;
			mp_image->SetRectListSize(current_guage_rect);
			past_mp_ -= mp_increase_speed_ * g_SecondPerFrame;
		}
		else if (past_mp_ < mp_)
		{
			FLOAT_RECT& mp_rect = mp_image->get_max_rect_size();
			FLOAT_RECT current_guage_rect = { 0,0,0,0 };
			current_guage_rect.right = mp_rect.right * past_mp_rate;
			current_guage_rect.bottom = mp_rect.bottom;
			mp_image->SetRectListSize(current_guage_rect);
			past_mp_ += mp_increase_speed_ * g_SecondPerFrame;
		}
		if (past_exp_ > exp_ && abs(past_exp_ - exp_) > 0.01f) //EXP
		{
			FLOAT_RECT& exp_rect = exp_image->get_max_rect_size();
			FLOAT_RECT current_guage_rect = { 0,0,0,0 };
			current_guage_rect.right = exp_rect.right * past_exp_rate;
			current_guage_rect.bottom = exp_rect.bottom;
			exp_image->SetRectListSize(current_guage_rect);
			past_exp_ -= exp_increase_speed_ * g_SecondPerFrame;
		}
		else if (past_exp_ < exp_) //EXP
		{
			FLOAT_RECT& exp_rect = exp_image->get_max_rect_size();
			FLOAT_RECT current_guage_rect = { 0,0,0,0 };
			current_guage_rect.right = exp_rect.right * past_exp_rate;
			current_guage_rect.bottom = exp_rect.bottom;
			exp_image->SetRectListSize(current_guage_rect);
			past_exp_ += exp_increase_speed_ * g_SecondPerFrame;
		}
	}
	player_ui_component->Render();

	return true;
}

bool PPlayerStatus::Release()
{
	player_ui_component->Release();
	return true;
}

void PPlayerStatus::StatusSet(multibyte_string status_path, multibyte_string object_name)
{
	PObjectInfoManager::GetInstance().LoadStatusDataFromScript(status_path);
	ObjectStatus* status = PObjectInfoManager::GetInstance().get_object_status_from_map(object_name);

	level_ = status->level_;
	hp_ = status->current_hp_;
	mp_ = status->current_mp_;
	exp_ = status->current_exp_;
	max_hp_ = status->max_hp_;
	max_mp_ = status->max_mp_;
	max_exp_ = status->level_ * 1003;
	str_ = status->str_;
	dex_ = status->dex_;
	atk_ = str_ * 2 + dex_;
	past_hp_ = hp_;
	past_mp_ = mp_;
	past_exp_ = exp_;
	reward_exp_mob = status->reward_exp_;
	damage_mob = status->monster_damage_;


	if (owner_->get_type_() == Type::PLAYER || owner_->get_type_() == Type::OTHER_PLAYER)
	{
		player_ui_component = PUIDataManager::GetInstance().get_ui_composition_list_from_map(L"ENERGY_GUAGE");
		std::vector<PUIComponent*>& comp_vec = player_ui_component->get_component_list_();


		for (int i = 0; i < comp_vec.size(); i++)
		{
			if (comp_vec[i]->get_object_name().compare(L"hpbar") == 0)
			{

				/*수정필요 1 : UI사이즈를 API에서는 스프라이트의 RECT와 비트맵 이미지 내의 좌표가 같았기 때문에 
				바로 size를 리턴받아 사용해도 됐었지만, 이젠 size를 얻으려면 texutre내의 uv좌표를 다시 이미지 좌표로 변경해줘야함.
				*/
				hp_image = (PImageControl*)comp_vec[i];
				//hp_image->set_max_rect_size(comp_vec[i]->get_sprite_()->get_original_size_list()[represent_sprite_number]);
			}

			else if (comp_vec[i]->get_object_name().compare(L"mpbar") == 0)
			{
				mp_image = (PImageControl*)comp_vec[i];
				//mp_image->set_max_rect_size(comp_vec[i]->get_sprite_()->get_original_size_list()[represent_sprite_number]);
			}
			else if (comp_vec[i]->get_object_name().compare(L"expbar") == 0)
			{
				exp_image = (PImageControl*)comp_vec[i];
				//exp_image->set_max_rect_size(comp_vec[i]->get_sprite_()->get_original_size_list()[represent_sprite_number]);
			}
		}
	}

	if (owner_->get_type_() == Type::BOSS_MONSTER)
	{
		player_ui_component = PUIDataManager::GetInstance().get_ui_composition_list_from_map(L"BOSS_HPBAR");
		std::vector<PUIComponent*>& comp_boss_vec = player_ui_component->get_component_list_();

		for (int i = 0; i < comp_boss_vec.size(); i++) //플레이어 적용안되면 보스로 전이
		{
			if (comp_boss_vec[i]->get_object_name().compare(L"boss_hpbar") == 0)
			{
				hp_image = (PImageControl*)comp_boss_vec[i];
				//hp_image->set_max_rect_size(comp_boss_vec[i]->get_sprite_()->get_original_size_list()[represent_sprite_number]);
			}
		}
	}

}

void PPlayerStatus::StatusSetForServer(multibyte_string status_path, multibyte_string object_name)
{
	PObjectInfoManager::GetInstance().LoadStatusDataFromScript(status_path);
	ObjectStatus* status = PObjectInfoManager::GetInstance().get_object_status_from_map(object_name);

	level_ = status->level_;
	hp_ = status->current_hp_;
	mp_ = status->current_mp_;
	exp_ = status->current_exp_;
	max_hp_ = status->max_hp_;
	max_mp_ = status->max_mp_;
	max_exp_ = status->level_ * 1003;
	str_ = status->str_;
	dex_ = status->dex_;
	atk_ = str_ * 2 + dex_;
	past_hp_ = hp_;
	past_mp_ = mp_;
	past_exp_ = exp_;
	reward_exp_mob = status->reward_exp_;
	damage_mob = status->monster_damage_;
}

int PPlayerStatus::get_level_()
{
	return level_;
}

int PPlayerStatus::get_hp_()
{
	return hp_;
}

int PPlayerStatus::get_mp_()
{
	return mp_;
}

int PPlayerStatus::get_exp_()
{
	return exp_;
}

int PPlayerStatus::get_max_hp()
{
	return max_hp_;
}

int PPlayerStatus::get_max_exp()
{
	return max_exp_;
}

int PPlayerStatus::get_max_mp()
{
	return max_mp_;
}

int PPlayerStatus::get_damage()
{
	int damage = (str_ * 2 + dex_ + 1);

	std::random_device r;
	std::mt19937 engine(r());
	std::uniform_int_distribution<int> distribution(-300, 300);
	auto generator = std::bind(distribution, engine); //데미지 난수화 
	int random = generator();
	damage += random;
	
	return damage;
}

int PPlayerStatus::get_reward_exp()
{
	return reward_exp_mob;
}

int PPlayerStatus::mob_damage()
{
	return damage_mob;
}

bool PPlayerStatus::is_dead()
{
	if (hp_ < 0)
		return true;

	return false;
}

void PPlayerStatus::set_owner(PObject* owner)
{
	owner_ = owner;
}

