#pragma once
#include "PStd.h"

class PFsm;

class PFiniteState
{
	friend class PFsm;

private:
	std::map<FSM_Event, FSM_State> next_state_list_;
public:
	 FSM_State get_next_state(FSM_Event key);

public:
	PFiniteState();
	virtual ~PFiniteState();
};

