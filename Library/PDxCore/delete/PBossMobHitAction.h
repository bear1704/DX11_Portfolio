#pragma once
#include "PBossMobState.h"


class PBossMobHitAction : public PBossMobState
{
public:
	PBossMobHitAction(PBossMonster* parent);
	~PBossMobHitAction();

	pPoint damage_pos;
	int damage_length;

public:
	void Process();
};

