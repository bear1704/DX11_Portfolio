#include "PBitmap.h"



PBitmap::PBitmap()
{
}


PBitmap::~PBitmap()
{
}

bool PBitmap::Init()
{
	return true;
}

bool PBitmap::Frame()
{
	return true;
}

bool PBitmap::Render()
{
	return true;
}

bool PBitmap::Release()
{
	if (handle_bitmap_ != NULL)
	{
		DeleteObject(handle_bitmap_);
	}
	return true;
}

bool PBitmap::Load(std::wstring filename)
{

	HANDLE hFile = CreateFile(filename.c_str(),
		GENERIC_READ, 0,
		NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	BITMAPFILEHEADER m_bitFileHeader;
	DWORD dwRead;
	ReadFile(hFile, &m_bitFileHeader,
		sizeof(BITMAPFILEHEADER), &dwRead, NULL);

	DWORD dwDataSize;
	dwDataSize = m_bitFileHeader.bfOffBits -
		sizeof(BITMAPFILEHEADER);
	BITMAPINFO* bitInfo = (BITMAPINFO*)malloc(dwDataSize);
	ReadFile(hFile, bitInfo,
		dwDataSize, &dwRead, NULL);

	PVOID pRaster = nullptr;
	handle_bitmap_ = CreateDIBSection(
		g_handle_screenDC, bitInfo, DIB_RGB_COLORS, &pRaster, NULL, 0);
	ReadFile(hFile, pRaster,
		m_bitFileHeader.bfSize - m_bitFileHeader.bfOffBits,
		&dwRead, NULL);
	free(bitInfo);
	CloseHandle(hFile);

	handle_memoryDC_ = CreateCompatibleDC(g_handle_screenDC);
	if (handle_memoryDC_ == NULL)
	{
		handle_memoryDC_ = CreateCompatibleDC(GetDC(g_hWnd));
		
	}

	SelectObject(handle_memoryDC_, handle_bitmap_);

	GetObject(handle_bitmap_, sizeof(BITMAP), &bitmap_info);
	return true;
}

bool PBitmap::Draw(float x, float y, FLOAT_RECT rect, DWORD draw_mode, float scale)
{
	float half_width = abs((rect.right * scale) / 2);
	float half_height = abs((rect.bottom * scale) / 2);

	SetStretchBltMode(g_handle_off_screenDC, HALFTONE);
	StretchBlt(g_handle_off_screenDC, x - half_width, y - half_height,
		 rect.right * scale, rect.bottom * scale,
		handle_memoryDC_,
		rect.left, rect.top, rect.right ,rect.bottom,
		draw_mode);
	return true;
}

bool PBitmap::Draw(float x, float y, FLOAT_RECT rect, BLENDFUNCTION bf, float scale, bool is_reversal)
{
	float half_width = abs((rect.right * scale) / 2);
	float half_height = abs((rect.bottom * scale) / 2);

	
	HDC reversal_DC = CreateCompatibleDC(handle_memoryDC_);
	HBITMAP bitmap = CreateCompatibleBitmap(handle_memoryDC_, rect.right, rect.bottom);
	SelectObject(reversal_DC, bitmap);

	if (is_reversal)
	{
		StretchBlt(reversal_DC, rect.right, 0,
			-rect.right, rect.bottom, handle_memoryDC_, rect.left, rect.top, rect.right, rect.bottom,
			SRCCOPY);

		AlphaBlend(g_handle_off_screenDC, x - half_width, y - half_height,
			rect.right * scale, rect.bottom* scale, reversal_DC, 0, 0, rect.right, rect.bottom,
			bf);

	}
	else
	{
		StretchBlt(reversal_DC, 0,0,
			rect.right, rect.bottom, handle_memoryDC_, rect.left, rect.top, rect.right, rect.bottom,
			SRCCOPY);
		AlphaBlend(g_handle_off_screenDC, x - half_width, y - half_height,
			rect.right * scale, rect.bottom * scale, reversal_DC, 0, 0, rect.right, rect.bottom,
			bf);
		//AlphaBlend(g_handle_off_screenDC, x - half_width, y - half_height,
		//	rect.right * scale, rect.bottom * scale, handle_memoryDC_, rect.left, rect.top, rect.right, rect.bottom,
		//	bf); //기존 알파블렌딩 코드
	}



	DeleteObject(bitmap);
	DeleteDC(reversal_DC);
	return true;
}

bool PBitmap::DrawNotCenter(float x, float y, FLOAT_RECT rect, BLENDFUNCTION bf, float scale)
{

	AlphaBlend(g_handle_off_screenDC, x, y,
		rect.right * scale, rect.bottom * scale, handle_memoryDC_, rect.left, rect.top, rect.right, rect.bottom,
		bf);

	return true;
}


bool PBitmap::DrawColorKey(float x, float y, FLOAT_RECT rect, COLORREF key_color)
{
	TransparentBlt(
		g_handle_off_screenDC, x, y, rect.right, rect.bottom, handle_memoryDC_, rect.left, rect.top,
		rect.right, rect.bottom, key_color);

	return true;
}

