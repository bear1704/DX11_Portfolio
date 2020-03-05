#pragma once
#include "PSprite.h"

class PObjectRotateUtil
{
public:
	PObjectRotateUtil();
	~PObjectRotateUtil();
public:
	static void RotateAndDraw(PSprite* sprite, int image_number, float posX, float posY, float angle,float alpha, float scale,  bool clockwise = true);
	static void GetRotationBitmap(PSprite* one_frame_sprite, HDC& colorDC, float angle, float width, float height, bool clockwise);
};

