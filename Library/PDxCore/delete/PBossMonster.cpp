#include "PBossMonster.h"
#include "PBossMobIdleAction.h"
#include "PBossMobHitAction.h"
#include "PPlayerCharacter.h"
#include "PScene.h"

HANDLE g_handle_4s_sprite_timer_queue_;
HANDLE g_handle_4s_sprite_timer_;


//VOID CALLBACK BossSkillTimerCallBack(PVOID lpParam, BOOLEAN TimerOrWaitFired)
//{
//
//	PSprite* sprite = (PSprite*)lpParam;
//	
//	*sprite = *PSpriteManager::GetInstance().get_sprite_from_map_ex(L"zakum_idle");
//	(*sprite).SetPosition(720, 490);
//	(*sprite).set_scale_(1.3f);
//	
//
//	DeleteTimerQueueTimer(g_handle_4s_sprite_timer_queue_, g_handle_4s_sprite_timer_, nullptr);
//}
//
//
//VOID CALLBACK ExplosionCallBack(PVOID lpParam, BOOLEAN TimerOrWaitFired)
//{
//
//	UserListForStorage* list = (UserListForStorage*)lpParam;
//	PPlayerCharacter* mychar = (PPlayerCharacter*)g_current_scene_->FindObjectByCid(list->cid);
//	PSoundMgr::GetInstance().Play(PSoundMgr::GetInstance().Load(L"data/sound/pang.mp3"));
//	if (!PCollision::GetInstance().RectInRect(list->safe_zone, mychar->get_collision_rect_()))
//	{
//
//		mychar->get_status().DecreaseHP(500);
//	}
//	else
//	{
//		PSoundMgr::GetInstance().Play(PSoundMgr::GetInstance().Load(L"data/sound/defence.mp3"));
//		mychar->get_status().IncreaseHP(700);
//	}
//
//	PSprite explosion;
//	explosion.Clone(PSpriteManager::GetInstance().get_sprite_from_map_ex(L"boom"), 1.0f, 1.3f);
//	explosion.SetPosition(720, 300);
//	PSpriteManager::GetInstance().AddRenderWaitList(explosion);
//
//	PBossMonster* bm = (PBossMonster*) g_current_scene_->FindObjectByCid(20000);
//	bm->set_sprite_(*PSpriteManager::GetInstance().get_sprite_from_map_ex(L"zakum_idle"));
//	bm->get_sprite_()->set_scale_(1.3f);
//	bm->get_sprite_()->SetPosition(720,490);
//	bm->ResetSkill();
//
//	DeleteTimerQueueTimer(g_handle_4s_sprite_timer_queue_, g_handle_4s_sprite_timer_, nullptr);
//}

PBossMonster::PBossMonster()
{
}

PBossMonster::~PBossMonster()
{
}

bool PBossMonster::Init()
{
	action_list_.insert((std::make_pair(FSM_State::BOSS_IDLE, new PBossMobIdleAction(this))));
	action_list_.insert((std::make_pair(FSM_State::BOSS_HIT, new PBossMobHitAction(this))));

	current_boss_monster_action_ = action_list_[FSM_State::BOSS_IDLE];
	current_boss_monster_state_ = FSM_State::BOSS_IDLE;

	boss_monster_fsm_.Add(FSM_State::BOSS_IDLE, FSM_Event::HIT, FSM_State::BOSS_HIT);
	boss_monster_fsm_.Add(FSM_State::BOSS_HIT, FSM_Event::INPUT_NONE, FSM_State::BOSS_IDLE);

	status.Init();
	return false;
}

void PBossMonster::Set(multibyte_string data_path, multibyte_string object_name, pPoint position)
{
	PObjectInfoManager::GetInstance().LoadDataFromScript(data_path, ObjectLoadType::CHARACTER);
	ObjectInfo info = *(PObjectInfoManager::GetInstance().get_object_info_list_from_map(object_name));

	character_name_ = info.object_name_;
	object_name_ = info.object_name_;
	position_ = position;
	collision_box_norm_ = info.collision_box_;
	alpha_ = info.alpha_;
	scale_ = info.scale_;

	PSpriteManager::GetInstance().LoadSpriteDataFromScript(info.sprite_path, ObjectLoadType::CHARACTER);
	sprite_.Clone(PSpriteManager::GetInstance().get_sprite_from_map_ex(info.sprite_name), alpha_, scale_);

	sprite_.SetPosition(position_.x, position_.y);

	FLOAT_RECT scaled_collisionbox_norm = { collision_box_norm_.left * scale_, collision_box_norm_.top * scale_ ,
	collision_box_norm_.right * scale_, collision_box_norm_.bottom * scale_ };

	collision_box_norm_ = scaled_collisionbox_norm;
	set_collision_box_(collision_box_norm_);

	spawn_position_ = position;

}

void PBossMonster::SetForServer(multibyte_string data_path, multibyte_string object_name, pPoint position)
{
	PObjectInfoManager::GetInstance().LoadDataFromScript(data_path, ObjectLoadType::CHARACTER);
	ObjectInfo info = *(PObjectInfoManager::GetInstance().get_object_info_list_from_map(object_name));

	character_name_ = info.object_name_;
	object_name_ = info.object_name_;
	position_ = position;
	collision_box_norm_ = info.collision_box_;
	alpha_ = info.alpha_;
	scale_ = info.scale_;

	FLOAT_RECT scaled_collisionbox_norm = { collision_box_norm_.left * scale_, collision_box_norm_.top * scale_ ,
	collision_box_norm_.right * scale_, collision_box_norm_.bottom * scale_ };

	collision_box_norm_ = scaled_collisionbox_norm;
	set_collision_box_(collision_box_norm_);
}

void PBossMonster::SetTransition(FSM_Event event)
{
	
		PFiniteState* state = boss_monster_fsm_.get_state(current_boss_monster_state_); //FSM공간에서 플레이어의 현재 스테이트를 가져온다.
		if (!state) assert(!state); //현재 스테이트가 없으면 에러 

		FSM_State next = state->get_next_state(event); //현재 스테이트에서 이벤트로 트랜지션되는 다음 스테이트 가져오기

		if (next == FSM_State::ERR) //다음 스테이트가 없을때 현재 스테이트 고정
			next = current_boss_monster_state_;

		current_boss_monster_action_ = action_list_[next]; //그 다음 스테이트에 맞게 액션 가져오기.(스테이트 전환)
		current_boss_monster_state_ = next;  //현재 스테이트 변수도 다음스테이트를 가리키게 전환

	
}

void PBossMonster::ProcessAction()
{
	current_boss_monster_action_->Process();
}

bool PBossMonster::check_hit(FLOAT_RECT player_attack_col)
{
	if (PCollision::GetInstance().RectInRect(player_attack_col, collision_box_))
		return true;

	return false;
}

FSM_State PBossMonster::get_current_monster_state_()
{
	return FSM_State();
}

bool PBossMonster::Frame()
{
	sprite_.Frame();
	status.Frame();
	ProcessAction();
	set_collision_box_(collision_box_norm_);



	return false;
}

bool PBossMonster::Render()
{
	Spawn();
	status.Render();

	return false;
}

bool PBossMonster::Release()
{
	sprite_.Release();
	return false;
}


void PBossMonster::set_ishit_(bool hit)
{
	ishit_ = hit;
}

bool PBossMonster::get_ishit_()
{
	return ishit_;
}

void PBossMonster::set_be_received_damage_(int damage)
{
	be_received_damage_ = damage;
}

int PBossMonster::get_be_received_damage()
{
	return be_received_damage_;
}

void PBossMonster::StatusSet(multibyte_string status_path, multibyte_string object_name)
{
	status.StatusSet(status_path, object_name);
	
	
}



//void PBossMonster::StartSkillPhase(int skill_number)
//{
//	if (skill_number == 0)
//	{
//
//
//		MeteorRandNumber num = PNetworkDataStorage::GetInstance().PopMeteorData();
//		
//		current_skill_ = skill_list_[skill_number];
//		 current_skill_->Start(num.initpos, num.downspeed);
//
//		 PSoundMgr::GetInstance().Play(PSoundMgr::GetInstance().Load(L"data/sound/casting.mp3"));
//		 PSoundMgr::GetInstance().Play(PSoundMgr::GetInstance().Load(L"data/sound/drop.mp3"));
//		 set_sprite_(*find_sprite_by_type(ANIMATIONTYPE::SKILLONE));
//		 sprite_.SetPosition(position_.x, position_.y);
//		 CreateTimerQueueTimer(&g_handle_4s_sprite_timer_, g_handle_4s_sprite_timer_queue_, (WAITORTIMERCALLBACK)BossSkillTimerCallBack,
//			 &sprite_, 4000, 0, WT_EXECUTEDEFAULT);
//		
//	}
//	else if (skill_number == 1)
//	{
//
//
//		WORD randnum = PNetworkDataStorage::GetInstance().get_explosion_rand_number_();
//
//		current_skill_ = skill_list_[skill_number];
//		current_skill_->Start(randnum);
//		current_skill_->Init();
//		PSoundMgr::GetInstance().Play(PSoundMgr::GetInstance().Load(L"data/sound/casting.mp3"));
//		set_sprite_(*find_sprite_by_type(ANIMATIONTYPE::SKILLTWO));
//		sprite_.SetPosition(position_.x, position_.y);
//		
//
//		UserListForStorage list = PNetworkDataStorage::GetInstance().PopUserData();
//		UserListForStorage* sendlist = new UserListForStorage;
//		sendlist->cid = list.cid;
//		sendlist->safe_zone = list.safe_zone;
//		CreateTimerQueueTimer(&g_handle_4s_sprite_timer_, g_handle_4s_sprite_timer_queue_, (WAITORTIMERCALLBACK)ExplosionCallBack,
//			sendlist, 5000, 0, WT_EXECUTEDEFAULT);
//	}
//}

void PBossMonster::ChangeSprite(PSprite* sprite)
{
	sprite_ = *sprite;
}

