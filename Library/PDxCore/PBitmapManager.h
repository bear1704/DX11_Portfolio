#pragma once
#include "PBitmap.h"

class PBitmapManager : public PSingleton<PBitmapManager>
{
private:
	PBitmapManager();
public:
	~PBitmapManager();

private:
	friend class PSingleton<PBitmapManager>;
	std::map<int, PBitmap*> bitmap_list_;
	int index_;

public:
	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;
	int BitmapLoad(multibyte_string filepath, PLoadMode mode);
	PBitmap* get_bitmap_from_map(int key);
	bool Delete(int key);



public:

};

