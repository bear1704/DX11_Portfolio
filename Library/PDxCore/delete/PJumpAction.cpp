#include "PJumpAction.h"
#include "PPlayerCharacter.h"


PJumpAction::PJumpAction(PPlayerCharacter* parent) : PPlayerState(parent)
{
}


PJumpAction::~PJumpAction()
{
}

void PJumpAction::Process()
{
	bool is_owner = (owner_->get_client_owner_character());

	if (owner_->get_sprite_()->get_animation_type_() != ANIMATIONTYPE::JUMP)
	{
		owner_->set_sprite_(*owner_->find_sprite_by_type(ANIMATIONTYPE::JUMP));
		PSoundMgr::GetInstance().Play(PSoundMgr::GetInstance().Load(L"data/sound/jump.mp3"));
	}
	if (owner_->get_hit_() && owner_->get_invisible_() == false)
	{
		owner_->SetTransition(FSM_Event::HIT);
	}

	if (is_owner)
	{

		if (g_InputActionMap.leftArrowKey == KEYSTAT::KEY_HOLD)
		{
			owner_->set_is_reversal(false);
			owner_->set_position_(pPoint(owner_->get_position_().x - owner_->get_move_speed_() * g_SecondPerFrame, owner_->get_position_().y));
		}
		else if (g_InputActionMap.rightArrowKey == KEYSTAT::KEY_HOLD)
		{
			owner_->set_is_reversal(true);
			owner_->set_position_(pPoint(owner_->get_position_().x + owner_->get_move_speed_() * g_SecondPerFrame, owner_->get_position_().y));
		} //점프에 무브먼트를 넣어두는건 매우 멍청한 생각. 후에 점프시에 방향키 방향에 따라 결정되는 관성값으로 이동하게 해야 함.
	}
	else //computer other-player
	{

		if (!(owner_->is_right_dir()))
		{
			owner_->set_is_reversal(false);
			owner_->set_position_(pPoint(owner_->get_position_().x - owner_->get_move_speed_() * g_SecondPerFrame, owner_->get_position_().y));
		}
		else if ((owner_->is_right_dir()))
		{
			owner_->set_is_reversal(true);
			owner_->set_position_(pPoint(owner_->get_position_().x + owner_->get_move_speed_() * g_SecondPerFrame, owner_->get_position_().y));
		}
	}


	if (!(owner_->get_physics_().get_isjump()))
	{
		owner_->SetTransition(FSM_Event::JUMP_END);
	}
	if (g_InputActionMap.attackKey== KEYSTAT::KEY_PUSH)
	{
		if(is_owner)
			owner_->SetTransition(FSM_Event::INPUT_ATTACK);
	}


	owner_->get_sprite_()->SetPosition(owner_->get_position_().x, owner_->get_position_().y);
}
