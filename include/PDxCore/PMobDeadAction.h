#pragma once
#include "PMobState.h"
class PMobDeadAction :
	public PMobState
{
public:
	PMobDeadAction(PMonster* parent);
	~PMobDeadAction();
public:
	float progress_time = 0.0f;
	void Process(PPlayerCharacter* target);
};

