#include "PFrustum.h"

bool P_PLANE::CreatePlane(D3DXVECTOR3 v0, D3DXVECTOR3 v1, D3DXVECTOR3 v2)
{
	D3DXVECTOR3 edge0 = v1 - v0;
	D3DXVECTOR3 edge1 = v2 - v0;
	D3DXVECTOR3 normal;
	D3DXVec3Cross(&normal, &edge0, &edge1); 
	D3DXVec3Normalize(&normal, &normal);
	a = normal.x;
	b = normal.y;
	c = normal.z;
	//v0을 평면의 방정식에 대입하여, ax+by+cz+d = 0에서 d를 제외한 나머지를 이항하여 d를 도출.
	d = -(a*v0.x + b*v0.y + c*v0.z);
	
	return true;
}

bool P_PLANE::CreatePlane(D3DXVECTOR3 normal, D3DXVECTOR3 v1)
{
	D3DXVec3Normalize(&normal, &normal);
	a = normal.x;
	b = normal.y;
	c = normal.z;
	d = -(a * v1.x + b * v1.y + c * v1.z);
	return true;
}

void P_PLANE::Normalize()
{
	float magnitude = std::sqrtf(a * a + b * b + c * c);
	a = a / magnitude;
	b = b / magnitude;
	c = c / magnitude;
	d = d / magnitude;
}

void PFrustum::CreateFrustum(D3DXMATRIX& mat_view, D3DXMATRIX& mat_proj)
{
	mat_view_ = mat_view;
	mat_proj_ = mat_proj;
	D3DXMATRIX mat_inv_vp = mat_view * mat_proj;
	D3DXMatrixInverse(&mat_inv_vp, NULL, &mat_inv_vp);
	//5 6
	//4 7  far

	//1 2
	//0 3  near

	frustum_vertex_[0] = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	frustum_vertex_[1] = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	frustum_vertex_[2] = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	frustum_vertex_[3] = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	
	frustum_vertex_[4] = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	frustum_vertex_[5] = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	frustum_vertex_[6] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	frustum_vertex_[7] = D3DXVECTOR3(1.0f, -1.0f, 1.0f);

	//모두 월드 좌표로 
	for (int i = 0; i < kNumberofFrustumVertex; i++)
	{
		D3DXVec3TransformCoord(
			&frustum_vertex_[i],
			&frustum_vertex_[i],
			&mat_inv_vp);
	}

	frustum_plane_[0].CreatePlane(frustum_vertex_[5], frustum_vertex_[0], frustum_vertex_[1]); //좌평면
	frustum_plane_[1].CreatePlane(frustum_vertex_[3], frustum_vertex_[6], frustum_vertex_[2]); //우평면
	frustum_plane_[2].CreatePlane(frustum_vertex_[5], frustum_vertex_[2], frustum_vertex_[6]); //상평면
	frustum_plane_[3].CreatePlane(frustum_vertex_[0], frustum_vertex_[7], frustum_vertex_[3]); //하평면
	frustum_plane_[4].CreatePlane(frustum_vertex_[0], frustum_vertex_[2], frustum_vertex_[1]); //근평면
	frustum_plane_[5].CreatePlane(frustum_vertex_[6], frustum_vertex_[4], frustum_vertex_[5]); //원평면

}


bool PFrustum::CheckCollisionOBB(P_BOX& box)
{
	float dist_plane_to_center = 0;
	float dist_obb_inner_products = 0;
	D3DXVECTOR3 axis_dir;
	D3DXVECTOR3 normal;

	for (int i = 0; i < kNumberofFrustumPlane; i++)
	{
		//obb x축 내적합
		normal = D3DXVECTOR3(frustum_plane_[i].a, frustum_plane_[i].b, frustum_plane_[i].c);
		
		axis_dir = box.obb_axis[0] * box.obb_extents[0];
		dist_obb_inner_products = std::fabs(D3DXVec3Dot(&normal, &axis_dir));

		//obb y축 내적합
		axis_dir = box.obb_axis[1] * box.obb_extents[1];
		dist_obb_inner_products += std::fabs(D3DXVec3Dot(&normal, &axis_dir));

		//obb z축 내적합
		axis_dir = box.obb_axis[2] * box.obb_extents[2];
		dist_obb_inner_products += std::fabs(D3DXVec3Dot(&normal, &axis_dir));

		//obb의 평면노말에 대한 내적합들과 평면-원점 사이 거리 비교 
		//평면방정식에 박스의 center정점을 대입하여 거리 도출
		dist_plane_to_center =
			frustum_plane_[i].a * box.center.x +
			frustum_plane_[i].b * box.center.y +
			frustum_plane_[i].c * box.center.z +
			frustum_plane_[i].d;

		//OBB 중점이 Plane 외부에 존재할 경우, D값은 음수가 되기 때문에 d도 음수로 비교해야 한다.
		if (dist_plane_to_center <= -dist_obb_inner_products) //제외
			return false;
	}
	return true;
}

RELATIVE_POSITION PFrustum::CheckOBBRelativePos(P_BOX& box)
{
	float dist_plane_to_center = 0;
	float dist_obb_inner_products = 0;
	D3DXVECTOR3 axis_dir;
	D3DXVECTOR3 normal;
	RELATIVE_POSITION r_pos = RELATIVE_POSITION::FRONT;

	for (int i = 0; i < kNumberofFrustumPlane; i++)
	{
		//obb x축 내적합
		normal = D3DXVECTOR3(frustum_plane_[i].a, frustum_plane_[i].b, frustum_plane_[i].c);

		axis_dir = box.obb_axis[0] * box.obb_extents[0];
		dist_obb_inner_products = std::fabs(D3DXVec3Dot(&normal, &axis_dir));

		//obb y축 내적합
		axis_dir = box.obb_axis[1] * box.obb_extents[1];
		dist_obb_inner_products += std::fabs(D3DXVec3Dot(&normal, &axis_dir));

		//obb z축 내적합
		axis_dir = box.obb_axis[2] * box.obb_extents[2];
		dist_obb_inner_products += std::fabs(D3DXVec3Dot(&normal, &axis_dir));

		//obb의 평면노말에 대한 내적합들과 평면-원점 사이 거리 비교 
		//평면방정식에 박스의 center정점을 대입하여 거리 도출
		dist_plane_to_center =
			frustum_plane_[i].a * box.center.x +
			frustum_plane_[i].b * box.center.y +
			frustum_plane_[i].c * box.center.z +
			frustum_plane_[i].d;

		//BACK이어도 일단 SPANNING으로 들어간다. 왜냐면 BACK위치에서 PlaneToCenter는 음수이기 때문에.
		if (dist_plane_to_center <= dist_obb_inner_products)
			r_pos = RELATIVE_POSITION::SPANNING;
		//제외, PlaneToCenter(센터와 평면간의 거리)가 BACK위치면 음수이기 때문에, 내적값에도 마이너스를 붙여서 계산해 주어야 한다.
		if (dist_plane_to_center < -dist_obb_inner_products) 
			return RELATIVE_POSITION::BACK;
	}

	return r_pos;	
}

bool PFrustum::PreRender(ID3D11DeviceContext* context, int stride_length)
{
	//0, 1, 2, 3순으로 하면 문제있나?
	//near plane
	frustum_box_obj_.vertex_list_[0] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[1].x, frustum_vertex_[1].y, frustum_vertex_[1].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[1] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[2].x, frustum_vertex_[2].y, frustum_vertex_[2].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[2] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[3].x, frustum_vertex_[3].y, frustum_vertex_[3].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[3] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[0].x, frustum_vertex_[0].y, frustum_vertex_[0].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));

	//far
	frustum_box_obj_.vertex_list_[4] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[6].x, frustum_vertex_[6].y, frustum_vertex_[6].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[5] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[5].x, frustum_vertex_[5].y, frustum_vertex_[5].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[6] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[4].x, frustum_vertex_[4].y, frustum_vertex_[4].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[7] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[7].x, frustum_vertex_[7].y, frustum_vertex_[7].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));

	//right
	frustum_box_obj_.vertex_list_[8] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[5].x, frustum_vertex_[5].y, frustum_vertex_[5].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[9] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[1].x, frustum_vertex_[1].y, frustum_vertex_[1].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[10] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[0].x, frustum_vertex_[0].y, frustum_vertex_[0].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[11] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[4].x, frustum_vertex_[4].y, frustum_vertex_[4].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));

	//left
	frustum_box_obj_.vertex_list_[12] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[2].x, frustum_vertex_[2].y, frustum_vertex_[2].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[13] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[6].x, frustum_vertex_[6].y, frustum_vertex_[6].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[14] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[7].x, frustum_vertex_[7].y, frustum_vertex_[7].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[15] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[3].x, frustum_vertex_[3].y, frustum_vertex_[3].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	
	//top
	frustum_box_obj_.vertex_list_[16] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[5].x, frustum_vertex_[5].y, frustum_vertex_[5].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[17] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[6].x, frustum_vertex_[6].y, frustum_vertex_[6].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[18] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[2].x, frustum_vertex_[2].y, frustum_vertex_[2].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[19] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[1].x, frustum_vertex_[1].y, frustum_vertex_[1].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));

	//bottom
	frustum_box_obj_.vertex_list_[20] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[0].x, frustum_vertex_[0].y, frustum_vertex_[0].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[21] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[3].x, frustum_vertex_[3].y, frustum_vertex_[3].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[22] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[7].x, frustum_vertex_[7].y, frustum_vertex_[7].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	frustum_box_obj_.vertex_list_[23] =
		Vertex_PNCT(D3DXVECTOR3(frustum_vertex_[4].x, frustum_vertex_[4].y, frustum_vertex_[4].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	
	context->UpdateSubresource(frustum_box_obj_.dx_helper_.vertex_buffer_.Get(), 0, NULL, &frustum_box_obj_.vertex_list_.at(0), 0, 0);
	frustum_box_obj_.dx_helper_.PreRender(context, stride_length);

	return true;
}

bool PFrustum::PostRender(ID3D11DeviceContext* context)
{
	frustum_box_obj_.dx_helper_.PostRender(context, frustum_box_obj_.dx_helper_.index_count_);
	return true;
}

bool PFrustum::Render(ID3D11DeviceContext* context)
{
	context->UpdateSubresource(frustum_box_obj_.dx_helper_.constant_buffer_.Get(), 0, NULL, &frustum_box_obj_.constant_data_, 0, 0);
	PreRender(context, frustum_box_obj_.dx_helper_.vertex_size_);
	context->PSSetShader(pixel_shader_.Get(), NULL, 0);
	PostRender(context);
	return true;
}
