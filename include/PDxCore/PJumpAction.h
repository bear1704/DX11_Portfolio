#pragma once
#include "PPlayerState.h"
class PJumpAction :
	public PPlayerState
{
public:
	PJumpAction(PPlayerCharacter* parent);
	~PJumpAction();

public:
	void Process();
};

