#pragma once
#include "PStd.h"
#include "PObject.h"



class PObjectDataManager : public PSingleton<PObjectDataManager>
{
public:
	~PObjectDataManager();


private:
	friend class PSingleton<PObjectDataManager>;
	std::map<std::wstring, std::vector<PObject*>> object_composition_list_;
	std::map<std::wstring, std::vector<PSprite*>> object_animation_list_;
	int index_;
	bool need_load_data_;
public:
	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;

	std::vector<PObject*> get_object_list_from_map(std::wstring key);
	std::vector<PSprite*> get_animation_list_from_map(std::wstring key);
	void LoadDataFromScript(multibyte_string filepath);  
	void LoadAnimationDataFromScriptEx(multibyte_string filepath);
	//void LoadAnimationDataFromScript(multibyte_string filepath);


};

