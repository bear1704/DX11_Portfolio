#pragma once
#include "PPlayerState.h"
#include "PObjectDataManager.h"
#include "PScene.h"

class PAttackAction : public PPlayerState
{
public:
	PAttackAction(PPlayerCharacter* parent);
	~PAttackAction();

public:
	void Process();
	void AttackProcess();

private:
	PSprite effect_sprite;
};

