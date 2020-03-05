#include "PPhysicsModule.h"



PPhysicsModule::PPhysicsModule() 
{
	isjump = false;
	is_ground_ = false;
	is_downphase_ = true;
}


PPhysicsModule::~PPhysicsModule()
{
}

void PPhysicsModule::Jump(float first_time, pPoint & position, float jump_force,float jump_length)
{

	float last_time = g_fGameTimer;
	if ((last_time - first_time) <= (jump_length - jump_length/5) && isjump == true)
	{
 		position.y -= jump_force * g_SecondPerFrame;
	}
	else if ((last_time - first_time) >= (jump_length - jump_length / 5)&& (last_time - first_time) <= jump_length && isjump == true)
	{
   		position.y = position.y;
	}
	else if(isjump)
	{
  		is_downphase_ = true;
		
		if (is_ground_)
			isjump = false;
	}

}

void PPhysicsModule::StartJump()
{
	if (isjump)
		return;

	is_downphase_ = false;
	isjump = true;
	is_ground_ = false;
	jump_init_time = g_fGameTimer;
}

bool PPhysicsModule::get_isjump()
{
	return isjump;
}

void PPhysicsModule::set_isjump(bool jump)
{
	isjump = jump;
}

void PPhysicsModule::set_is_ground_(bool ground)
{
	is_ground_ = ground;
}

bool PPhysicsModule::get_is_ground_()
{
	return is_ground_;
}

bool PPhysicsModule::get_is_downphase_()
{
	return is_downphase_;
}


void PPhysicsModule::Gravity(pPoint & position, float gravity)
{
	float deltatime = g_SecondPerFrame;
	if (deltatime > 0.1f)
		deltatime = 0.0f;

	if (is_downphase_ == true)
		position.y += gravity * deltatime;
}

bool PPhysicsModule::HitCalc()
{
	return true;
}
