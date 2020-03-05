#pragma once
#include "PPlayerState.h"
class PMoveAction :
	public PPlayerState
{
public:
	PMoveAction(PPlayerCharacter* parent);
	~PMoveAction();

public:
	void Process();

};

