#include "PStd.h"

#pragma once


class PPhysicsModule
{
public:
	PPhysicsModule();
	~PPhysicsModule();

	

private:
		bool isjump;
		bool is_ground_;
		bool is_downphase_;
	

public:
	void Gravity(pPoint& position, float gravity);
	bool HitCalc();
	void Jump(float first_time, pPoint& position, float jump_force, float jump_length);
	void StartJump();
	bool get_isjump();
	void set_isjump(bool jump);
	void set_is_ground_(bool ground);
	bool get_is_ground_();
	bool get_is_downphase_();
	float jump_init_time = 999.0f;
};

