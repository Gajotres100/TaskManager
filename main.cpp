#include "main.h"
#include "rc.h"
#include <tlhelp32.h>
#include <commctrl.h>
#include <psapi.h>

HWND Windows;

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

	ListView* lv = new ListView();
	lv->AddColumn(0, 200, "Ime", listView);
	lv->AddColumn(1, 100, "Process ID", listView);
	lv->AddColumn(2, 100, "Thread count", listView);
	lv->AddColumn(3, 100, "Priority base", listView);
	GetProcesses();
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
	int subitemIndex = 0;
	do
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		ListView* lv = new ListView();
		char procID[50],threadCount[50], priority[50];
		sprintf(procID, "%d", pe32.th32ProcessID);
		sprintf(threadCount, "%d", pe32.cntThreads);
		sprintf(priority, "%d", pe32.pcPriClassBase);

		lv->AddItem(subitemIndex, 0, pe32.szExeFile, listView);
		lv->AddItem(subitemIndex, 1, procID, listView);
		lv->AddItem(subitemIndex, 2, threadCount, listView);

		PrintMemoryInfo(pe32.th32ProcessID, subitemIndex);

		subitemIndex++;
	} 
	while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return(true);
}

bool MainWindow::PrintMemoryInfo(DWORD processID, int subitemIndex)
{
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL == hProcess)
		return false;

	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		char procID[50];
		sprintf(procID, "%d", (pmc.WorkingSetSize / 2048));
		ListView* lv = new ListView();
		lv->AddItem(subitemIndex, 3, procID, listView);	
	}

	CloseHandle(hProcess);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	Application app;
	MainWindow* wnd = new MainWindow();
	
	wnd->Create(NULL, WS_OVERLAPPEDWINDOW | WS_VISIBLE, "NWP", 
		(int)LoadMenu(hInstance, MAKEINTRESOURCE(IDM_MAIN)));

	return app.Run();
}
