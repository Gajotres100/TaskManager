#include "main.h"
#include "rc.h"
#include <tlhelp32.h>
#include <commctrl.h>
#include <psapi.h>
#include <iostream>
#include <vector>

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
		std::string filePath = BrowseFile(*this);

		if (filePath.compare(""))
			SetDlgItemText(*this, IDC_TASKNAME, filePath.c_str());
	}

	return 0;
}

std::string NewTaskDialog::BrowseFile(HWND hwnd)
{
	TCHAR FilePath[260];
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
	return FilePath;
}

int ProcessInfoDialog::IDD(){
	return IDD_PROCESSINFO;
}

bool ProcessInfoDialog::OnInitDialog()
{
	SetDlgItemText(*this, IDS_COLL2, ProcessID);
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, atoi(ProcessID));
	std::string  value;
	TCHAR szBuffer[MAX_PATH];
	TCHAR s1[128];
	DWORD dwSize = sizeof(szBuffer) / sizeof(szBuffer[0]) - 1;
	bool iResult = QueryFullProcessImageName(hProcess, 0, szBuffer, &dwSize);
	if (!iResult)
	{
		LoadString(0, IDS_DEFAULTPROCLOC, s1, sizeof s1);
		value = s1;
	}
	else value = szBuffer;

	for (int i = 0; value[i]; i++) value[i] = tolower(value[i]);		
	SetDlgItemText(*this, IDS_COLL3, value.c_str());	
	CloseHandle(hProcess);
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
			/*case NM_RCLICK:
				OnRowRMClick((LPNMLISTVIEW)lParam);					
				break;*/
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
	ListProcesses.Create(*this, WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | WS_BORDER | LVS_SHOWSELALWAYS, "", IDC_LV, (int)LVProcSize::X, (int)LVProcSize::Y, (int)LVProcSize::Width, (int)LVProcSize::Height);
	LoadString(0, IDS_ENDTASK, s1, sizeof s1);
	EndProcess.Create(*this, WS_CHILD | WS_VISIBLE | WS_BORDER, s1, IDC_ENDPROCES, (int)BtnEndProc::X, (int)BtnEndProc::Y, (int)BtnEndProc::Width, (int)BtnEndProc::Height);
	LoadString(0, IDS_REFRESH, s1, sizeof s1);
	EndProcess.Create(*this, WS_CHILD | WS_VISIBLE | WS_BORDER, s1, ID_REFRESH, (int)BtnRefresh::X, (int)BtnRefresh::Y, (int)BtnRefresh::Width, (int)BtnRefresh::Height);


	LoadString(0, IDS_EXTRASTYLE, s1, sizeof s1);
	ListProcesses.SetExSyles(LPSTR(s1), ListProcesses);
	LoadString(0, IDS_COLL1, s1, sizeof s1);
	ListProcesses.AddColumn(0, (int)ColumnWidth::Name, s1, ListProcesses);
	LoadString(0, IDS_COLL2, s1, sizeof s1);
	ListProcesses.AddColumn(1, (int)ColumnWidth::ProcID, s1, ListProcesses);
	LoadString(0, IDS_COLL3, s1, sizeof s1);
	ListProcesses.AddColumn(2, (int)ColumnWidth::ThreadNo, s1, ListProcesses);
	LoadString(0, IDS_COLL4, s1, sizeof s1);
	ListProcesses.AddColumn(3, (int)ColumnWidth::Loc, s1, ListProcesses);
	
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
			index = ListView_GetNextItem(ListProcesses, (WPARAM)-1, (LPARAM)LVNI_SELECTED);
			while (index != -1) 
			{
				KillProcess(index);
				ListView_GetNextItem(ListProcesses, index, (LPARAM)LVNI_SELECTED);				
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
			index = 0;
			int rownum = ListView_GetItemCount(ListProcesses);
			while (rownum > 0 && rownum > index)
			{
				LVITEM lvi;
				lvi.iItem = index;
				ListView_GetItem(ListProcesses, (LPARAM)&lvi);
				index++;
				delete static_cast<ListItem*>((void*)lvi.lParam);
				ListView_GetNextItem(ListProcesses, index, LVNI_ALL);
			}
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
	int index = 0;
	int rownum = ListView_GetItemCount(ListProcesses);
	while (rownum > 0 && rownum > index)
	{
		LVITEM lvi;		
		lvi.iItem = index;
		ListView_GetItem(ListProcesses, (LPARAM)&lvi);
		index++;
		delete static_cast<ListItem*>((void*)lvi.lParam);
		ListView_GetNextItem(ListProcesses, index, LVNI_ALL);
	}

	ListView_DeleteAllItems(ListProcesses);

	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;

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

		ListItem* pItem = new ListItem();

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);	

		if (pe32.th32ProcessID > 0)
		{
			int insertIndex = ListView_GetItemCount(ListProcesses);

			std::string value = pe32.szExeFile;	
			for (int i = 0; value[i]; i++) value[i] = tolower(value[i]);

			pItem->szExeFile = value.c_str();
			pItem->procID = std::to_string(pe32.th32ProcessID);
			pItem->threadCount = std::to_string(pe32.cntThreads);

			TCHAR szBuffer[MAX_PATH];
			DWORD dwSize = sizeof(szBuffer) / sizeof(szBuffer[0]) - 1;
			bool iResult = QueryFullProcessImageName(hProcess, 0, szBuffer, &dwSize);
			if (!iResult)
			{
				LoadString(0, IDS_DEFAULTPROCLOC, s1, sizeof s1);
				value = s1;
			}
			else value = szBuffer;

			for (int i = 0; value[i]; i++) value[i] = tolower(value[i]);			
			pItem->location = value;
		
			LV_ITEM newItem;
			newItem.mask = LVIF_TEXT | LVIF_PARAM;
			newItem.iItem = insertIndex;
			newItem.pszText = (LPSTR)(pItem->szExeFile.c_str());
			newItem.cchTextMax = strlen(pItem->szExeFile.c_str());
			newItem.iSubItem = 0;

			newItem.lParam = (LPARAM)pItem;		
			
			insertIndex = ListView_InsertItem(ListProcesses, &newItem);
			ListView_SetItemText(ListProcesses, insertIndex, 1, (LPSTR)(pItem->procID.c_str()));
			ListView_SetItemText(ListProcesses, insertIndex, 2, (LPSTR)(pItem->threadCount.c_str()));
			ListView_SetItemText(ListProcesses, insertIndex, 3, (LPSTR)(pItem->location.c_str()));
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

	LVITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_STATE;
	lvi.state = LVIS_SELECTED;
	lvi.stateMask = LVIS_SELECTED;
	lvi.iSubItem = 1;
	lvi.pszText = retText;
	lvi.cchTextMax = MAX_PATH;
	lvi.iItem = index;	

	ListView_GetItem(ListProcesses, (LPARAM)&lvi);

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
			return strcmp(item2->szExeFile.c_str(), item1->szExeFile.c_str());
		else
			return strcmp(item1->szExeFile.c_str(), item2->szExeFile.c_str());
	}
	else if (nColumn == 1)
	{		
		if (isAsc)
		{
			if (atoi(item1->procID.c_str()) < atoi(item2->procID.c_str())) { return -1; }
			if (atoi(item1->procID.c_str()) > atoi(item2->procID.c_str())) { return  1; }
		}
		else
		{
			if (atoi(item2->procID.c_str()) < atoi(item1->procID.c_str())) { return -1; }
			if (atoi(item2->procID.c_str()) > atoi(item1->procID.c_str())) { return  1; }
		}
	}
	else if (nColumn == 2)
	{
		if (isAsc)
		{
			if (atoi(item1->threadCount.c_str()) < atoi(item2->threadCount.c_str())) { return -1; }
			if (atoi(item1->threadCount.c_str()) > atoi(item2->threadCount.c_str())) { return  1; }
		}
		else
		{
			if (atoi(item2->threadCount.c_str()) < atoi(item1->threadCount.c_str())) { return -1; }
			if (atoi(item2->threadCount.c_str()) > atoi(item1->threadCount.c_str())) { return  1; }
		}
	}
	else if (nColumn == 3)
	{
		if (isAsc)
			return strcmp(item1->szExeFile.c_str(), item2->szExeFile.c_str());
		else
			return strcmp(item2->szExeFile.c_str(), item1->szExeFile.c_str());
	}
	return 0;
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
	ListView_GetItem(pLVInfo->hdr.hwndFrom, (LPARAM)&lvi);

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
	ListView_GetNextItem(pLVInfo->hdr.hwndFrom, (WPARAM)-1, (LPARAM)LVNI_SELECTED);
	int index = ListView_GetNextItem(pLVInfo->hdr.hwndFrom, (WPARAM)-1, (LPARAM)LVNI_SELECTED);
	while (index != -1)
	{
		KillProcess(index);
		index = ListView_GetNextItem(pLVInfo->hdr.hwndFrom, index, (LPARAM)LVNI_SELECTED);
	}
	GetProcesses();
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	Application app;
	MainWindow* wnd = new MainWindow();

	wnd->Create(NULL, WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE, "TM",
		(int)LoadMenu(hInstance, MAKEINTRESOURCE(IDM_MAIN)), (int)MainWinSize::X, (int)MainWinSize::Y, (int)MainWinSize::Width, (int)MainWinSize::Height);

	return app.Run();
}
