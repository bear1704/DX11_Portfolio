#include "PBossMobHitAction.h"
#include "PBossMonster.h"
#include "PSoundMgr.h"

PBossMobHitAction::PBossMobHitAction(PBossMonster* parent) : PBossMobState(parent)
{
}

PBossMobHitAction::~PBossMobHitAction()
{
}

void PBossMobHitAction::Process()
{


	if (owner_->get_sprite_()->get_animation_type_() != ANIMATIONTYPE::IDLE)
	{
		//owner_->set_sprite_(*owner_->find_sprite_by_type(ANIMATIONTYPE::IDLE));
		PSoundMgr::GetInstance().Play(PSoundMgr::GetInstance().Load(L"data/sound/skill_hit.mp3"));
		//owner_->damage_present_image_->hit_world_pos = pPoint(owner_->get_position_().x, owner_->get_position_().y - 65.0f);
		//owner_->damage_present_image_->DamageToSprite(owner_->get_be_received_damage());
		//owner_->damage_present_image_->ResetDmgPresent();
	}

	//if (PCollision::GetInstance().RectInRect(target->get_collision_rect_(), owner_->get_collision_rect_()))
	//{  //플레이어와의 충돌 체크 
	//	if (target->get_hit_() == false)
	//		target->set_hit_(true);
	//}


	//if (owner_->get_status().get_hp_() <= 0)
	//{
	//	owner_->SetTransition(FSM_Event::HPEMPTY);
	//}
	owner_->SetTransition(FSM_Event::INPUT_NONE);

}
