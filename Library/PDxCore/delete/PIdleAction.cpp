#include "PIdleAction.h"
#include "PPlayerCharacter.h"


PIdleAction::PIdleAction(PPlayerCharacter * parent) : PPlayerState(parent)
{

}

PIdleAction::~PIdleAction()
{
}
void PIdleAction::Process()
{
}
//
//void PIdleAction::Process()
//{
//	bool is_owner = (owner_->get_client_owner_character());
//	PNetworkDataStorage::GetInstance().set_b_need_report(true);
//
//	if (owner_->get_sprite_()->get_animation_type_() != ANIMATIONTYPE::IDLE)
//	{
//		owner_->set_sprite_(*owner_->find_sprite_by_type(ANIMATIONTYPE::IDLE));
//		PNetworkDataStorage::GetInstance().set_b_need_report(true);
//	}
//
//
//	if (owner_->get_status().get_hp_() < 0)
//	{
//		owner_->SetTransition(FSM_Event::HPEMPTY);
//	}
//
//
//	if (owner_->get_hit_() && owner_->get_invisible_() == false)
//	{
//		if(is_owner)
//			owner_->SetTransition(FSM_Event::HIT);
//	}
//
//	if (g_InputActionMap.jumpKey == KEYSTAT::KEY_PUSH)
//	{
//		if (is_owner)
//			owner_->SetTransition(FSM_Event::INPUT_JUMP);
//	}
//
//
//	if (g_InputActionMap.leftArrowKey == KEYSTAT::KEY_HOLD || g_InputActionMap.rightArrowKey == KEYSTAT::KEY_HOLD)
//	{
//		if (is_owner)
//			owner_->SetTransition(FSM_Event::INPUT_MOVE); 
//	}
//
//
//	if (g_InputActionMap.attackKey == KEYSTAT::KEY_PUSH) //공격 
//	{
//		if (is_owner)
//			owner_->SetTransition(FSM_Event::INPUT_ATTACK); //sprite setposition필요
//	}
//
//	owner_->get_sprite_()->SetPosition(owner_->get_position_().x, owner_->get_position_().y);
//}
