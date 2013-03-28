//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include <DGLE.h>
using namespace DGLE;

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TPanel *Panel3;
	TScrollBar *ScrollBar1;
	TLabel *Label1;
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations

	uint		uiCounter;

	IEngineCore	*pEngineCore;
	IRender3D	*pRender3D;
    ITexture	*pTex;
    IMesh		*pMesh;

	static void DGLE_API s_Init(void *pParameter);
	static void DGLE_API s_Render(void *pParameter);
	static void DGLE_API s_Update(void *pParameter);

    void EngineInit();
    void EngineRender();
    void EngineUpdate();

public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
 	__fastcall ~TForm1();
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
