#pragma once
#include "PFiniteState.h"

class PFsm
{
public:
	PFsm();
	~PFsm();
private:
	std::map<FSM_State, shared_ptr<PFiniteState>> state_space_;

public:
	void Add(FSM_State key_inputstate, FSM_Event event, FSM_State key_outputstate);
	PFiniteState* get_state(FSM_State key);


};

