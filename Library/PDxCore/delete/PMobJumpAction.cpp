#include "PMobJumpAction.h"
#include "PMonster.h"


PMobJumpAction::PMobJumpAction(PMonster* parent) : PMobState(parent)
{
}


PMobJumpAction::~PMobJumpAction()
{
}

void PMobJumpAction::Process(PPlayerCharacter* target)
{

	if (owner_->get_sprite_()->get_animation_type_() != ANIMATIONTYPE::JUMP)
	{
		owner_->set_sprite_(*owner_->find_sprite_by_type(ANIMATIONTYPE::JUMP));
		owner_->get_physics_().StartJump();
	}

	if (PCollision::GetInstance().RectInRect(target->get_collision_rect_(), owner_->get_collision_rect_()))
	{  //플레이어와의 충돌 체크 
		target->set_hit_(true);
	}

	
	if (owner_->get_ishit_())
	{
		owner_->set_ishit_(false);
		owner_->SetTransition(FSM_Event::HIT);
	}

	if (owner_->get_physics_().get_isjump() == false)
	{
		owner_->SetTransition(FSM_Event::JUMP_END);
	}

}
