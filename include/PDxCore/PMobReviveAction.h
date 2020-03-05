#pragma once
#include "PMobState.h"
class PMobReviveAction :
	public PMobState
{
public:
	PMobReviveAction(PMonster* parent);
	~PMobReviveAction();
public:
	float progress_time = 0.0f;
	void Process(PPlayerCharacter* target);
};

