#pragma once
#include "PMobState.h"
class PMobHitAction :
	public PMobState
{
public:
	PMobHitAction(PMonster* parent);
	~PMobHitAction();
	float progress_time_ = 0.0f;
	float knockback_distance_ = 35.0f;
	float knockback_speed_ = 150.0f;

	pPoint damage_pos;
	int damage_length;

public:
	void Process(PPlayerCharacter* target);
	void KnockBack();
};

