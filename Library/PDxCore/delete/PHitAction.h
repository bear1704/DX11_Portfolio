#pragma once
#include "PPlayerState.h"
#include "PObjectDataManager.h"
#include "PScene.h"



class PHitAction :
	public PPlayerState
{
public:
	PHitAction(PPlayerCharacter* parent);
	~PHitAction();
private:
	float knockback_distance_ = 45.0f;
	float knockback_speed_ = 200.0f;
	SIDE player_to_mob_side;
	bool invisible;
public:
	void Process();
	void KnockBack();
	void CollisionCheck();
};

