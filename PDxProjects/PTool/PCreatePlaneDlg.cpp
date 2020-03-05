// PCreatePlaneDlg.cpp: 구현 파일
//

#include "pch.h"
#include "PTool.h"
#include "PCreatePlaneDlg.h"
#include "afxdialogex.h"


// PCreatePlaneDlg 대화 상자

IMPLEMENT_DYNAMIC(PCreatePlaneDlg, CDialogEx)

PCreatePlaneDlg::PCreatePlaneDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CreatePlane, pParent)
	, m_PlaneHeight(0)
	, m_FrameInterval(0)
	, m_LifeTime(0)
	, m_SpriteXInit(0)
	, m_SpriteYInit(0)
	, m_XOffset(0)
	, m_YOffset(0)
	, m_PlaneWidth(0)
	, m_PlaneName(_T(""))
	, m_CurrentPlaneIsLoop(false)
	, m_XCount(0)
	, m_YCount(0)
	, m_TextureWidth(0)
	, m_TextureHeight(0)
	, m_WorldTx(0)
	, m_WorldTy(0)
	, m_WorldTz(0)
	, m_WorldRx(0)
	, m_WorldRy(0)
	, m_WorldRz(0)
	, m_WorldSx(0)
	, m_WorldSy(0)
	, m_WorldSz(0)
	, m_IsMultiTexture(FALSE)
{
	app = (CPToolApp*)AfxGetApp();

	
}

PCreatePlaneDlg::~PCreatePlaneDlg()
{
}

void PCreatePlaneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_PlaneHeight);
	DDV_MinMaxFloat(pDX, m_PlaneHeight, 0.01, FLT_MAX);
	DDX_Text(pDX, IDC_EDIT_FrameInterval, m_FrameInterval);
	DDV_MinMaxFloat(pDX, m_FrameInterval, 0.01, FLT_MAX);
	DDX_Text(pDX, IDC_EDIT_LifeTime, m_LifeTime);
	DDX_Text(pDX, IDC_EDIT_XInit, m_SpriteXInit);
	DDX_Text(pDX, IDC_EDIT_YInit, m_SpriteYInit);
	DDX_Text(pDX, IDC_EDIT_XOffset, m_XOffset);
	DDX_Text(pDX, IDC_EDIT_YOffset, m_YOffset);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_PlaneWidth);
	DDX_Text(pDX, IDC_EDIT_PlaneName, m_PlaneName);
	DDV_MaxChars(pDX, m_PlaneName, 30);
	DDX_Check(pDX, IDC_CHECK_IsLoop, m_CurrentPlaneIsLoop);
	DDX_Text(pDX, IDC_EDIT1_XCount, m_XCount);
	DDX_Text(pDX, IDC_EDIT_YCount, m_YCount);
	DDX_Text(pDX, IDC_EDIT_TextureWidth, m_TextureWidth);
	DDX_Text(pDX, IDC_EDIT_TextureHeight, m_TextureHeight);
	DDX_Text(pDX, IDC_EDIT_Tx, m_WorldTx);
	DDX_Text(pDX, IDC_EDIT_Ty, m_WorldTy);
	DDX_Text(pDX, IDC_EDIT_Tz, m_WorldTz);
	DDX_Text(pDX, IDC_EDIT_Rx, m_WorldRx);
	DDX_Text(pDX, IDC_EDIT_Ry, m_WorldRy);
	DDX_Text(pDX, IDC_EDIT_Rz, m_WorldRz);
	DDX_Text(pDX, IDC_EDIT_Sx, m_WorldSx);
	DDX_Text(pDX, IDC_EDIT_Sy, m_WorldSy);
	DDX_Text(pDX, IDC_EDIT_Sz, m_WorldSz);
	DDX_Check(pDX, IDC_CHECK_IsMultiTexture, m_IsMultiTexture);
}


BEGIN_MESSAGE_MAP(PCreatePlaneDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &PCreatePlaneDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_Btn_SelectTexture, &PCreatePlaneDlg::OnBtnClickedSelectTexture)
	ON_BN_CLICKED(IDC_Btn_SelectScript, &PCreatePlaneDlg::OnBtnClickedSelectScript)
	ON_BN_CLICKED(IDCANCEL, &PCreatePlaneDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_Btn_Initialize, &PCreatePlaneDlg::OnBnClickedBtnInitialize)
	ON_BN_CLICKED(IDC_CHECK_IsBlended, &PCreatePlaneDlg::OnBnClickedCheckIsblended)
END_MESSAGE_MAP()


// PCreatePlaneDlg 메시지 처리기



void PCreatePlaneDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
	UpdateData(TRUE);
	std::wstring plane_and_sprite_name = m_PlaneName;
	std::string sprite_name_str;
	WstringToString(plane_and_sprite_name, sprite_name_str);
	D3DXMATRIX mat_rot;
	D3DXMATRIX mat_scale;
	D3DXMATRIX mat_world;
	D3DXMatrixIdentity(&mat_rot);
	D3DXMatrixIdentity(&mat_scale);
	D3DXMatrixIdentity(&mat_world);
	D3DXMatrixRotationYawPitchRoll(&mat_rot, m_WorldRy, m_WorldRx, m_WorldRz);
	D3DXMatrixScaling(&mat_scale, m_WorldSx, m_WorldSy, m_WorldSz);

	mat_world = mat_scale * mat_rot;
	mat_world._41 = m_WorldTx;
	mat_world._42 = m_WorldTy;
	mat_world._43 = m_WorldTz;


	// > 1  이면 다중텍스쳐 스프라이트
	if (m_IsMultiTexture == TRUE)
	{
		SpriteDataInfo sp_info;
		EffectInfo eff_info;
		sp_info.max_frame = tex_list.size();
		sp_info.once_playtime = m_FrameInterval * tex_list.size();
		sp_info.posX = 100.0f;
		sp_info.posY = 100.0f;
		sp_info.sprite_name = sprite_name_str;
		sp_info.texture_list = tex_list;
		std::wstring plane_and_sprite_name = m_PlaneName;
		sp_info.texture_name = plane_and_sprite_name;

		if (m_CurrentPlaneIsLoop == TRUE)
			sp_info.is_loop = true;
		else
			sp_info.is_loop = false;

		sp_info.lifetime = m_LifeTime;

		eff_info.is_multi_texture = true;
		sp_info.sprite_name.assign(plane_and_sprite_name.begin(), plane_and_sprite_name.end());

		std::wstring wstr_name;
		StringToWstring(sp_info.sprite_name, wstr_name);
		PSpriteManager::GetInstance().LoadSpriteDataWithoutScript(wstr_name, tex_list, sp_info);

		PEffectObject* pp = new PEffectObject();
		pp->matWorld_ = mat_world;
		pp->CreateEffect(app->m_tool.device(), app->m_tool.immediate_device_context(), m_PlaneWidth, m_PlaneHeight, plane_and_sprite_name, eff_info);
		pp->width_ = m_PlaneWidth;
		pp->height_ = m_PlaneHeight;
		pp->plane_rot_matrix_ = mat_rot;
		pp->vec_rotation_for_object_.x = m_WorldRx;
		pp->vec_rotation_for_object_.y = m_WorldRy;
		pp->vec_rotation_for_object_.z = m_WorldRz;
		app->m_tool.effect_plane_.eff_list_.push_back(pp);
		tex_list.clear();
	}
	else
	{
		SpriteDataInfo sp_info;
		EffectInfo eff_info;
		sp_info.max_frame = m_XCount * m_YCount;
		sp_info.once_playtime = m_FrameInterval * sp_info.max_frame;
		sp_info.posX = 100.0f;
		sp_info.posY = 100.0f;
		sp_info.sprite_name = sprite_name_str;
		sp_info.texture_name = tex_list[0]->tex_name();


		if (m_CurrentPlaneIsLoop == TRUE)
			sp_info.is_loop = true;
		else
			sp_info.is_loop = false;

		sp_info.lifetime = m_LifeTime;
		
		float xOrigin = m_SpriteXInit;
		float yOrigin = m_SpriteYInit;

		for (int ii = 0; ii < m_YCount; ii++)
		{
			for (int jj = 0; jj < m_XCount; jj++)
			{
				float current_xOrigin = xOrigin + m_XOffset * jj;
				float current_yOrigin = yOrigin + m_YOffset * ii;

				DX::PTex_uv4 uv;
				uv.u[0] = current_xOrigin / m_TextureWidth;
				uv.u[1] = (current_xOrigin + m_XOffset) / m_TextureWidth;
				uv.u[2] = (current_xOrigin + m_XOffset) / m_TextureWidth;
				uv.u[3] = current_xOrigin / m_TextureWidth;
				uv.v[0] = current_yOrigin / m_TextureHeight;
				uv.v[1] = current_yOrigin / m_TextureHeight;
				uv.v[2] = (current_yOrigin + m_YOffset) / m_TextureHeight;
				uv.v[3] = (current_yOrigin + m_YOffset) / m_TextureHeight;

				sp_info.tex_boundary_list.push_back(uv);
			}
		}
		
		eff_info.is_multi_texture = false;
		sp_info.sprite_name.assign(plane_and_sprite_name.begin(), plane_and_sprite_name.end());

		eff_info.x_count = m_XCount;
		eff_info.y_count = m_YCount;
		eff_info.x_init = m_SpriteXInit;
		eff_info.y_init = m_SpriteYInit;
		eff_info.x_offset = m_XOffset;
		eff_info.y_offset = m_YOffset;
		eff_info.tex_width = m_TextureWidth;
		eff_info.tex_height = m_TextureHeight;

		std::wstring wstr_name;
		StringToWstring(sp_info.sprite_name, wstr_name);
		PSpriteManager::GetInstance().LoadSpriteDataWithoutScript(wstr_name, tex_list, sp_info);


		PEffectObject* pp = new PEffectObject();
		pp->matWorld_ = mat_world;
		pp->CreateEffect(app->m_tool.device(), app->m_tool.immediate_device_context(), m_PlaneWidth, m_PlaneHeight, plane_and_sprite_name, eff_info);
		pp->width_ = m_PlaneWidth;
		pp->height_ = m_PlaneHeight;
		pp->plane_rot_matrix_ = mat_rot;
		pp->vec_rotation_for_object_.x = m_WorldRx;
		pp->vec_rotation_for_object_.y = m_WorldRy;
		pp->vec_rotation_for_object_.z = m_WorldRz;
		app->m_tool.effect_plane_.eff_list_.push_back(pp);
		tex_list.clear();
	}

	UpdateData(TRUE);
}


void PCreatePlaneDlg::OnBtnClickedSelectTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString selFileName;
	CFileDialog dlg(
		TRUE, L"bmp", NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
		L"img Files(*.이믜지)|*.png;*.dds;*.bmp;*.jpg;*.jpeg|All Files(*.*)|*.*||", this
	);
	CString file_path;
	char str_file[4096];
	TCHAR path[MAX_PATH];
	dlg.m_ofn.lpstrFile = file_path.GetBuffer(4096);
	dlg.m_ofn.nMaxFile = sizeof(str_file);
	GetCurrentDirectory(MAX_PATH, path);
	StrCat(path, L"\\Effect");

	dlg.m_ofn.lpstrInitialDir = path;


	if (dlg.DoModal() == IDOK)
	{
		UpdateData(TRUE);
		
		for (POSITION ii = dlg.GetStartPosition(); ii != NULL;)
		{
			CString path = dlg.GetNextPathName(ii);
			int a = path.Find(L"Effect");
			CString relativepath = path.Mid(a, path.GetLength() - a);
			TextureInfo info;
			//info.tex_path = path;
			info.tex_path = relativepath;
			//주의 : 땜빵

			PTexture* tex = PTextureManager::GetInstance().LoadTextureWithoutScript(info, app->m_tool.device());
			if (tex == nullptr)
			{
				TCHAR Drive[MAX_PATH] = { 0, };
				TCHAR Dir[MAX_PATH] = { 0, };
				TCHAR Name[MAX_PATH] = { 0, };
				TCHAR Ext[MAX_PATH] = { 0, };

				_tsplitpath_s(info.tex_path.c_str(), Drive, Dir, Name, Ext);
				tex = PTextureManager::GetInstance().GetTextureFromMap(Name);
			}

			tex_list.push_back(tex);
		}
	}

}


void PCreatePlaneDlg::OnBtnClickedSelectScript()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

}


void PCreatePlaneDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}




void PCreatePlaneDlg::OnBnClickedBtnInitialize()
{
	m_WorldTx = 0.0f;
	m_WorldTy = 0.0f;
	m_WorldTz = 0.0f;
	m_WorldRx = 0.0f;
	m_WorldRy = 0.0f;
	m_WorldRz = 0.0f; 
	m_WorldSx = 1.0f;
	m_WorldSy = 1.0f;
	m_WorldSz = 1.0f;
	m_PlaneHeight = 10.0f;
	m_PlaneWidth = 10.0f;
	m_FrameInterval = 0.1f;
	m_LifeTime = 3.0f;
	UpdateData(FALSE);
}


void PCreatePlaneDlg::OnBnClickedCheckIsblended()
{
	DX::ApplyBlendState(app->m_tool.immediate_device_context(), DX::PDxState::blend_state_alphablend_);
}
