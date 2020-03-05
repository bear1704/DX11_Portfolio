#pragma once
#include "PMobState.h"
class PMobIdleAction :
	public PMobState
{
public:
	PMobIdleAction(PMonster* parent);
	~PMobIdleAction();
	float progress_time_ = 0.0f;
	float change_time_ = 8.0f;

public:
	void Process(PPlayerCharacter* target) override;
};

