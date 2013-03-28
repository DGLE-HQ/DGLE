//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

DGLE_DYNAMIC_FUNC

#define DLL_PATH	"..\\..\\DGLE.dll"

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner), pEngineCore(NULL), pRender3D(NULL),
    uiCounter(0)
{
	GetEngine(DLL_PATH, pEngineCore);
}
//---------------------------------------------------------------------------
__fastcall TForm1::~TForm1()
{
	FreeEngine();
}
//---------------------------------------------------------------------------
void DGLE_API TForm1::s_Init(void *pParameter)
{
	((TForm1 *)pParameter)->EngineInit();
}
//---------------------------------------------------------------------------
void DGLE_API TForm1::s_Render(void *pParameter)
{
	((TForm1 *)pParameter)->EngineRender();
}
//---------------------------------------------------------------------------
void DGLE_API TForm1::s_Update(void *pParameter)
{
	((TForm1 *)pParameter)->EngineUpdate();
}
//---------------------------------------------------------------------------
void TForm1::EngineInit()
{
	IRender *p_render;
	pEngineCore->GetSubSystem(ESS_RENDER,(IEngineSubSystem *&)p_render);
	p_render->GetRender3D(pRender3D);

	IResourceManager *p_rman;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER,(IEngineSubSystem *&)p_rman);
    p_rman->GetDefaultResource(EOT_TEXTURE,(IEngineBaseObject *&)pTex);
    p_rman->GetDefaultResource(EOT_MESH,(IEngineBaseObject *&)pMesh);
}
//---------------------------------------------------------------------------
void TForm1::EngineRender()
{
	pRender3D->MultMatrix(MatrixTranslate(TVector3(0.f, 0.f, -10.f - ScrollBar1->Position)));
    pRender3D->MultMatrix(MatrixRotate(uiCounter / 2.f, TVector3(0.25f, 0.5f, 0.25f)) *
    	MatrixScale(TVector3(10.f, 10.f, 10.f)));

    pTex->Bind();
    pMesh->Draw();
}
//---------------------------------------------------------------------------
void TForm1::EngineUpdate()
{
	++uiCounter;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
	if (!pEngineCore)
    	return;

	pEngineCore->InitializeEngine(Panel1->Handle, Form1->Caption.t_str());
 	pEngineCore->AddProcedure(EPT_INIT, &s_Init, this);
 	pEngineCore->AddProcedure(EPT_RENDER, &s_Render, this);
 	pEngineCore->AddProcedure(EPT_UPDATE, &s_Update, this);
	pEngineCore->StartEngine();
}
//---------------------------------------------------------------------------

