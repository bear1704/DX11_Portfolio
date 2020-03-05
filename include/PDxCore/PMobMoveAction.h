#pragma once
#include "PMobState.h"
class PMobMoveAction :
	public PMobState
{
public:
	PMobMoveAction(PMonster* parent);
	~PMobMoveAction();


	float progress_time_ = 0.0f;
	float direction_change_progress_time_ = 0.0f;
	float direction_change_time = 4.0f;
	float change_time_ = 6.0f;

public:
	void Process(PPlayerCharacter* target) override;
};

