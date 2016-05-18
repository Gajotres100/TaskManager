#include "main.h"
#include "rc.h"
#include <tlhelp32.h>
#include <commctrl.h>
#include <psapi.h>
#include <vector>


HWND listBoxHWND;

TCHAR FilePath[260];
TCHAR s1[128];
TCHAR s2[128];

int CALLBACK CompareListItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

int AboutDialog::IDD(){
	return IDD_ABOUT;
}

int NewTaskDialog::IDD(){
	return IDD_TASK;
}

bool NewTaskDialog::OnOK(){
	TCHAR Data[265];
	LoadString(0, IDS_EIMEPROC, s1, sizeof s1);
	LoadString(0, IDS_EROR, s2, sizeof s2);

	GetDlgItemText(*this, IDC_TASKNAME, Data, 512);

	if (strcmp(Data, ""))
	{
		ShellExecute(NULL, NULL, Data, NULL, NULL, SW_SHOWNORMAL);
	}
	else
	{
		MessageBox(NULL, s1, s2, MB_OK | MB_ICONWARNING);	
	}

	return true;
}

bool NewTaskDialog::OnCommand(int id, int code)
{
	if (id == IDC_BROWSE)
	{
		BrowseFile(*this);
		if (strcmp(FilePath, "")) SetDlgItemText(*this, IDC_TASKNAME, FilePath);
	}

	return 0;
}

bool NewTaskDialog::BrowseFile(HWND hwnd)
{
	LoadString(0, IDS_EXFILTER, s1, sizeof s1);
	LoadString(0, IDS_DEEXT, s2, sizeof s2);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	FilePath[0] = 0;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = LPSTR(s1);
	ofn.lpstrFile = FilePath;
	ofn.nMaxFile = 260;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = LPSTR(s2);
	GetOpenFileName(&ofn);
}

int ProcessInfoDialog::IDD(){
	return IDD_PROCESSINFO;
}

bool ProcessInfoDialog::OnInitDialog()
{
	SetDlgItemText(*this, IDS_COLL2, ProcessID);

	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, atoi(ProcessID));
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
		TCHAR  vrijednost[500];
		sprintf(vrijednost, "%s", me32.szExePath);
		
		SetDlgItemText(*this, IDS_COLL3, me32.szExePath);
		SetDlgItemText(*this, IDS_COLL4, me32.szModule);

		sprintf(vrijednost, "0x%08X", me32.modBaseAddr);
		SetDlgItemText(*this, IDS_COLL5, vrijednost);

		sprintf(vrijednost, "0x%04X", me32.ProccntUsage);
		SetDlgItemText(*this, IDS_COLL6, vrijednost);

		sprintf(vrijednost, "0x%04X", me32.GlblcntUsage);
		SetDlgItemText(*this, IDS_COLL7, vrijednost);

	} 
	while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);
	return true;
}

bool ProcessInfoDialog::OnCommand(int id, int code)
{
	if (id == IDC_ENDPROCES)
	{
		MainWindow* wnd = new MainWindow();
		wnd->KillProcess(index);
		EndDialog(*this, IDC_ENDPROCES);
	}

	return 0;
}

int HelpDialog::IDD(){
	return IDD_HELP;
}

void MainWindow::OnNotify(LPARAM lParam)
{
	if ((((LPNMHDR)lParam)->idFrom == IDC_LV))
	{
		switch (((LPNMHDR)lParam)->code)
		{
			case LVN_COLUMNCLICK:
				OnColumnClick((LPNMLISTVIEW)lParam);
				break;

			case NM_RCLICK:
				OnRowRMClick((LPNMLISTVIEW)lParam);					
				break;
			case LVN_KEYDOWN:
				LPNMLVKEYDOWN pNMLVKEYDOWN = (LPNMLVKEYDOWN)lParam;
				if (pNMLVKEYDOWN->wVKey == VK_DELETE) OnDeletePress((LPNMLISTVIEW)lParam);	
				else if (pNMLVKEYDOWN->wVKey == VK_F1)
				{
					HelpDialog  help;
					if (help.DoModal(NULL, *this) == IDOK)
					{

					}
				}
				break;				
		}	
	}
}

void MainWindow::OnKeyDown(int code)
{
	switch (code)
	{
	case VK_F1:
		HelpDialog  help;
		if (help.DoModal(NULL, *this) == IDOK)
		{

		}
		break;
	}
}

int MainWindow::OnCreate(CREATESTRUCT* pcs)
{
	ListProcesses.Create(*this, WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | WS_BORDER, "", IDC_LV, 0, 0, 500, 500);
	LoadString(0, IDS_ENDTASK, s1, sizeof s1);
	EndProcess.Create(*this, WS_CHILD | WS_VISIBLE | WS_BORDER, s1, IDC_ENDPROCES, 0, 510, 120, 40);
	LoadString(0, IDS_REFRESH, s1, sizeof s1);
	EndProcess.Create(*this, WS_CHILD | WS_VISIBLE | WS_BORDER, s1, ID_REFRESH, 130, 510, 120, 40);

	listBoxHWND = ListProcesses;

	ListView* lv = new ListView();
	LoadString(0, IDS_EXTRASTYLE, s1, sizeof s1);
	lv->SetExSyles(LPSTR(s1), ListProcesses);
	LoadString(0, IDS_COLL1, s1, sizeof s1);
	lv->AddColumn(0, 190, s1, ListProcesses);
	LoadString(0, IDS_COLL2, s1, sizeof s1);
	lv->AddColumn(1, 100, s1, ListProcesses);
	LoadString(0, IDS_COLL3, s1, sizeof s1);
	lv->AddColumn(2, 120, s1, ListProcesses);
	LoadString(0, IDS_COLL4, s1, sizeof s1);
	lv->AddColumn(3, 350, s1, ListProcesses);
	GetProcesses();
	return 0;
}

void MainWindow::OnCommand(int id){		
	int index;	
	NewTaskDialog  ndl;
	HelpDialog  help;
	AboutDialog about;
	HMENU hMenu = GetMenu(*this);
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };

	switch(id){	
		case ID_NEWTASK: 
			if (ndl.DoModal(NULL, *this) == IDOK)
			{
				index = 0;
			}		
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
		case IDD_HELP:
			if (help.DoModal(NULL, *this) == IDOK){	}
			break;
		case ID_EXPAND:			
			mii.fMask = MIIM_STATE;
			GetMenuItemInfo(hMenu, ID_EXPAND, FALSE, &mii);
			mii.fState ^= MFS_CHECKED;
			SetMenuItemInfo(hMenu, ID_EXPAND, FALSE, &mii);
			break;
		case ID_ABOUT:
			if (about.DoModal(NULL, *this) == IDOK){}
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

	TCHAR szText[64];
	LVITEM lvi;

	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;


	HMENU hMenu = GetMenu(*this);
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.fMask = MIIM_STATE;
	GetMenuItemInfo(hMenu, ID_EXPAND, FALSE, &mii);

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
		HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
		if (hModuleSnap == INVALID_HANDLE_VALUE && mii.fState != MFS_CHECKED)
		{
			continue;
		}

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		ListView* lv = new ListView();
	

		if (pe32.th32ProcessID > 0)
		{
			int insertIndex = ListView_GetItemCount(ListProcesses);
			

			ListItem* pItem = new ListItem();

			LV_ITEM newItem;

			TCHAR * value = pe32.szExeFile;

			for (int i = 0; i < strlen(value); ++i)
				value[i] = tolower(value[i]);			

			sprintf(pItem->szExeFile, value);
			sprintf(pItem->procID, "%d", pe32.th32ProcessID);
			sprintf(pItem->threadCount, "%d", pe32.cntThreads);

			if (GetModuleFileNameEx(hProcess, NULL, FilePath, MAX_PATH) != 0)
			{
				value = FilePath;
				for (int i = 0; i < strlen(value); ++i)
					value[i] = tolower(value[i]);
				sprintf(pItem->location, value);
			}		
			
			newItem.mask = LVIF_TEXT | LVIF_PARAM;
			newItem.iItem = insertIndex;
			newItem.pszText = pItem->szExeFile;
			newItem.cchTextMax = strlen(pItem->szExeFile);
			newItem.iSubItem = 0;
			newItem.lParam = (LPARAM)pItem;
			insertIndex = SendMessage(ListProcesses, LVM_INSERTITEM, 0, (LPARAM)&newItem);

			newItem.mask = LVIF_TEXT;
			newItem.pszText = pItem->procID;
			newItem.cchTextMax = strlen(pItem->procID);
			newItem.iSubItem = 1;
			SendMessage(ListProcesses, LVM_SETITEM, 0, (LPARAM)&newItem);

			newItem.mask = LVIF_TEXT;
			newItem.pszText = pItem->threadCount;
			newItem.cchTextMax = strlen(pItem->threadCount);
			newItem.iSubItem = 2;
			SendMessage(ListProcesses, LVM_SETITEM, 0, (LPARAM)&newItem);

			newItem.mask = LVIF_TEXT;
			newItem.pszText = pItem->location;
			newItem.cchTextMax = strlen(pItem->location);
			newItem.iSubItem = 3;
			SendMessage(ListProcesses, LVM_SETITEM, 0, (LPARAM)&newItem);
		}
	} 
	while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return(true);
}

bool MainWindow::KillProcess(int index)
{
	TCHAR retText[500];
	LoadString(0, IDS_EROR, s2, sizeof s2);

	int report;
	LVITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_STATE;
	lvi.state = LVIS_SELECTED;
	lvi.stateMask = LVIS_SELECTED;
	lvi.iSubItem = 1;
	lvi.pszText = retText;
	lvi.cchTextMax = MAX_PATH;
	lvi.iItem = index;

	SendMessage(listBoxHWND, LVM_GETITEM, 0, (LPARAM)&lvi);

	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, atoi(lvi.pszText));
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (SCManager == NULL)
		{
			LoadString(0, IDS_NORIGHTS, s1, sizeof s1);
			MessageBox(NULL, s1, s2, MB_OK | MB_ICONERROR);
			return false;
		}

		tstring service;
		if (EnumerateServices(atoi(lvi.pszText)).empty())
		{
			LoadString(0, IDS_UNABLEKILLRSERV, s1, sizeof s1);
			MessageBox(NULL, s1, s2, MB_OK | MB_ICONERROR);
			return false;
		}
		service = EnumerateServices(atoi(lvi.pszText));
		SERVICE_STATUS Status;

		SC_HANDLE SHandle = OpenService(SCManager, service.c_str() , SC_MANAGER_ALL_ACCESS);

		if (SHandle == NULL)
		{
			LoadString(0, IDS_UNABLEKILLRSERV, s1, sizeof s1);
			MessageBox(NULL, s1, s2, MB_OK | MB_ICONERROR);		
		}
		else if (!ControlService(SHandle, SERVICE_CONTROL_STOP, &Status))
		{
			LoadString(0, IDS_ERKILLRSERV, s1, sizeof s1);
			MessageBox(NULL, s1, s2, MB_OK | MB_ICONERROR);
		}

		CloseServiceHandle(SCManager);
		CloseServiceHandle(SHandle);
	}

	else
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, atoi(lvi.pszText));

		LoadString(0, IDS_ERKILLRPROC, s1, sizeof s1);		

		if (TerminateProcess(hProcess, 0) == 0)
		{
			MessageBox(NULL, s1, s2, MB_OK | MB_ICONERROR);
		}

		CloseHandle(hProcess);
	}
	return true;
}

tstring MainWindow::EnumerateServices(DWORD processId)
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
	if (hSCM == NULL)
	{
		return NULL;
	}

	DWORD bufferSize = 0;
	DWORD requiredBufferSize = 0;
	DWORD totalServicesCount = 0;
	EnumServicesStatusEx(hSCM, 	SC_ENUM_PROCESS_INFO,
		SERVICE_WIN32,
		SERVICE_STATE_ALL,
		nullptr,
		bufferSize,
		&requiredBufferSize,
		&totalServicesCount,
		nullptr,
		nullptr);

	std::vector<BYTE> buffer(requiredBufferSize);
	EnumServicesStatusEx(hSCM,
		SC_ENUM_PROCESS_INFO,
		SERVICE_WIN32,
		SERVICE_STATE_ALL,
		buffer.data(),
		buffer.size(),
		&requiredBufferSize,
		&totalServicesCount,
		nullptr,
		nullptr);

	LPENUM_SERVICE_STATUS_PROCESS services =
		reinterpret_cast<LPENUM_SERVICE_STATUS_PROCESS>(buffer.data());
	for (unsigned int i = 0; i < totalServicesCount; ++i)
	{
		ENUM_SERVICE_STATUS_PROCESS service = services[i];
		if (service.ServiceStatusProcess.dwProcessId == processId)
		{
			(void)CloseServiceHandle(hSCM);
			return service.lpServiceName;
		}
	}

	(void)CloseServiceHandle(hSCM);
	
	return "";
}

int CALLBACK CompareListItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	bool isAsc = (lParamSort > 0);
	int nColumn = abs(lParamSort) -1;
	ListItem *item1, *item2;

	item1 = (ListItem*)lParam1;
	item2 = (ListItem*)lParam2;
	
	if (nColumn == 0)
	{
		if (isAsc)
			return strcmp(item2->szExeFile, item1->szExeFile);			
		else
			return strcmp(item1->szExeFile, item2->szExeFile);
	}
	else if (nColumn == 1)
	{
		
		if (isAsc)
		{
			if (atoi(item1->procID) < atoi(item2->procID)) { return -1; }
			if (atoi(item1->procID) > atoi(item2->procID)) { return  1; }
		}
		else
		{
			if (atoi(item2->procID) < atoi(item1->procID)) { return -1; }
			if (atoi(item2->procID) > atoi(item1->procID)) { return  1; }
		}
	}
	else if (nColumn == 2)
	{
		if (isAsc)
		{
			if (atoi(item1->threadCount) < atoi(item2->threadCount)) { return -1; }
			if (atoi(item1->threadCount) > atoi(item2->threadCount)) { return  1; }
		}
		else
		{
			if (atoi(item2->threadCount) < atoi(item1->threadCount)) { return -1; }
			if (atoi(item2->threadCount) > atoi(item1->threadCount)) { return  1; }
		}
	}
	else if (nColumn == 3)
	{
		if (isAsc)
			return strcmp(item1->szExeFile, item2->szExeFile);
		else
			return strcmp(item2->szExeFile, item1->szExeFile);
	}
}

bool MainWindow::OnColumnClick(LPNMLISTVIEW pLVInfo)
{
	static int nSortColumn = 0;
	static BOOL bSortAscending = TRUE;
	LPARAM lParamSort;

	if (pLVInfo->iSubItem == nSortColumn)
		bSortAscending = !bSortAscending;
	else
	{
		nSortColumn = pLVInfo->iSubItem;
		bSortAscending = TRUE;
	}
	lParamSort = 1 + nSortColumn;
	if (!bSortAscending)
		lParamSort = -lParamSort;
	
	ListView_SortItems(pLVInfo->hdr.hwndFrom, CompareListItems, lParamSort);	

	return 0;

}

bool MainWindow::OnRowRMClick(LPNMLISTVIEW pLVInfo)
{
	TCHAR retText[500];
	int row = ((LPNMLISTVIEW)pLVInfo)->iItem;
	LVITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_STATE;
	lvi.state = LVIS_SELECTED;
	lvi.stateMask = LVIS_SELECTED;
	lvi.iSubItem = 1;
	lvi.pszText = retText;
	lvi.cchTextMax = MAX_PATH;
	lvi.iItem = row;
	SendMessage(pLVInfo->hdr.hwndFrom, LVM_GETITEM, 0, (LPARAM)&lvi);

	ProcessInfoDialog  pid;
	pid.ProcessID = retText;
	pid.index = row;
	if (pid.DoModal(NULL, *this) == IDC_ENDPROCES)
	{
		
	}
	GetProcesses();
	return 0;
}

bool MainWindow::OnDeletePress(LPNMLISTVIEW pLVInfo)
{
	int index = SendMessage(pLVInfo->hdr.hwndFrom, LVM_GETNEXTITEM, (WPARAM)-1, (LPARAM)LVNI_SELECTED);
	while (index != -1)
	{
		KillProcess(index);
		index = SendMessage(pLVInfo->hdr.hwndFrom, LVM_GETNEXTITEM, index, (LPARAM)LVNI_SELECTED);
	}
	GetProcesses();
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	Application app;
	MainWindow* wnd = new MainWindow();

	wnd->Create(NULL, WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE, "TM",
		(int)LoadMenu(hInstance, MAKEINTRESOURCE(IDM_MAIN)),0,0,520,630);

	return app.Run();
}
