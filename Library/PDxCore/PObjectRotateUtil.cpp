#include "PObjectRotateUtil.h"



PObjectRotateUtil::PObjectRotateUtil()
{
}


PObjectRotateUtil::~PObjectRotateUtil()
{
}
void PObjectRotateUtil::RotateAndDraw(PSprite* sprite, int image_number ,float posX, float posY, float angle, float alpha, float scale, bool clockwise)
{
	PSprite one_frame_sprite;
	SpriteDataInfo sdi;
	sdi.rect_list.resize(1);
	sdi.bitmap_path = sprite->bitmap_->image_path_ + sprite->bitmap_->image_name_;
	sdi.lifetime = 777;
	sdi.max_frame = 1;  sdi.once_playtime = 1;  sdi.rect_list[0] = sprite->get_rect_list_copy()[image_number];  //1프레임만 복사 
	sdi.scale = 1.0f; sdi.posX = posX;   sdi.posY = posY;
	one_frame_sprite.Set(sdi, 1.0f, 1.0f);
	
	
	int new_width = one_frame_sprite.get_rect_list_copy()[0].right;
	int new_height = one_frame_sprite.get_rect_list_copy()[0].bottom;

	float new_size = sqrt(new_width*new_width + new_height * new_height);

	HDC colorDC;

	GetRotationBitmap(&one_frame_sprite, colorDC, angle , new_size, new_size,  clockwise);

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = alpha * 255.0f;
	bf.AlphaFormat = AC_SRC_ALPHA;
	AlphaBlend(g_handle_off_screenDC, posX - one_frame_sprite.get_rect_list_copy()[0].right/2 , 
	posY - one_frame_sprite.get_rect_list_copy()[0].bottom / 2,
		new_size * scale, new_size * scale,
	colorDC, 0, 0,
		new_size, new_size,
	bf);



	//AlphaBlend(g_handle_off_screenDC, posX - one_frame_sprite.get_rect_list_copy()[0].right/2 , 
	//	posY - one_frame_sprite.get_rect_list_copy()[0].bottom / 2,
	//	one_frame_sprite.get_rect_list_copy()[0].right * scale, one_frame_sprite.get_rect_list_copy()[0].bottom * scale,
	//	colorDC, 0, 0,
	//	new_width, new_height,
	//	bf);


	DeleteDC(colorDC);

	
}
void PObjectRotateUtil::GetRotationBitmap(PSprite* one_frame_sprite, HDC& colorDC, float angle, float width, float height ,bool clockwise)
{
	float origin_size_width = one_frame_sprite->get_rect_list_copy()[0].right;
	float origin_size_height = one_frame_sprite->get_rect_list_copy()[0].bottom;

	HDC srcDC = one_frame_sprite->bitmap_->handle_memoryDC_;
	HDC destDC = CreateCompatibleDC(NULL);

	FLOAT_RECT sprite_rect = one_frame_sprite->get_rect_list_copy()[0];

		int rotate_direction = 1;

		HBITMAP bitmap = CreateCompatibleBitmap(g_handle_screenDC, width, height);
		if (bitmap == NULL)
			return;
		SelectObject(destDC , bitmap);


		HBRUSH hbrBrush = CreateSolidBrush(RGB(255, 255, 255));
		SelectObject(destDC, hbrBrush);
		PatBlt(destDC, 0, 0, width, height, PATCOPY);
		DeleteObject(hbrBrush);

		if (!clockwise)
			rotate_direction *= -1;


		float radian = (angle * 3.141592f ) / 180.0f;
		float cosine = cos(radian);
		float sine = sin(radian) * rotate_direction;

		XFORM xform;
		xform.eM11 = cosine; xform.eM12 = sine;
		xform.eM21 = -sine; xform.eM22 = cosine;
		//xform.eDx = width / 2;
		//xform.eDy = height/ 2;

		xform.eDx = (width / 2);
		xform.eDy = (height / 2);

		int old_graphic = SetGraphicsMode(destDC, GM_ADVANCED);
		SetWorldTransform(destDC, &xform);



		BitBlt(destDC, -(sprite_rect.right / 2),
			-(sprite_rect.bottom / 2),
			sprite_rect.right, sprite_rect.bottom,
			srcDC, sprite_rect.left, sprite_rect.top, SRCCOPY);


		xform.eM11 = 1; xform.eM12 = 0;
		xform.eM21 = 0; xform.eM22 = 1;
		xform.eDx = 0; xform.eDy = 0;
		SetWorldTransform(destDC, &xform);
		SetGraphicsMode(destDC, old_graphic);

		colorDC = destDC;

		DeleteObject(bitmap);
		DeleteDC(destDC);

	//FLOAT_RECT 회전방법 정의

}
