//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.jpeg.hpp>
//-------------------------------------------------------------------------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblStatus;
	TProgressBar *pgbStatus;
	TImage *Image1;
	TLabel *btnCancel;
	TLabel *btnShowFolder;
	TLabel *btnClose;

	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
	void __fastcall btnCancelMouseEnter(TObject *Sender);
	void __fastcall btnCancelMouseLeave(TObject *Sender);
	void __fastcall btnShowFolderClick(TObject *Sender);
	void __fastcall btnShowFolderMouseEnter(TObject *Sender);
	void __fastcall btnShowFolderMouseLeave(TObject *Sender);
	void __fastcall btnCloseMouseEnter(TObject *Sender);
	void __fastcall btnCloseMouseLeave(TObject *Sender);
	bool __fastcall FormHelp(WORD Command, THelpEventData Data, bool &CallHelp);

private:
    volatile bool CancelRequested;

protected:
    void __fastcall WndProc(TMessage &Message) override;

public:
	__fastcall TfrmMain(TComponent* Owner);
	void __fastcall DismThreadTerminated(TObject* Sender);
};


//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif

