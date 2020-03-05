#include "PMobIdleAction.h"
#include "PMonster.h"

PMobIdleAction::PMobIdleAction(PMonster* parent) : PMobState(parent)
{
}


PMobIdleAction::~PMobIdleAction()
{
}

void PMobIdleAction::Process(PPlayerCharacter* target)
{
	if (owner_->get_sprite_()->get_animation_type_() != ANIMATIONTYPE::IDLE)
	{
		owner_->set_sprite_(*owner_->find_sprite_by_type(ANIMATIONTYPE::IDLE));
	}

	if (target != nullptr) // 플레이어가 아직 스폰되지 않았다면,
	{
		if (PCollision::GetInstance().RectInRect(target->get_collision_rect_(), owner_->get_collision_rect_()))
		{  //플레이어와의 충돌 체크 
			target->set_hit_(true);
		}
	}

	if (owner_->get_ishit_())
	{
		owner_->set_ishit_(false);
		owner_->SetTransition(FSM_Event::HIT);
	}
	progress_time_ += g_SecondPerFrame;
	if (progress_time_ > change_time_)
	{
		std::random_device r;
		std::mt19937 engine(r());
		std::uniform_int_distribution<int> distribution(0, 4);
		auto generator = std::bind(distribution, engine);
		int random = generator();


		if (random < 2)
		{
			owner_->SetTransition(FSM_Event::MOB_TIME_OUT);
		}
		else
		{
			owner_->SetTransition(FSM_Event::MOB_JUMP_TIME_OUT);
			//owner_->SetTransition(FSM_Event::MOB_JUMP_TIME_OUT); //Jump
		}
		progress_time_ = 0.0f;

		change_time_ = random + 2.0f;

	}



}
