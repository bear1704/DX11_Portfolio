#include "PUIComponent.h"



PUIComponent::PUIComponent()
{
}


PUIComponent::~PUIComponent()
{
}
bool PUIComponent::Init()
{
	for (auto &iter : component_list_)
	{
		iter->Init();
	}
	return true;
}
bool PUIComponent::Frame()
{
	for (auto &iter : component_list_)
	{
		iter->Frame();
	}
	return true;
}

bool PUIComponent::Render()
{
	for (auto &iter : component_list_)
	{
		iter->Render();
	}
	return true;
}

bool PUIComponent::Release()
{
	for (auto &iter : component_list_)
	{
		iter->Release();
	}
	return true;
}

void PUIComponent::Add(PUIComponent* component)
{
	
	component_list_.push_back(component);

}

void PUIComponent::Set(multibyte_string data_path, multibyte_string object_name, pPoint position)
{

		PObjectInfoManager::GetInstance().LoadDataFromScript(data_path, ObjectLoadType::UI); //UI도 뼈대는 RectObject이므로 이걸로 세팅하는게 맞긴 한데.. 좀 께름칙하네
		ObjectInfo info = *(PObjectInfoManager::GetInstance().get_object_info_list_from_map(object_name));

		object_name_ = info.object_name_;
		position_ = position;
		collision_box_norm_ = info.collision_box_;
		alpha_ = info.alpha_;
		scale_ = info.scale_;

		PSpriteManager::GetInstance().LoadSpriteDataFromScript(info.sprite_path, ObjectLoadType::UI);
		//sprite_.Set(*PSpriteManager::GetInstance().get_sprite_data_list_from_map(info.sprite_name), alpha_, scale_);
		sprite_.Clone(PSpriteManager::GetInstance().get_sprite_from_map_ex(info.sprite_name), alpha_, scale_);
		sprite_.SetPosition(position_.x, position_.y);


		FLOAT_RECT scaled_collisionbox_norm = { collision_box_norm_.left*scale_, collision_box_norm_.top*scale_ ,
		collision_box_norm_.right*scale_, collision_box_norm_.bottom*scale_ };

		collision_box_norm_ = scaled_collisionbox_norm;


}

void PUIComponent::ReviseAllComponentPosition(pPoint& worldpos)
{
	worldpos.y -= g_SecondPerFrame * 10.0f;
	position_ = P2DCamera::GetInstance().WorldToGamescreen(worldpos);

	int accumulation_gap = 0.0f;

	accumtime += g_SecondPerFrame;

	if (accumtime >= 8.0f)
	{
		accumtime = 0.0f;
		
		for (auto& iter : component_list_)
		{
			iter->get_sprite_()->set_alpha_(0.0f);
		}
	}

	for (auto& iter : component_list_)
	{
		pPoint posi = pPoint(position_.x + accumulation_gap, position_.y);
		iter->position_ = posi;
		iter->get_sprite_()->SetPosition(posi.x, posi.y);
		accumulation_gap += gap;
	}
}

std::vector<PUIComponent*>& PUIComponent::get_component_list_()
{
	return component_list_;
}


void PUIComponent::ResetDmgPresent()
{
	for (auto& iter : component_list_)
	{
		iter->get_sprite_()->set_alpha_(1.0f);
		iter->get_sprite_()->set_scale_(1.3f);
	}
}

void PUIComponent::DamageToSprite(int damage)
{
	std::string dmgstr = std::to_string(damage);

	for (int i = 0; i < dmgstr.length(); i++)
	{
		int number = dmgstr[i] - '0';
		component_list_[i]->get_sprite_()->set_current_played_spriteframe_(number);

	}


}

