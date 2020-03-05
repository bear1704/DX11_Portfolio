#include "PMobAttackAction.h"
#include "PMonster.h"


PMobAttackAction::PMobAttackAction(PMonster* parent) : PMobState(parent)
{
}


PMobAttackAction::~PMobAttackAction()
{
}

void PMobAttackAction::Process(PPlayerCharacter* target)
{
	
	if (owner_->get_sprite_()->get_animation_type_() != ANIMATIONTYPE::ATTACK) //attack(chase)는 move와 스프라이트를 공유
	{
		owner_->set_sprite_(*owner_->find_sprite_by_type(ANIMATIONTYPE::ATTACK));
	}

	if (owner_->get_ishit_()) //HIT
	{
		owner_->set_ishit_(false);
		owner_->SetTransition(FSM_Event::HIT);
		first_hit = false;
	}

	if (PCollision::GetInstance().RectInRect(target->get_collision_rect_(), owner_->get_collision_rect_()))
	{  //플레이어와의 충돌 체크 
			target->set_hit_(true);
	}

	if (!first_hit) //처음 1회 히트시
	{
		owner_->set_direction_side_(((target->get_collision_rect_().left - owner_->get_collision_rect_().left) > 0) ?
			SIDE::RIGHT : SIDE::LEFT);

		first_hit = true;
	}

	if (focus_time > 0.7f) //히트 뒤에 지속적인 추격
	{
		owner_->set_direction_side_(((target->get_collision_rect_().left - owner_->get_collision_rect_().left) > 0) ?
			SIDE::RIGHT : SIDE::LEFT);

		focus_time = 0.0f;
	}

	if (owner_->get_direction_side_() == SIDE::LEFT)
	{
		owner_->set_is_reversal(false);
		owner_->set_position_(pPoint(owner_->get_position_().x - owner_->get_move_speed_() * g_SecondPerFrame, owner_->get_position_().y));
	}
	else
	{
		owner_->set_is_reversal(true);
		owner_->set_position_(pPoint(owner_->get_position_().x + owner_->get_move_speed_() * g_SecondPerFrame, owner_->get_position_().y));
	}

	progress_time += g_SecondPerFrame;
	focus_time += g_SecondPerFrame;

	if (progress_time > 13.0f)
	{
		owner_->SetTransition(FSM_Event::MOB_TIME_OUT);
		progress_time = 0.0f;
		first_hit = false;
	}


}

