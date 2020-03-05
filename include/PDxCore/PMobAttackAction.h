#pragma once
#include "PMobState.h"


class PMobAttackAction : public PMobState
{
public:
	PMobAttackAction(PMonster* parent);
	~PMobAttackAction();

	float progress_time = 0.0f;
	float focus_time = 0.0f;
	bool first_hit = false;
public:
	void Process(PPlayerCharacter* target);
};

