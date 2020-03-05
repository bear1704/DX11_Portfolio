#include "PHitAction.h"
#include "PPlayerCharacter.h"


PHitAction::PHitAction(PPlayerCharacter* parent) : PPlayerState(parent)
{

}


PHitAction::~PHitAction()
{
}

void PHitAction::Process() //날아가는대로 바로 스테이트 체인지, 무적과 히트상태 풀기는 다른 곳에서
{
	if (owner_->get_sprite_()->get_animation_type_() != ANIMATIONTYPE::IDLE) //HIT는 IDLE과 모션을 공유, 단 깜빡임
	{
		owner_->set_sprite_(*owner_->find_sprite_by_type(ANIMATIONTYPE::IDLE));
		//owner_->set_invisible_(true);
	}


	if (owner_->get_status().get_hp_() < 0)
	{
		owner_->SetTransition(FSM_Event::HPEMPTY);
	}


	if (knockback_distance_ <= 0)
	{
		owner_->SetTransition(FSM_Event::TIME_OUT);
		owner_->set_hit_(false);
		knockback_distance_ = 45.0f;
	}
	
	
		CollisionCheck();
		KnockBack();
	
	




}

void PHitAction::KnockBack()
{
	if (knockback_distance_ > 0)
	{
		float deltatime = g_SecondPerFrame;

		if (deltatime > 0.1f)
			deltatime = 0.0f;

		pPoint pos = owner_->get_position_();
		pPoint nextpos;
		if (player_to_mob_side == SIDE::LEFT)
			nextpos = pPoint(pos.x + knockback_speed_ * deltatime, pos.y);
		else
			nextpos = pPoint(pos.x - knockback_speed_ * deltatime, pos.y);

		owner_->set_position_(nextpos);
		owner_->get_sprite_()->SetPosition(nextpos.x, nextpos.y);
		knockback_distance_ -= g_SecondPerFrame * knockback_speed_;
	}

}

void PHitAction::CollisionCheck()
{
	if (g_current_scene_)
	{
		//std::vector<PObject*> game_objects =
		//	PObjectDataManager::GetInstance().get_object_list_from_map(g_current_scene_->get_scene_name_()); //MUSHROOMRAND

		std::vector<PObject*> game_objects = *g_current_scene_->get_game_objects();

		//몬스터별 충돌체크(RECT가지고)
		for (int i = 0; i < game_objects.size(); i++)
		{
			if (game_objects[i]->get_type_() == Type::MONSTER)
			{
				PMonster* monster = (PMonster*)game_objects[i];

				if (monster->get_current_monster_state_() == FSM_State::DEAD)
					continue;   //죽은 몬스터는 히트판정 X

				if (monster->check_hit(owner_->get_collision_rect_()))
				{
					int damage = monster->get_status().mob_damage();

					player_to_mob_side =
						(owner_->get_collision_rect_().left - monster->get_collision_rect_().left) > 0 ? SIDE::LEFT : SIDE::RIGHT;

					if (!(owner_->get_invisible_()))
					{
						owner_->get_status().DecreaseHP(damage);
						owner_->set_invisible_(true);
					}
					
					break;
				}
			}
			else if (game_objects[i]->get_type_() == Type::BOSS_MONSTER)
			{
				PBossMonster* monster = (PBossMonster*)game_objects[i];

				if (monster->get_current_monster_state_() == FSM_State::DEAD)
					continue;   //죽은 몬스터는 히트판정 X

				if (monster->check_hit(owner_->get_collision_rect_()))
				{
					int damage = monster->get_status().mob_damage();

					player_to_mob_side =
						(owner_->get_collision_rect_().left - monster->get_collision_rect_().left) > 0 ? SIDE::LEFT : SIDE::RIGHT;

					if (!(owner_->get_invisible_()))
					{
						owner_->get_status().DecreaseHP(damage);
						owner_->set_invisible_(true);
					}

					break;
				}
			}
			
		}


	}
}