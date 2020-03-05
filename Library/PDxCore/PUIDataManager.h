#pragma once
#include "PStd.h"
#include "PButtonControl.h"
#include "PImageControl.h"

class PUIDataManager : public PSingleton<PUIDataManager>
{
private:
	PUIDataManager();
public:
	virtual ~PUIDataManager();
private:
	friend class PSingleton<PUIDataManager>;
	std::map<std::wstring, PUIComponent*> ui_composition_list_;
	int index_;
	bool need_load_data_;
public:
	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;

	PUIComponent* get_ui_composition_list_from_map(std::wstring key);
	void LoadDataFromScript(multibyte_string filepath);  //파일에서 스프라이트 정보(Rect 좌표 등...)를 전부 읽어서 맵에 저장한다. name maxframe lifetime once_playtime path
	//bool Delete(int key);


};
