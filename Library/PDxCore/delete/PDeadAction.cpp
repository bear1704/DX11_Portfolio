#include "PDeadAction.h"
#include "PPlayerCharacter.h"


PDeadAction::PDeadAction(PPlayerCharacter* parent) : PPlayerState(parent)
{
}


PDeadAction::~PDeadAction()
{
}

void PDeadAction::Process()
{
}

//void PDeadAction::Process()
//{
//
//	if (owner_->get_sprite_()->get_animation_type_() != ANIMATIONTYPE::DEAD) //HIT는 IDLE과 모션을 공유, 단 깜빡임
//	{
//		owner_->set_sprite_(*owner_->find_sprite_by_type(ANIMATIONTYPE::DEAD));
//		PNetworkDataStorage::GetInstance().set_b_need_report(true);
//	}
//
//	owner_->set_is_character_dead(true);
//	owner_->get_sprite_()->SetPosition(owner_->get_position_().x, owner_->get_position_().y);
//
//}
