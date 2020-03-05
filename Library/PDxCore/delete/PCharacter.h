#pragma once
#include "PObject.h"
#include "PPlayerStatus.h"


class PCharacter : public PObject
{
public:
	PCharacter();
	~PCharacter();

public:
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();
protected:
	multibyte_string character_name_;
	FLOAT_RECT foot_plane_;
	float move_speed_;	
	pPoint prev_position_;
	FLOAT_RECT attack_collision_box_;
	PPlayerStatus status;


public:
	void Movement();
	void Set(multibyte_string data_path, multibyte_string object_name, pPoint position);
	void set_collision_box_(FLOAT_RECT norm_box);
	FLOAT_RECT get_foot_plane_box();
	FLOAT_RECT get_attack_collision_box_();
	void PlatformWallCollision();
	void SavePrevPosition();
	float get_move_speed_();
	virtual void StatusSet(multibyte_string status_path, multibyte_string object_name);
	virtual void StatusSetForServer(multibyte_string status_path, multibyte_string object_name);
	PPlayerStatus& get_status();


};
