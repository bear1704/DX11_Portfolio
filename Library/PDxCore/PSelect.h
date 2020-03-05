#pragma once
#include "PShape.h"

struct PickingRay
{
	float extent;
	D3DXVECTOR3 origin;
	D3DXVECTOR3 direction;

	PickingRay()
	{
		extent = -1;
	}

};


class PSelect
{
public:
	PSelect();
	~PSelect();

public:
	PickingRay picking_ray_;
	D3DXMATRIX mat_world_;
	D3DXMATRIX mat_view_;
	D3DXMATRIX mat_proj_;
	D3DXVECTOR3 intersection_; //레이의 원점 -> 교점까지의 벡터(정규화안됨)

	float t_min;
	float t_max;

public:
	//반직선과 평면의 교차여부를 판정하고 반직선의 교점 좌표값을 얻는다
	bool GetIntersection(OUT_ D3DXVECTOR3& intersection, 
		const D3DXVECTOR3& start, 
		const D3DXVECTOR3& end, 
		const D3DXVECTOR3& normal,
		const D3DXVECTOR3& v0);

	/*폴리곤(Triangle)과 반직선,선분의 교차를 판정한다.
	point : 검사할 교점    face_normal : 폴리곤 삼각형의 노말벡터
	*/
	bool IsPointInTriPolygon(D3DXVECTOR3& point, 
		D3DXVECTOR3& face_normal,
		D3DXVECTOR3& v0,
		D3DXVECTOR3& v1, 
		D3DXVECTOR3& v2);

	void SetMatrix(D3DXMATRIX* world, D3DXMATRIX* view, D3DXMATRIX* proj);
	void RefreshPickingRay();
	//tbasis2 p.374
	bool CheckRaytoOBBCollision(P_BOX* box, PickingRay* ray = nullptr);

};

