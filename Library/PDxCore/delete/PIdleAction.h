#pragma once
#include "PPlayerState.h"
class PIdleAction :
	public PPlayerState
{
public:
	PIdleAction(PPlayerCharacter* parent);
	virtual ~PIdleAction();

public:
	void Process();
};

