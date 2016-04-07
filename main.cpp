#include "main.h"
#include "rc.h"
#include <tlhelp32.h>
#include <commctrl.h>

HWND Windows;
LVCOLUMN LvCol;
LVITEM LvItem;

int NumberDialog::IDD(){
	return IDD_NUMBER; 
}
bool NumberDialog::OnInitDialog(){
	
	return true;
}
bool NumberDialog::OnOK(){
	try
	{		
		return true;
	}
	catch (XCtrl e)
	{
		return false;
	}
}

 
void MainWindow::OnPaint(HDC hdc)
{
	
}

int MainWindow::OnCreate(CREATESTRUCT* pcs)
{
	bool initok = listView.Create(*this, WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | WS_BORDER, "", IDC_LV, 0, 0, 500, 500, true);

	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText = "Title";
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;
	ListView_InsertColumn(listView, 0, &lvc);

	//GetProcesses();
	return 0;
}

void MainWindow::OnCommand(int id){		
	
	NumberDialog ndl;	
	switch(id){	
		case ID_NEWTASK: 
			break;
		case ID_EXIT: 
			DestroyWindow(*this);
			break;
	}
}

void MainWindow::OnDestroy(){
	::PostQuitMessage(0);
}

MainWindow::MainWindow(){

};

bool MainWindow::GetProcesses()
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return(FALSE);
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return(FALSE);
	}

	do
	{
		dwPriorityClass = 0;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		SendMessage(listView, LB_ADDSTRING, 0, (LPARAM)pe32.szExeFile);
		SendMessage(listView, LB_ADDSTRING, 1, (LPARAM)pe32.szExeFile);
	} 
	while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return(true);
}

HWND MainWindow::GetFirstWindowText(char buf[], unsigned int max_out, int *text_written){
	HWND ret_val;
	ret_val = GetForegroundWindow();
	if (!text_written) GetWindowText(ret_val, buf, max_out);
	else *text_written = GetWindowText(ret_val, buf, max_out);
	return ret_val;
}

int MainWindow::GetNextWindowText(char buf[], unsigned int max_out, HWND* handle){
	*handle = GetNextWindow(*handle, GW_HWNDNEXT);
	return GetWindowText(*handle, buf, max_out);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	Application app;
	MainWindow* wnd = new MainWindow();
	
	wnd->Create(NULL, WS_OVERLAPPEDWINDOW | WS_VISIBLE, "NWP", 
		(int)LoadMenu(hInstance, MAKEINTRESOURCE(IDM_MAIN)));

	return app.Run();
}
