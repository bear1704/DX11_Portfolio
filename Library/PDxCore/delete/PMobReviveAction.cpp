#include "PMobReviveAction.h"
#include "PMonster.h"


PMobReviveAction::PMobReviveAction(PMonster* parent) : PMobState(parent)
{
}


PMobReviveAction::~PMobReviveAction()
{
}

void PMobReviveAction::Process(PPlayerCharacter * target)
{
	if (owner_->get_sprite_()->get_animation_type_() != ANIMATIONTYPE::IDLE) //Revive는 IDLE과 공유
	{
		owner_->set_sprite_(*owner_->find_sprite_by_type(ANIMATIONTYPE::IDLE));
		owner_->set_position_(owner_->get_spawn_position_());
		owner_->get_sprite_()->set_alpha_(0.0f);
		owner_->get_status().ModifyHP(owner_->get_status().get_max_hp());
	}

	if (PCollision::GetInstance().RectInRect(target->get_collision_rect_(), owner_->get_collision_rect_()) && progress_time > 3.0f)
	{  //플레이어와의 충돌 체크 
		target->set_hit_(true);
	}


	float current_alpha = owner_->get_sprite_()->get_alpha_();
	current_alpha += g_SecondPerFrame * 1.0f;
	owner_->get_sprite_()->set_alpha_(current_alpha);

	if (owner_->get_sprite_()->get_alpha_() > 1.0f)
	{
		owner_->get_sprite_()->set_alpha_(1.0f);
	}

	progress_time += g_SecondPerFrame;

	if (owner_->get_ishit_())
	{
		owner_->set_ishit_(false);
		owner_->SetTransition(FSM_Event::HIT);
	}

	if (progress_time > 7.0f)
	{
		progress_time = 0.0f;
		owner_->SetTransition(FSM_Event::MOB_TIME_OUT);
	}

}
