#pragma once
#include "PBossMobState.h"
class PBossMobIdleAction :
	public PBossMobState
{
public:
	PBossMobIdleAction(PBossMonster* parent);
	~PBossMobIdleAction();

public:
	void Process() override;
};

