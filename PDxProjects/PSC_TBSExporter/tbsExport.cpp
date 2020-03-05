#include "pch.h"
#include "PSCWriter.h"
#include "PSkinExp.h"
#include "PMatrixExp.h"
#include "resource.h"


#define PSCExport_CLASS_ID Class_ID(0x2d8540f6, 0x48ab7160)

extern HINSTANCE handle_instance;

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class tbsExport: public UtilityObj
{
private:
	tbsExport() 
	{
		handle_panel_ = NULL;
	};
public:
	~tbsExport() {};
public:
	HWND handle_panel_;
	Interface* interface_;
public:
	virtual void BeginEditParams(Interface* ip, IUtil* iu) override
	{
		interface_ = ip;
		handle_panel_ = ip->AddRollupPage(handle_instance, MAKEINTRESOURCE(IDD_DIALOG), DlgProc, _T("tbsExp"), 0);
	}

	virtual void EndEditParams(Interface* ip, IUtil* iu) override
	{
		ip->DeleteRollupPage(handle_panel_);
	}
	virtual void DeleteThis() override
	{

	}
	virtual void SelectionSetChanged(Interface* ip, IUtil* iu) override
	{
		PMatrixExp::GetInstance().Set(NULL, ip);

		PSkinExp::GetInstance().object_list_.clear();
		PSkinExp::GetInstance().material_list_.clear();
		PSkinExp::GetInstance().pmtl_list_.clear();

		int numberof_select = ip->GetSelNodeCount();
		if (numberof_select <= 0)
			return;

		for (int i = 0; i < numberof_select; i++)
		{
			INode* node = ip->GetSelNode(i);
			PSkinExp::GetInstance().object_list_.push_back(node);
			PSkinExp::GetInstance().AddMaterial(node);
		}
	}


public:
	static tbsExport* GetInstance()
	{
		static tbsExport exp;
		return &exp;
	}

};


class tbsExportClassDesc : public ClassDesc2
{
public:
	virtual int IsPublic() { return TRUE; };
	virtual void* Create(BOOL)
	{
		return tbsExport::GetInstance();
	}
	virtual const TCHAR* ClassName() { return _T("PSCExporter v4"); }
	virtual SClass_ID SuperClassID()
	{
		return UTILITY_CLASS_ID;
	}
	virtual Class_ID ClassID()
	{
		return PSCExport_CLASS_ID;
	}
	virtual const TCHAR* Category() { return _T("PSCExporter"); }
	virtual const TCHAR* InternalName() { return _T("tbsExportClassDesc"); }
	virtual HINSTANCE HandleInstance() { return handle_instance; }
};

ClassDesc2* GetExportDesc()
{
	static tbsExportClassDesc PSCExportDesc;
	return &PSCExportDesc;

}

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		break;
	case WM_DESTROY:
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_SKINEXP:
		{
			TSTR exp_file = PSkinExp::GetInstance().SaveFileDialog(L"skm", L"SkinWriter");
			if (exp_file != NULL)
			{
				PSkinExp::GetInstance().Set(exp_file, tbsExport::GetInstance()->interface_);
				PSkinExp::GetInstance().Export();
			}

		}break;
		case IDC_MATRIXEXP:
		{
			TSTR exp_file = PMatrixExp::GetInstance().SaveFileDialog(L"anim", L"MatrixWriter");
			if (exp_file != NULL)
			{
				PMatrixExp::GetInstance().Set(exp_file, tbsExport::GetInstance()->interface_);
				PMatrixExp::GetInstance().Export();
			}

		}break;
		case IDC_KGCEXP:
		{
			TSTR exp_file = PSCWriter::GetInstance().SaveFileDialog(L"tbs", L"kgcWriter");
			if (exp_file != NULL)
			{
				PSCWriter::GetInstance().Set(exp_file, tbsExport::GetInstance()->interface_);
				PSCWriter::GetInstance().Export();
			}
		}break;
		}
	}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		GetCOREInterface()->RollupMouseMessage(hWnd, msg, wParam, lParam);
		break;
	default:
		return 0;
	}
	return 1;
}

