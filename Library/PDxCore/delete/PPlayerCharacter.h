#pragma once
#include "PCharacter.h"
#include "PFsm.h"
#include "PPlayerState.h"

class PPlayerCharacter : public PCharacter
{
public:
	PPlayerCharacter();
	~PPlayerCharacter();
private:
	multibyte_string player_character_name_;
	bool hit_;
	float flickering_rate;
	float invincible_rate;
	bool client_owner_character_;
	bool automata_right_dir_;
	bool is_character_dead_;
protected:

public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
	void Movement();
	void Set(multibyte_string data_path, multibyte_string object_name, pPoint position);
public:
	PFsm player_fsm_;
	PPlayerState* current_player_action_;
	std::map<FSM_State, PPlayerState*> action_list_;
	FSM_State current_player_state_;
	void SetTransition(FSM_Event event);
	void ProcessAction();
	void set_hit_(bool hit);
	void set_client_owner_character(bool isowner);
	void set_right_dir(bool isright);
	void set_is_character_dead(bool isdead);

	bool get_hit_();
	void InvincibleProgress();
	bool get_client_owner_character();
	bool get_is_character_dead();
	bool is_right_dir();


	
};

