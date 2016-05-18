#include "win.h"
#include "dlg.h"
#include "tchar.h"



class NewTaskDialog : public Dialog {
public:
protected:
	int IDD();
	bool OnOK();
	bool OnCommand(int id, int code);
	bool BrowseFile(HWND);
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
	tstring EnumerateServices(DWORD processId);
protected:
	void OnCommand(int id);
	void OnDestroy();
	int OnCreate(CREATESTRUCT* pcs);
	void OnNotify(LPARAM lParam);
	void OnKeyDown(int i);
};
