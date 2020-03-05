#pragma once
#include "PStd.h"
#include "PUIComponent.h"

class PComponentStorage : public PSingleton<PComponentStorage>
{

private:
	friend class PSingleton<PComponentStorage>;
	std::vector<PUIComponent*> ui_compositions_;
	std::vector<PObject*> game_objects_;
public:
	~PComponentStorage();
public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
	void InsertObject(PUIComponent* component);
	void InsertObject(std::vector<PObject*>& list_component);



};

