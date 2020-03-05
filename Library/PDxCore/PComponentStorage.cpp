#include "PComponentStorage.h"



PComponentStorage::~PComponentStorage()
{
}

bool PComponentStorage::Init()
{
	return false;
}

bool PComponentStorage::Frame()
{
	return false;
}

bool PComponentStorage::Render()
{
	return false;
}

bool PComponentStorage::Release()
{
	return false;
}

void PComponentStorage::InsertObject(PUIComponent * component)
{
	ui_compositions_.push_back(component);
}

void PComponentStorage::InsertObject(std::vector<PObject*>& list_component)
{
	game_objects_ = list_component;
}