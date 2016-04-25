#include "main.h"
#include "rc.h"
#include <tlhelp32.h>
#include <commctrl.h>
#include <psapi.h>

HWND Windows;

int NumberDialog::IDD(){
	return 0; 
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
	ListProcesses.Create(*this, WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | WS_BORDER , "", IDC_LV, 0, 0, 500, 500);

	EndProcess.Create(*this, WS_CHILD | WS_VISIBLE | WS_BORDER, "End Task", IDC_ENDPROCES, 0, 510, 120, 40);
	EndProcess.Create(*this, WS_CHILD | WS_VISIBLE | WS_BORDER, "Refresh now", ID_REFRESH, 130, 510, 120, 40);


	ListView* lv = new ListView();
	lv->SetExSyles("fullrow header overflow", ListProcesses);
	lv->AddColumn(0, 190, "Ime", ListProcesses);
	lv->AddColumn(1, 100, "ProcessID", ListProcesses);
	lv->AddColumn(2, 120, "Broj threadova", ListProcesses);
	lv->AddColumn(3, 350, "Lokacija", ListProcesses);
	GetProcesses();
	return 0;
}

void MainWindow::OnCommand(int id){		
	int index;
	NumberDialog ndl;

	switch(id){	
		case ID_NEWTASK: 
			break;
		case LVN_COLUMNCLICK:
			index = 0;
			break;
		case IDC_ENDPROCES:
			index = SendMessage(ListProcesses, LVM_GETNEXTITEM, (WPARAM)-1, (LPARAM)LVNI_SELECTED);
			while (index != -1) 
			{
				KillProcess(index);
				index = SendMessage(ListProcesses, LVM_GETNEXTITEM, index, (LPARAM)LVNI_SELECTED);
			}
			break;		
		case ID_REFRESH:
			break;			
		case ID_EXIT: 
			DestroyWindow(*this);
			break;
	}

	GetProcesses();
}

void MainWindow::OnDestroy(){
	::PostQuitMessage(0);
}

MainWindow::MainWindow(){

};

bool MainWindow::GetProcesses()
{
	SendMessage(ListProcesses, LVM_DELETEALLITEMS, 0, 0);
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
		HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
		if (hModuleSnap == INVALID_HANDLE_VALUE)
		{
			continue;
		}

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		ListView* lv = new ListView();
		char procID[50],threadCount[50], priority[50];
		sprintf(procID, "%d", pe32.th32ProcessID);
		sprintf(threadCount, "%d", pe32.cntThreads);		

		lv->AddItem(subitemIndex, 0, pe32.szExeFile, ListProcesses);
		lv->AddItem(subitemIndex, 1, procID, ListProcesses);
		lv->AddItem(subitemIndex, 2, threadCount, ListProcesses);		

		ListProcessModules(pe32.th32ProcessID, subitemIndex);

		//Nekako izraèunati memoriju za pojedini proces da je kao 
		//PrintMemoryInfo(pe32.th32ProcessID, subitemIndex);

		subitemIndex++;
	} 
	while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return(true);
}

bool MainWindow::ListProcessModules(DWORD dwPID, int subitemIndex)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return(false);
	}

	me32.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(hModuleSnap, &me32))
	{
		CloseHandle(hModuleSnap);
		return(false);
	}

	do
	{
		ListView* lv = new ListView();
		char  lokacija[500];
		sprintf(lokacija, "%s", me32.szExePath);
		lv->AddItem(subitemIndex, 3, lokacija, ListProcesses);
	} while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);
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
		char workingSetSize[50];
		sprintf(workingSetSize, "%d", pmc.PagefileUsage);
		ListView* lv = new ListView();
		lv->AddItem(subitemIndex, 3, workingSetSize, ListProcesses);
	}

	CloseHandle(hProcess);
}

bool MainWindow::KillProcess(int index)
{
	char retText[500];

	int report;
	LVITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_STATE;
	lvi.state = LVIS_SELECTED;
	lvi.stateMask = LVIS_SELECTED;
	lvi.iSubItem = 1;
	lvi.pszText = retText;
	lvi.cchTextMax = MAX_PATH;
	lvi.iItem = index;

	SendMessage(ListProcesses, LVM_GETITEM, 0, (LPARAM)&lvi);

	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, atoi(lvi.pszText));

	if (TerminateProcess(hProcess, 0) == 0)
	{
		MessageBox(NULL, "Terminating process failed !", "KillProcess", MB_OK | MB_ICONERROR);
	}

	CloseHandle(hProcess);

	return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	Application app;
	MainWindow* wnd = new MainWindow();
	
	wnd->Create(NULL, WS_OVERLAPPEDWINDOW | WS_VISIBLE, "NWP", 
		(int)LoadMenu(hInstance, MAKEINTRESOURCE(IDM_MAIN)),0,0,520,630);

	return app.Run();
}
