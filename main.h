#include "win.h"
#include "dlg.h"
#include "tchar.h"
#include <iostream>
#include <vector>

enum class LVProcSize { 
	X = 0, 
	Y = 0, 
	Width = 500, 
	Height = 500 
};

enum class BtnEndProc {
	X = 0,
	Y = 510,
	Width = 130,
	Height = 40
};

enum class BtnRefresh {
	X = 140,
	Y = 510,
	Width = 130,
	Height = 40
};

enum class ColumnWidth {
	Name = 190,
	ProcID = 100,
	ThreadNo = 120,
	Loc = 350
};

enum class MainWinSize {
	X = 0,
	Y = 0,
	Width = 520,
	Height = 630
};




class NewTaskDialog : public Dialog {
public:
	TCHAR s1[128];
	TCHAR s2[128];
protected:
	int IDD();
	bool OnOK();
	bool OnCommand(int id, int code);
	std::string BrowseFile(HWND);
};

class AboutDialog : public Dialog {
public:
protected:
	int IDD();
};

class ProcessInfoDialog : public Dialog {
public:
	int index;
	TCHAR* ProcessID;
protected:
	int IDD();
	bool OnInitDialog();
	bool OnCommand(int id, int code);
};

class HelpDialog : public Dialog {
public:	
protected:
	int IDD();
};

class MainWindow : public Window {
public:	
	TCHAR s1[128];
	TCHAR s2[128];
	MainWindow();
	ListView ListProcesses;
	ListBox ListBox;
	Button EndProcess;
	Button RefreshProceses;
	bool GetTasks();
	bool GetProcesses();
	ListItem* ListProcessModules(DWORD dwPID, int subitemIndex, ListItem* pItem);
	bool PrintMemoryInfo(DWORD dwPID, int subitemIndex);
	bool KillProcess(int index);
	TCHAR Data[265];
	HWND GetFirstWindowText(TCHAR buf[], unsigned int max_out, int *text_written);
	int GetNextWindowText(TCHAR buf[], unsigned int max_out, HWND* handle);
	bool OnColumnClick(LPNMLISTVIEW pLVInfo);
	bool OnRowRMClick(LPNMLISTVIEW pLVInfo);
	bool OnDeletePress(LPNMLISTVIEW pLVInfo);
	void DeleteListViewPointers();
	tstring EnumerateServices(DWORD processId);
protected:
	void OnCommand(int id);
	void OnDestroy();
	int OnCreate(CREATESTRUCT* pcs);
	void OnNotify(LPARAM lParam);
	void OnKeyDown(int i);
};
