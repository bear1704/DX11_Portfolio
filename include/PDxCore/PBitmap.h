#pragma once
#include "PStd.h"

class PBitmap
{
public:
	PBitmap();
	virtual ~PBitmap();
public:
	HBITMAP  handle_bitmap_;
	HDC		 handle_memoryDC_;
	multibyte_string image_path_;
	multibyte_string image_name_;
	BITMAP bitmap_info;

public:
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();
	virtual bool Load(std::wstring filename);
	bool Draw(float x, float y, FLOAT_RECT rect, DWORD draw_mode, float scale = 1.0f);
	bool Draw(float x, float y, FLOAT_RECT rect, BLENDFUNCTION bf, float scale = 1.0f, bool is_reversal = false);
	bool DrawNotCenter(float x, float y, FLOAT_RECT rect, BLENDFUNCTION bf, float scale = 1.0f);
	bool DrawColorKey(float x, float y, FLOAT_RECT rect, COLORREF key_color);


};

