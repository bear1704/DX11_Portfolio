#pragma once
#include "PPlayerState.h"
class PDeadAction :
	public PPlayerState
{
public:
	PDeadAction(PPlayerCharacter* parent);
	~PDeadAction();
public:
	void Process();
};

