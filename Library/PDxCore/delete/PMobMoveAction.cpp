#include "PMobMoveAction.h"
#include "PMonster.h"


PMobMoveAction::PMobMoveAction(PMonster* parent) : PMobState(parent)
{
}


PMobMoveAction::~PMobMoveAction()
{
}

void PMobMoveAction::Process(PPlayerCharacter* target)
{
	if (owner_->get_sprite_()->get_animation_type_() != ANIMATIONTYPE::MOVE)
	{
		owner_->set_sprite_(*owner_->find_sprite_by_type(ANIMATIONTYPE::MOVE));
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

	progress_time_ += g_SecondPerFrame;
	direction_change_progress_time_ += g_SecondPerFrame;


	
	if (direction_change_progress_time_ > direction_change_time)
	{
		std::random_device r;
		std::mt19937 engine(r());
		std::uniform_int_distribution<int> distribution(0, 4);
		auto generator = std::bind(distribution, engine);
		int random = generator();

		//if (owner_->get_direction_side_() == SIDE::LEFT)
		if(random < 2)
		{
			owner_->set_direction_side_(SIDE::RIGHT);
			direction_change_progress_time_ = 0.0f;
		}
		else
		{
			owner_->set_direction_side_(SIDE::LEFT);
			direction_change_progress_time_ = 0.0f;
		}

		direction_change_time = random + 3;
	}

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
		}
		progress_time_ = 0.0f;
		change_time_ = random + 2;
	}

	if (owner_->get_direction_side_() == SIDE::LEFT)
	{
		owner_->set_is_reversal(false);
		owner_->set_position_(pPoint(owner_->get_position_().x - owner_->get_move_speed_() * g_SecondPerFrame, owner_->get_position_().y));
	}
	else
	{
		owner_->set_is_reversal(true);
		owner_->set_position_(pPoint(owner_->get_position_().x + owner_->get_move_speed_() * g_SecondPerFrame, owner_->get_position_().y));
	}
	

}
