#include "PFiniteState.h"
#include "assert.h"


FSM_State PFiniteState::get_next_state(FSM_Event key)
{
	auto iter = next_state_list_.find(key);
	//assert(iter != next_state_list_.end());
	if(iter == next_state_list_.end())
		return FSM_State::ERR;

	return iter->second;
}

PFiniteState::PFiniteState()
{
}


PFiniteState::~PFiniteState()
{
}
