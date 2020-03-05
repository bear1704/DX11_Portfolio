#include "PCollision.h"
#include <assert.h>


PCollision::PCollision()
{

}


PCollision::~PCollision()
{
}

bool PCollision::RectInRect(FLOAT_RECT collision_box1, FLOAT_RECT collision_box2)
{
	pPoint collision_box1_center;
	pPoint collision_box2_center;
	collision_box1_center.x = (collision_box1.right / 2 + collision_box1.left);
	collision_box1_center.y = (collision_box1.bottom / 2 + collision_box1.top);
	collision_box2_center.x = (collision_box2.right / 2 + collision_box2.left);
	collision_box2_center.y = (collision_box2.bottom / 2 + collision_box2.top);

	pPoint collision_box1_radius;
	pPoint collision_box2_radius;
	collision_box1_radius.x = collision_box1.right / 2;
	collision_box1_radius.y = collision_box1.bottom / 2;
	collision_box2_radius.x = collision_box2.right / 2;
	collision_box2_radius.y = collision_box2.bottom / 2;

	pPoint between_rect_distance;
	between_rect_distance.x = abs(collision_box2_center.x - collision_box1_center.x);
	between_rect_distance.y = abs(collision_box2_center.y - collision_box1_center.y);


	if (between_rect_distance.x <= (collision_box1_radius.x + collision_box2_radius.x) &&
		between_rect_distance.y <= (collision_box1_radius.y + collision_box2_radius.y))
	{
		return true;
	}

	return false;

}

bool PCollision::RectInRect(FLOAT_RECT collision_box1, FLOAT_RECT collision_box2, float& overlapping_ylength)
{
	pPoint collision_box1_center;
	pPoint collision_box2_center;
	collision_box1_center.x = (collision_box1.right / 2 + collision_box1.left);
	collision_box1_center.y = (collision_box1.bottom / 2 + collision_box1.top);
	collision_box2_center.x = (collision_box2.right / 2 + collision_box2.left);
	collision_box2_center.y = (collision_box2.bottom / 2 + collision_box2.top);

	pPoint collision_box1_radius;
	pPoint collision_box2_radius;
	collision_box1_radius.x = collision_box1.right / 2;
	collision_box1_radius.y = collision_box1.bottom / 2;
	collision_box2_radius.x = collision_box2.right / 2;
	collision_box2_radius.y = collision_box2.bottom / 2;

	pPoint between_rect_distance;
	between_rect_distance.x = abs(collision_box2_center.x - collision_box1_center.x);
	between_rect_distance.y = abs(collision_box2_center.y - collision_box1_center.y);

	overlapping_ylength = (collision_box1_radius.y + collision_box2_radius.y) - between_rect_distance.y;

	if (between_rect_distance.x <= (collision_box1_radius.x + collision_box2_radius.x) &&
		between_rect_distance.y <= (collision_box1_radius.y + collision_box2_radius.y))
	{
		return true;
	}


	return false;

}

bool PCollision::RectInPoint(FLOAT_RECT collision_box, pPoint point)
{
	if (collision_box.left <= point.x && collision_box.right >= point.x)
	{
		if (collision_box.top <= point.y && collision_box.bottom >= point.y)
		{
			return true;
		}
	}

	return false;
}

bool PCollision::RectInPoint(FLOAT_RECT collision_box, POINT point)
{
	if (collision_box.left <= point.x && collision_box.left + collision_box.right >= point.x)
	{
		if (collision_box.top <= point.y &&  collision_box.top + collision_box.bottom >= point.y)
		{
			return true;
		}
	}

	return false;
}


bool PCollision::CheckOBBtoObb(const P_BOX& box0, const P_BOX& box1)
{
	D3DXVECTOR3 distance_center(box1.center[0] - box0.center[0],
		box1.center[1] - box0.center[1], box1.center[2] - box0.center[2]);

	//A = box0  , B = box1
	//
	float obb_projection[3][3];
	float obb_projection_abs[3][3];
	float box0_axis_boxcenter_distance_dot[3];
	float box1_axis_boxcenter_distance_dot[3];
	float p1, p2, D;

	//Tbasis3d 2권 p.380
	//w1  (w = 분리축) OBB1.x
	const D3DXVECTOR3* Ax = &box0.obb_axis[0];
	obb_projection[0][0] = D3DXVec3Dot(Ax, &box1.obb_axis[0]);
	obb_projection[0][1] = D3DXVec3Dot(Ax, &box1.obb_axis[1]);
	obb_projection[0][2] = D3DXVec3Dot(Ax, &box1.obb_axis[2]);
	
	box0_axis_boxcenter_distance_dot[0] = D3DXVec3Dot(Ax, &distance_center);
	obb_projection_abs[0][0] = std::fabsf(obb_projection[0][0]);
	obb_projection_abs[0][1] = std::fabsf(obb_projection[0][1]);
	obb_projection_abs[0][2] = std::fabsf(obb_projection[0][2]);

	D = std::fabsf(box0_axis_boxcenter_distance_dot[0]);
	p1 = box1.obb_extents[0] * obb_projection_abs[0][0] + 
		box1.obb_extents[1] * obb_projection_abs[0][1] + box1.obb_extents[2] * obb_projection_abs[0][2];
	p2 = box0.obb_extents[0];

	if (D > (p1 + p2)) return false;

	//w2 OBB1.y
	const D3DXVECTOR3* Ay = &box0.obb_axis[1];
	obb_projection[1][0] = D3DXVec3Dot(Ay, &box1.obb_axis[0]);
	obb_projection[1][1] = D3DXVec3Dot(Ay, &box1.obb_axis[1]);
	obb_projection[1][2] = D3DXVec3Dot(Ay, &box1.obb_axis[2]);

	box0_axis_boxcenter_distance_dot[1] = D3DXVec3Dot(Ay, &distance_center);
	obb_projection_abs[1][0] = std::fabsf(obb_projection[1][0]);
	obb_projection_abs[1][1] = std::fabsf(obb_projection[1][1]);
	obb_projection_abs[1][2] = std::fabsf(obb_projection[1][2]);

	D = std::fabsf(box0_axis_boxcenter_distance_dot[1]);
	p1 = box1.obb_extents[0] * obb_projection_abs[1][0] +
		box1.obb_extents[1] * obb_projection_abs[1][1] + 
		box1.obb_extents[2] * obb_projection_abs[1][2];
	p2 = box0.obb_extents[1];

	if (D > (p1 + p2)) return false;

	//w3 OBB1.z
	const D3DXVECTOR3* Az = &box0.obb_axis[2];
	obb_projection[2][0] = D3DXVec3Dot(Az, &box1.obb_axis[0]);
	obb_projection[2][1] = D3DXVec3Dot(Az, &box1.obb_axis[1]);
	obb_projection[2][2] = D3DXVec3Dot(Az, &box1.obb_axis[2]);

	box0_axis_boxcenter_distance_dot[2] = D3DXVec3Dot(Az, &distance_center);
	obb_projection_abs[2][0] = std::fabsf(obb_projection[2][0]);
	obb_projection_abs[2][1] = std::fabsf(obb_projection[2][1]);
	obb_projection_abs[2][2] = std::fabsf(obb_projection[2][2]);

	D = std::fabsf(box0_axis_boxcenter_distance_dot[2]);
	p1 = box1.obb_extents[0] * obb_projection_abs[2][0] +
		box1.obb_extents[1] * obb_projection_abs[2][1] +
		box1.obb_extents[2] * obb_projection_abs[2][2];
	p2 = box0.obb_extents[2];

	if (D > (p1 + p2)) return false;
	
	//w4  OBB2.x
	const D3DXVECTOR3* Bx = &box1.obb_axis[0]; //box1의 x axis를 사용
	box1_axis_boxcenter_distance_dot[0] = D3DXVec3Dot(Bx, &distance_center);
	D = std::fabsf(box1_axis_boxcenter_distance_dot[0]);

	p1 = box0.obb_extents[0] * obb_projection_abs[0][0] +
		box0.obb_extents[1] * obb_projection_abs[1][0] +
		box0.obb_extents[2] * obb_projection_abs[2][0];
	p2 = box1.obb_extents[0];

	if (D > (p1 + p2)) return false;

	//w5  OBB2.y
	const D3DXVECTOR3* By = &box1.obb_axis[1];
	box1_axis_boxcenter_distance_dot[1] = D3DXVec3Dot(By, &distance_center);
	D = std::fabsf(box1_axis_boxcenter_distance_dot[1]);

	p1 = box0.obb_extents[0] * obb_projection_abs[0][1] +
		box0.obb_extents[1] * obb_projection_abs[1][1] +
		box0.obb_extents[2] * obb_projection_abs[2][1];
	p2 = box1.obb_extents[1];

	if (D > (p1 + p2)) return false;

	//w6  OBB2.z
	const D3DXVECTOR3* Bz = &box1.obb_axis[2];
	box1_axis_boxcenter_distance_dot[2] = D3DXVec3Dot(Bz, &distance_center);
	D = std::fabsf(box1_axis_boxcenter_distance_dot[2]);

	p1 = box0.obb_extents[0] * obb_projection_abs[0][2] +
		box0.obb_extents[1] * obb_projection_abs[1][2] +
		box0.obb_extents[2] * obb_projection_abs[2][2];
	p2 = box1.obb_extents[2];

	if (D > (p1 + p2)) return false;

	//w7 = A0 X B0    X = cross product   D : 분리축(separate plane)에 내적된 각 OBB의 센터끼리의 길이
	D = std::fabs(box0_axis_boxcenter_distance_dot[2] * obb_projection[1][0] - box0_axis_boxcenter_distance_dot[1] * obb_projection[2][0]);

	p1 = box0.obb_extents[1] * obb_projection_abs[2][0] + box0.obb_extents[2] * obb_projection_abs[1][0];
	p2 = box1.obb_extents[1] * obb_projection_abs[0][2] + box1.obb_extents[2] * obb_projection_abs[0][1];
	
	if (D > (p1 + p2)) return false;

	//w8 = A0 X B1    X = cross product   D : 분리축(separate plane)에 내적된 각 OBB의 센터끼리의 길이
	D = std::fabs(box0_axis_boxcenter_distance_dot[2] * obb_projection[1][1] - box0_axis_boxcenter_distance_dot[1] * obb_projection[2][1]);

	p1 = box0.obb_extents[1] * obb_projection_abs[2][1] + box0.obb_extents[2] * obb_projection_abs[1][1];
	p2 = box1.obb_extents[0] * obb_projection_abs[0][2] + box1.obb_extents[2] * obb_projection_abs[0][0];

	if (D > (p1 + p2)) return false;
	
	//w9 = A0 X B2    X = cross product   D : 분리축(separate plane)에 내적된 각 OBB의 센터끼리의 길이
	D = std::fabs(box0_axis_boxcenter_distance_dot[2] * obb_projection[1][2] - box0_axis_boxcenter_distance_dot[1] * obb_projection[2][2]);

	p1 = box0.obb_extents[1] * obb_projection_abs[2][2] + box0.obb_extents[2] * obb_projection_abs[1][2];
	p2 = box1.obb_extents[0] * obb_projection_abs[0][1] + box1.obb_extents[1] * obb_projection_abs[0][0];

	if (D > (p1 + p2)) return false;

	//w10 = A1 X B0    X = cross product   D : 분리축(separate plane)에 내적된 각 OBB의 센터끼리의 길이
	D = std::fabs(box0_axis_boxcenter_distance_dot[0] * obb_projection[2][0] - box0_axis_boxcenter_distance_dot[2] * obb_projection[0][0]);

	p1 = box0.obb_extents[0] * obb_projection_abs[2][0] + box0.obb_extents[2] * obb_projection_abs[0][0];
	p2 = box1.obb_extents[1] * obb_projection_abs[1][2] + box1.obb_extents[2] * obb_projection_abs[1][1];

	if (D > (p1 + p2)) return false;


	//w11 = A1 X B1    X = cross product   D : 분리축(separate plane)에 내적된 각 OBB의 센터끼리의 길이
	D = std::fabs(box0_axis_boxcenter_distance_dot[0] * obb_projection[2][1] - box0_axis_boxcenter_distance_dot[2] * obb_projection[0][1]);

	p1 = box0.obb_extents[0] * obb_projection_abs[2][1] + box0.obb_extents[2] * obb_projection_abs[0][1];
	p2 = box1.obb_extents[0] * obb_projection_abs[1][2] + box1.obb_extents[2] * obb_projection_abs[1][0];

	if (D > (p1 + p2)) return false;


	//w12 = A1 X B2    X = cross product   D : 분리축(separate plane)에 내적된 각 OBB의 센터끼리의 길이
	D = std::fabs(box0_axis_boxcenter_distance_dot[0] * obb_projection[2][2] - box0_axis_boxcenter_distance_dot[2] * obb_projection[0][2]);

	p1 = box0.obb_extents[0] * obb_projection_abs[2][2] + box0.obb_extents[2] * obb_projection_abs[0][2];
	p2 = box1.obb_extents[0] * obb_projection_abs[1][1] + box1.obb_extents[1] * obb_projection_abs[1][0];

	if (D > (p1 + p2)) return false;

	//w13 = A2 X B0    X = cross product   D : 분리축(separate plane)에 내적된 각 OBB의 센터끼리의 길이
	D = std::fabs(box0_axis_boxcenter_distance_dot[1] * obb_projection[0][0] - box0_axis_boxcenter_distance_dot[0] * obb_projection[1][0]);

	p1 = box0.obb_extents[0] * obb_projection_abs[1][0] + box0.obb_extents[1] * obb_projection_abs[0][0];
	p2 = box1.obb_extents[1] * obb_projection_abs[2][2] + box1.obb_extents[2] * obb_projection_abs[2][1];

	if (D > (p1 + p2)) return false;

	//w14 = A2 X B1    X = cross product   D : 분리축(separate plane)에 내적된 각 OBB의 센터끼리의 길이
	D = std::fabs(box0_axis_boxcenter_distance_dot[1] * obb_projection[0][1] - box0_axis_boxcenter_distance_dot[0] * obb_projection[1][1]);

	p1 = box0.obb_extents[0] * obb_projection_abs[1][1] + box0.obb_extents[1] * obb_projection_abs[0][1];
	p2 = box1.obb_extents[0] * obb_projection_abs[2][2] + box1.obb_extents[2] * obb_projection_abs[2][0];

	if (D > (p1 + p2)) return false;

	//w15 = A2 X B2    X = cross product   D : 분리축(separate plane)에 내적된 각 OBB의 센터끼리의 길이
	D = std::fabs(box0_axis_boxcenter_distance_dot[1] * obb_projection[0][2] - box0_axis_boxcenter_distance_dot[0] * obb_projection[1][2]);

	p1 = box0.obb_extents[0] * obb_projection_abs[1][2] + box0.obb_extents[1] * obb_projection_abs[0][2];
	p2 = box1.obb_extents[0] * obb_projection_abs[2][1] + box1.obb_extents[1] * obb_projection_abs[2][0];

	if (D > (p1 + p2)) return false;



	return true;
}

bool PCollision::Init()
{


	return true;
}

bool PCollision::Frame()
{
	return false;
}

bool PCollision::Render()
{
	return false;
}

bool PCollision::Release()
{
	return false;
}
