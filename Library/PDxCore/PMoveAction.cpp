#include "PMoveAction.h"
#include "PPlayerCharacter.h"



PMoveAction::PMoveAction(PPlayerCharacter * parent) : PPlayerState(parent)
{
}

PMoveAction::~PMoveAction()
{
}
void PMoveAction::Process()
{
}
//
//void PMoveAction::Process()
//{
//	bool is_owner = (owner_->get_client_owner_character());
//
//	PNetworkDataStorage::GetInstance().set_b_need_report(true);
//
//	if (owner_->get_sprite_()->get_animation_type_() != ANIMATIONTYPE::MOVE)
//	{
//		owner_->set_sprite_(*owner_->find_sprite_by_type(ANIMATIONTYPE::MOVE));
//	}
//
//
//	if (owner_->get_status().get_hp_() < 0)
//	{
//		owner_->SetTransition(FSM_Event::HPEMPTY);
//	}
//
//	if (g_InputActionMap.jumpKey == KEYSTAT::KEY_PUSH )
//	{
//		if (is_owner)
//			owner_->SetTransition(FSM_Event::INPUT_JUMP);
//
//	}
//	if (owner_->get_hit_() && owner_->get_invisible_() == false)
//	{
//		if (is_owner)
//			owner_->SetTransition(FSM_Event::HIT);
//	}
//
//	if (g_InputActionMap.leftArrowKey == KEYSTAT::KEY_HOLD || g_InputActionMap.leftArrowKey == KEYSTAT::KEY_PUSH)
//	{
//		if (is_owner)
//		{
//			owner_->set_is_reversal(false);
//			owner_->set_position_(pPoint(owner_->get_position_().x - owner_->get_move_speed_() * g_SecondPerFrame, owner_->get_position_().y));
//		}
//	}
//	else if (g_InputActionMap.rightArrowKey == KEYSTAT::KEY_HOLD || g_InputActionMap.leftArrowKey == KEYSTAT::KEY_PUSH)
//	{
//		if (is_owner)
//		{
//			
//			owner_->set_is_reversal(true);
//			owner_->set_position_(pPoint(owner_->get_position_().x + owner_->get_move_speed_() * g_SecondPerFrame, owner_->get_position_().y));
//		}
//	}
//	
//	
//	
//	if (owner_->is_right_dir() == false && is_owner == false) //other_player(online other client)
//	{
//			owner_->set_is_reversal(false);
//			owner_->set_position_(pPoint(owner_->get_position_().x - owner_->get_move_speed_() * g_SecondPerFrame, owner_->get_position_().y));
//	}
//	else if (owner_->is_right_dir() == true && is_owner == false) //other_player(online other client)
//	{
//			owner_->set_is_reversal(true);
//			owner_->set_position_(pPoint(owner_->get_position_().x + owner_->get_move_speed_() * g_SecondPerFrame, owner_->get_position_().y));
//	}
//
//
//
//
//
//	owner_->get_sprite_()->SetPosition(owner_->get_position_().x, owner_->get_position_().y);
//
//	if (g_InputActionMap.leftArrowKey == KEYSTAT::KEY_FREE && g_InputActionMap.rightArrowKey == KEYSTAT::KEY_FREE) //무브->IDLE
//	{
//		if (is_owner)
//			owner_->SetTransition(FSM_Event::INPUT_NONE);
//
//	}
//
//	if (g_InputActionMap.attackKey == KEYSTAT::KEY_PUSH) //공격으로 전환
//	{
//		if (is_owner)
//			owner_->SetTransition(FSM_Event::INPUT_ATTACK); //sprite setposition필요
//
//
//	}
//
//}
