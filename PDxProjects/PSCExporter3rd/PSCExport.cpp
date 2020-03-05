#include "pch.h"
#include "PSCWriter.h"

#define PSCExport_CLASS_ID Class_ID(0x162c0a20, 0x517d20e9)
class PSCExport : public SceneExport
{
public:
	PSCExport() {};
	~PSCExport() {};

private:
	PSCWriter psc_writer_;

public:

	virtual int				ExtCount() override;
	virtual const MCHAR* Ext(int n) override;
	virtual const MCHAR* LongDesc() override;
	virtual const MCHAR* ShortDesc() override;
	virtual const MCHAR* AuthorName() override;
	virtual const MCHAR* CopyrightMessage() override;
	virtual const MCHAR* OtherMessage1() override;
	virtual const MCHAR* OtherMessage2() override;
	virtual unsigned int	Version() override;
	virtual void			ShowAbout(HWND hWnd) override;
	virtual int				DoExport(const MCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts = FALSE, DWORD options = 0) override;

};


class PSCExportClassDesc : public ClassDesc2
{
public:
	virtual int IsPublic() { return TRUE; };
	virtual void* Create(BOOL)
	{
		return new PSCExport();
	}
	virtual const TCHAR* ClassName() { return _T("PSCExport v3"); }
	virtual SClass_ID SuperClassID()
	{
		return SCENE_EXPORT_CLASS_ID;
	}
	virtual Class_ID ClassID() 
	{
		return PSCExport_CLASS_ID;
	}
	virtual const TCHAR* Category() { return _T("PSCExporter"); }
	virtual const TCHAR* InternalName() { return _T(""); }
	virtual HINSTANCE HInstance() { return hInstance; }
};

ClassDesc2* GetExportDesc()
{
	static PSCExportClassDesc PSCExportDesc;
	return &PSCExportDesc;

}

int PSCExport::ExtCount()
{
	return 1;
}

const MCHAR* PSCExport::Ext(int n)
{
	return _T("png");
}

const MCHAR* PSCExport::LongDesc()
{
	return _T("PSC exporter v3");
}

const MCHAR* PSCExport::ShortDesc()
{
	return _T("PSC exporter");
}

const MCHAR* PSCExport::AuthorName()
{
	return _T("PSC");
}

const MCHAR* PSCExport::CopyrightMessage()
{
	return _T("Copyright by  Democratic People¡¯s Republic of Korea");
}

const MCHAR* PSCExport::OtherMessage1()
{
	return _T("");
}

const MCHAR* PSCExport::OtherMessage2()
{
	return _T("");
}

unsigned int PSCExport::Version()
{
	return 100;
}

void PSCExport::ShowAbout(HWND hWnd)
{
}

int PSCExport::DoExport(const MCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts, DWORD options)
{
	psc_writer_.Set(name, i);
	psc_writer_.Export();
	return TRUE;
}
