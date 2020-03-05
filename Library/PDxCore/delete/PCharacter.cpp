#include "PCharacter.h"



PCharacter::PCharacter()
{

}

PCharacter::~PCharacter()
{
}

bool PCharacter::Init()
{
	is_reversal_ = false;

	return true;
}

bool PCharacter::Frame()
{
	sprite_.Frame();
	set_collision_box_(collision_box_norm_);
	PlatformWallCollision();

	return true;
}

bool PCharacter::Render()
{
	Spawn();
	return true;
}

bool PCharacter::Release()
{
	sprite_.Release();
	return true;
}

void PCharacter::StatusSet(multibyte_string status_path, multibyte_string object_name)
{
	status.StatusSet(status_path, object_name);
	
}

void PCharacter::StatusSetForServer(multibyte_string status_path, multibyte_string object_name)
{
	status.StatusSetForServer(status_path, object_name);
}

PPlayerStatus & PCharacter::get_status()
{
		return status;
}

void PCharacter::Movement()
{

}

void PCharacter::Set(multibyte_string data_path, multibyte_string object_name, pPoint position)
{
	PObjectInfoManager::GetInstance().LoadDataFromScript(data_path, ObjectLoadType::CHARACTER);
	ObjectInfo info = *(PObjectInfoManager::GetInstance().get_object_info_list_from_map(object_name));

	character_name_ = info.object_name_;
	object_name_ = info.object_name_;
	position_ = position;
	collision_box_norm_ = info.collision_box_;
	alpha_ = info.alpha_;
	scale_ = info.scale_;
	move_speed_ = info.move_speed_;

	//PSpriteManager::GetInstance().LoadDataFromScript(info.sprite_path);
	//sprite_.Set(*PSpriteManager::GetInstance().get_sprite_data_list_from_map(info.sprite_name), alpha_, scale_);
	PSpriteManager::GetInstance().LoadSpriteDataFromScript(info.sprite_path, ObjectLoadType::CHARACTER);
	sprite_.Clone(PSpriteManager::GetInstance().get_sprite_from_map_ex(info.sprite_name), alpha_, scale_);
	sprite_.SetPosition(position_.x, position_.y);
	
	FLOAT_RECT scaled_collisionbox_norm = { collision_box_norm_.left*scale_, collision_box_norm_.top*scale_ ,
	collision_box_norm_.right*scale_, collision_box_norm_.bottom*scale_ };
	spawn_position_ = position;
	collision_box_norm_ = scaled_collisionbox_norm;
	set_collision_box_(collision_box_norm_);
	is_reversal_ = false;

}



void PCharacter::set_collision_box_(FLOAT_RECT norm_box)
{
	float half_width = norm_box.right / 2;
	float half_height = norm_box.bottom / 2;

	collision_box_.left = position_.x - half_width;
	collision_box_.top = position_.y - half_height;
	collision_box_.right = norm_box.right;
	collision_box_.bottom = norm_box.bottom;

	foot_plane_.left = collision_box_.left;
	foot_plane_.top = collision_box_.top + collision_box_.bottom - 5.0f;
	foot_plane_.right = collision_box_.right;
	foot_plane_.bottom = 5.0f;

	if (get_is_reversal_())
	{
		attack_collision_box_.left = collision_box_.left + collision_box_.right;
		attack_collision_box_.top = collision_box_.top;
	}
	else
	{
		attack_collision_box_.left = collision_box_.left - attack_collision_box_.right;
		attack_collision_box_.top = collision_box_.top;
	}
}

FLOAT_RECT PCharacter::get_foot_plane_box()
{
	return foot_plane_;
}

FLOAT_RECT PCharacter::get_attack_collision_box_()
{
	return attack_collision_box_;
}

void PCharacter::PlatformWallCollision()
{
	const std::vector<FLOAT_RECT>& platform_list = PWallAndPlatform::GetInstance().get_platform_list_();
	const std::vector<FLOAT_RECT>& wall_list = PWallAndPlatform::GetInstance().get_wall_list_();
	FLOAT_RECT current_collision_rect;
	float correction_ylength = 0.0f;

	for (auto& it : platform_list)
	{
		if (PCollision::GetInstance().RectInRect(it, foot_plane_, correction_ylength) && physics_.get_is_downphase_())
		{
			physics_.set_is_ground_(true);
			position_.y = it.top - collision_box_norm_.bottom / 2;
		}
	}

	for (auto& it : wall_list)
	{
		if (PCollision::GetInstance().RectInRect(it, collision_box_))
		{
			if (it.left - collision_box_.left < 0 && position_.x  < prev_position_.x) // 왼쪽 벽 충돌
			{
				position_.x = it.left + it.right + collision_box_.right / 2;
				//position_.x = prev_position_.x;
			}
			else if(it.left - collision_box_.left > 0 && position_.x > prev_position_.x) //오른쪽 벽 충돌
			{
				position_.x = it.left - collision_box_.right / 2;
				//position_.x = prev_position_.x;
			}
			
		}
	}
}

void PCharacter::SavePrevPosition()
{
	prev_position_ = position_;
}

float PCharacter::get_move_speed_()
{
	return move_speed_;
}

