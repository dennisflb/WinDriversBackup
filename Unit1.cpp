//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"
#include <windows.h>
#include <shellapi.h>

//---------------------------------------------------------------------------
// Static global for driver export path
static UnicodeString destPath;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;

// TDismThread definition
class TDismThread : public TThread
{
private:
    TfrmMain *FMainForm;
    UnicodeString destPath;
    volatile bool *CancelRequested;
public:
    __fastcall TDismThread(TfrmMain *MainForm, const UnicodeString &ADestPath, volatile bool *ACancelRequested)
        : TThread(true), FMainForm(MainForm), destPath(ADestPath), CancelRequested(ACancelRequested)
    {
        FreeOnTerminate = true;
    }
protected:
    void __fastcall Execute() override
    {
        UnicodeString cmdLine = L"/c dism /online /export-driver /destination=\"" + destPath + L"\"";

        wchar_t cmd[600];
        wcsncpy(cmd, cmdLine.c_str(), 599);
        cmd[599] = 0;

        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        BOOL ok = CreateProcessW(
            L"C:\\Windows\\System32\\cmd.exe", cmd, NULL, NULL, FALSE,
            CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
        if (!ok) {
            Synchronize([this]() {
                ShowMessage(L"CreateProcessW failed.");
                FMainForm->btnCancel->Visible = false;
                FMainForm->btnShowFolder->Visible = true;
                FMainForm->btnClose->Visible = true;
                FMainForm->pgbStatus->Visible = false;
            });
            return;
        }

        DWORD result = WAIT_TIMEOUT;
        while (result != WAIT_OBJECT_0) {
            if (*CancelRequested) {
                TerminateProcess(pi.hProcess, 0);
                break;
            }
            result = WaitForSingleObject(pi.hProcess, 50);
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}; // <-- This closing brace must be here!

// Now implement TfrmMain methods OUTSIDE the class
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnCloseClick(TObject *Sender)
{
	Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormShow(TObject *Sender)
{
    // Get the computer name
    wchar_t hostname[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    if (!GetComputerNameW(hostname, &size)) {
        ShowMessage(L"Failed to get computer name.");
        Application->Terminate();
        return;
    }
    destPath = L"C:\\";
    destPath += hostname;
    destPath += L"-Drivers";

    lblStatus->Caption = L"Saving Files To: \"" + destPath + L"\"";
    CancelRequested = false;

    // Prepare UI for export
    pgbStatus->Visible = true;
    btnCancel->Visible = true;
    btnShowFolder->Visible = false;
    btnClose->Visible = false;

    // Ensure the directory exists
    CreateDirectoryW(destPath.c_str(), NULL);

    // Start the DISM process in a background thread
    TDismThread *dismThread = new TDismThread(this, destPath, &CancelRequested);
    dismThread->OnTerminate = DismThreadTerminated;
    dismThread->Start();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::DismThreadTerminated(TObject *Sender)
{
    pgbStatus->Visible = false;
    btnCancel->Visible = false;
    btnShowFolder->Visible = true;
    btnClose->Visible = true;
    lblStatus->Caption = L"Files Saved To: \"" + destPath + L"\"";
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnCancelClick(TObject *Sender)
{
    CancelRequested = true;
    Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnCancelMouseEnter(TObject *Sender)
{
    btnCancel->Font->Color = clRed;
    btnCancel->Font->Style = TFontStyles() << fsBold << fsUnderline;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnCancelMouseLeave(TObject *Sender)
{
    btnCancel->Font->Color = clWindowText;
    btnCancel->Font->Style = TFontStyles()<< fsBold;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnShowFolderClick(TObject *Sender)
{
    UnicodeString selectArg = L"/select,\"" + destPath + L"\"";
    ShellExecuteW(0, L"open", L"explorer.exe", selectArg.c_str(), NULL, SW_SHOWNORMAL);
    Sleep(300);
    Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnShowFolderMouseEnter(TObject *Sender)
{
    btnShowFolder->Font->Color = clRed;
    btnShowFolder->Font->Style = TFontStyles() << fsBold << fsUnderline;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnShowFolderMouseLeave(TObject *Sender)
{
    btnShowFolder->Font->Color = clWindowText;
	btnShowFolder->Font->Style = TFontStyles()<< fsBold;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnCloseMouseEnter(TObject *Sender)
{
    btnClose->Font->Color = clRed;
    btnClose->Font->Style = TFontStyles() << fsBold << fsUnderline;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnCloseMouseLeave(TObject *Sender)
{
    btnClose->Font->Color = clWindowText;
	btnClose->Font->Style = TFontStyles()<< fsBold;
}
//---------------------------------------------------------------------------

bool __fastcall TfrmMain::FormHelp(WORD Command, THelpEventData Data, bool &CallHelp)
{
    ShowMessage(L"Custom help triggered!");
    CallHelp = false;
    return true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::WndProc(TMessage &Message)
{
    if (Message.Msg == WM_SYSCOMMAND && (Message.WParam & 0xFFF0) == SC_CONTEXTHELP)
    {
        frmAbout->ShowModal(); // Show your About form modally
        return; // Block default help mode and cursor
    }
    TForm::WndProc(Message);
}
//---------------------------------------------------------------------------

