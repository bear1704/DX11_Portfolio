#pragma once
#include "PObject.h"

class PUIComponent : public PObject
{


public:
	PUIComponent();
	virtual ~PUIComponent();

private:
	std::vector<PUIComponent*> component_list_; //polymorphism
	float gap = 33.0f;
	float accumtime = 0.0f;
public:
	virtual PUIComponent* Clone() { return nullptr; }
	virtual void Draw() {};

public:
	virtual bool Init() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;
	void Add(PUIComponent* component);
	void Set(multibyte_string data_path, multibyte_string object_name, pPoint position);
	void ReviseAllComponentPosition(pPoint& worldpos);
	std::vector<PUIComponent*>& get_component_list_();


public:
	pPoint hit_world_pos;
	void ResetDmgPresent();
	void DamageToSprite(int damage);
};

