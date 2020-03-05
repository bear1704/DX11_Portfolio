#pragma once
#include "PPlayerCharacter.h"
#include "PMobState.h"
#include "PFsm.h"

class PMonster : public PCharacter
{
public:
	PMonster();
	~PMonster();
private:
	multibyte_string monster_name_;
	SIDE my_direction_side_;
	SIDE enemy_to_direction_side_;
	bool ishit_;
	PPlayerCharacter* target_player_;
	int be_received_damage_;
	
	
public:
	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;
	PUIComponent* dmg_comp;
public:
	void Set(multibyte_string data_path, multibyte_string object_name, pPoint position);
	void MonsterWallCollision();
	void Movement();
	SIDE get_direction_side_();
	void set_direction_side_(SIDE side);
	void set_target_player_(PPlayerCharacter* player);

	void set_ishit_(bool hit);
	bool get_ishit_();
	void set_enemy_to_direction_side_(SIDE side);
	SIDE get_enemy_to_direction_side_();
	void set_be_received_damage_(int damage);
	int get_be_received_damage();




	PFsm monster_fsm_;
	PMobState* current_monster_action_;
	std::map<FSM_State, PMobState*> action_list_;
	FSM_State current_monster_state_;

	void SetTransition(FSM_Event event);
	void ProcessAction(PPlayerCharacter* target);
	bool check_hit(FLOAT_RECT player_attack_col);
	FSM_State get_current_monster_state_();

};

