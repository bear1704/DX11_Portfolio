#include "PPlayerCharacter.h"
#include "PIdleAction.h"
#include "PMoveAction.h"
#include "PJumpAction.h"
#include "PAttackAction.h"
#include "PDeadAction.h"
#include "PHitAction.h"

PPlayerCharacter::PPlayerCharacter()
{
	flickering_rate = 0.0f;
	invincible_rate = 0.0f;
}


PPlayerCharacter::~PPlayerCharacter()
{
	
}

bool PPlayerCharacter::Init()
{
	status.Init();
	action_list_.insert(std::make_pair(FSM_State::IDLE, new PIdleAction(this)));
	action_list_.insert(std::make_pair(FSM_State::MOVE, new PMoveAction(this)));
	action_list_.insert(std::make_pair(FSM_State::JUMP, new PJumpAction(this)));
	action_list_.insert(std::make_pair(FSM_State::ATTACK, new PAttackAction(this)));
	action_list_.insert(std::make_pair(FSM_State::HIT, new PHitAction(this)));
	action_list_.insert(std::make_pair(FSM_State::DEAD, new PDeadAction(this)));

	current_player_action_ = action_list_[FSM_State::IDLE];
	current_player_state_ = FSM_State::IDLE;
	
	player_fsm_.Add(FSM_State::IDLE, FSM_Event::HPEMPTY, FSM_State::DEAD);
	//MOVE
	player_fsm_.Add(FSM_State::IDLE, FSM_Event::INPUT_MOVE, FSM_State::MOVE);
	player_fsm_.Add(FSM_State::MOVE, FSM_Event::INPUT_NONE, FSM_State::IDLE);
	player_fsm_.Add(FSM_State::MOVE, FSM_Event::HPEMPTY, FSM_State::DEAD);
	//JUMP
	player_fsm_.Add(FSM_State::IDLE, FSM_Event::INPUT_JUMP, FSM_State::JUMP);
	player_fsm_.Add(FSM_State::MOVE, FSM_Event::INPUT_JUMP, FSM_State::JUMP);
	player_fsm_.Add(FSM_State::JUMP, FSM_Event::JUMP_END, FSM_State::MOVE);

	//ATTACK
	player_fsm_.Add(FSM_State::IDLE, FSM_Event::INPUT_ATTACK, FSM_State::ATTACK);
	player_fsm_.Add(FSM_State::MOVE, FSM_Event::INPUT_ATTACK, FSM_State::ATTACK);
	player_fsm_.Add(FSM_State::JUMP, FSM_Event::INPUT_ATTACK, FSM_State::ATTACK);
	player_fsm_.Add(FSM_State::ATTACK, FSM_Event::ATTACK_END, FSM_State::IDLE);//MOVE, JUMP는 공격시에 불가능함. 오직 IDLE로만 회귀

	//HIT
	player_fsm_.Add(FSM_State::IDLE, FSM_Event::HIT, FSM_State::HIT);
	player_fsm_.Add(FSM_State::MOVE, FSM_Event::HIT, FSM_State::HIT);
	player_fsm_.Add(FSM_State::JUMP, FSM_Event::HIT, FSM_State::HIT);
	player_fsm_.Add(FSM_State::ATTACK, FSM_Event::HIT, FSM_State::HIT);
	player_fsm_.Add(FSM_State::HIT, FSM_Event::TIME_OUT, FSM_State::IDLE);
	player_fsm_.Add(FSM_State::HIT, FSM_Event::HPEMPTY, FSM_State::DEAD);

	status.ModifyHP(1930);
	status.ModifyMP(1930);
	status.ModifyEXP(0);


	return true;
}

bool PPlayerCharacter::Frame()
{
	SavePrevPosition();
	ProcessAction();
	sprite_.Frame();
	set_collision_box_(collision_box_norm_);
	Movement();
	physics_.Jump(physics_.jump_init_time, position_, 800, 0.2f);
	physics_.Gravity(position_, gravity_);
	PlatformWallCollision();
	status.Frame();
	InvincibleProgress();
	return true;
}

bool PPlayerCharacter::Render()
{
	Spawn();
	status.Render();
	return true;
}

bool PPlayerCharacter::Release()
{
	sprite_.Release();
	status.Release();
	return true;
}

void PPlayerCharacter::Movement()
{


	//if (g_InputActionMap.leftArrowKey == KEYSTAT::KEY_HOLD)
	//{
	//	is_reversal_ = false;
	//	set_position_(pPoint(position_.x - move_speed_ * g_SecondPerFrame, position_.y));
	//}
	//if (g_InputActionMap.rightArrowKey == KEYSTAT::KEY_HOLD)
	//{
	//	is_reversal_ = true;
	//	set_position_(pPoint(position_.x + move_speed_ * g_SecondPerFrame, position_.y));
	//}

	if (g_InputActionMap.jumpKey == KEYSTAT::KEY_PUSH)
	{
		if(client_owner_character_)
			physics_.StartJump();
	}
	if (g_InputActionMap.qKey == KEYSTAT::KEY_PUSH)
	{
		status.ModifyHP(240);
	}
	if (g_InputActionMap.wKey == KEYSTAT::KEY_PUSH)
	{
		status.ModifyHP(1930);
	}
	if (g_InputActionMap.aKey == KEYSTAT::KEY_PUSH)
	{
		status.ModifyMP(10);
	}
	if (g_InputActionMap.sKey == KEYSTAT::KEY_PUSH)
	{
		status.ModifyMP(300);
	}
	if (g_InputActionMap.dKey == KEYSTAT::KEY_PUSH)
	{
		//status.ModifyEXP(30);
	}if (g_InputActionMap.fKey == KEYSTAT::KEY_PUSH)
	{
		//status.ModifyEXP(10030);
	}

}

void PPlayerCharacter::Set(multibyte_string data_path, multibyte_string object_name, pPoint position)
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

	if (character_name_ == L"player")
		client_owner_character_ = true;

	//PSpriteManager::GetInstance().LoadDataFromScript(info.sprite_path);
	//sprite_.Set(*PSpriteManager::GetInstance().get_sprite_data_list_from_map(info.sprite_name), alpha_, scale_);
	PSpriteManager::GetInstance().LoadSpriteDataFromScript(info.sprite_path, ObjectLoadType::CHARACTER);
	sprite_.Clone(PSpriteManager::GetInstance().get_sprite_from_map_ex(info.sprite_name), alpha_, scale_);
	sprite_.SetPosition(position_.x, position_.y);

	FLOAT_RECT scaled_collisionbox_norm = { collision_box_norm_.left*scale_, collision_box_norm_.top*scale_ ,
	collision_box_norm_.right*scale_, collision_box_norm_.bottom*scale_ };

	collision_box_norm_ = scaled_collisionbox_norm;
	set_collision_box_(collision_box_norm_);

	spawn_position_ = position;
	attack_collision_box_ = { 0, 0, 105, 100 }; //임시로 position,크기 지정 

	if(get_client_owner_character())
		P2DCamera::GetInstance().set_character_collision_rect(&collision_box_); //캐릭터 only
}


void PPlayerCharacter::SetTransition(FSM_Event event)
{
	PFiniteState* state = player_fsm_.get_state(current_player_state_); //FSM공간에서 플레이어의 현재 스테이트를 가져온다.
	if (!state) return; //현재 스테이트가 없으면 에러 

	FSM_State next = state->get_next_state(event); //현재 스테이트에서 이벤트로 트랜지션되는 다음 스테이트 가져오기
	
	if (next == FSM_State::ERR) //다음 스테이트가 없을때 현재 스테이트 고정
		next = current_player_state_;


	current_player_action_ = action_list_[next]; //그 다음 스테이트에 맞게 액션 가져오기.(스테이트 전환)
	current_player_state_ = next;  //현재 스테이트 변수도 다음스테이트를 가리키게 전환
}

void PPlayerCharacter::ProcessAction()
{
	current_player_action_->Process();
}

void PPlayerCharacter::set_hit_(bool hit)
{
	hit_ = hit;
}


void PPlayerCharacter::set_client_owner_character(bool isowner)
{
	client_owner_character_ = isowner;
}

void PPlayerCharacter::set_right_dir(bool isright)
{
	automata_right_dir_ = isright;
}


void PPlayerCharacter::set_is_character_dead(bool isdead)
{
	is_character_dead_ = isdead;
}

bool PPlayerCharacter::get_hit_()
{
	return hit_;
}



void PPlayerCharacter::InvincibleProgress()
{
	if (get_invisible_() == true)
	{
	

		if (flickering_rate < 0.15f)
		{
			sprite_.set_alpha_(0.2f);
		}
		else if (flickering_rate < 0.3f)
		{
			sprite_.set_alpha_(1.0f);
		}
		else
		{
			flickering_rate = 0.0f;
		}

		flickering_rate += g_SecondPerFrame;
		invincible_rate += g_SecondPerFrame;

		if (invincible_rate > 3.0f)
		{
			set_invisible_(false);
			invincible_rate = 0.0f;
			flickering_rate = 0.0f;
			sprite_.set_alpha_(1.0f);

		}

	}
}

bool PPlayerCharacter::get_client_owner_character()
{
	return client_owner_character_;
}

bool PPlayerCharacter::get_is_character_dead()
{
	return is_character_dead_;
}

bool PPlayerCharacter::is_right_dir()
{
	return automata_right_dir_;
}
