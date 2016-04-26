#include "nwpwin.h"
#include "nwpdlg.h"



class NumberDialog : public Dialog {
public:
	int broj;
protected:
	int IDD();
	bool OnInitDialog();
	bool OnOK();
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
	bool ListProcessModules(DWORD dwPID, int subitemIndex);
	bool PrintMemoryInfo(DWORD dwPID, int subitemIndex);
	bool KillProcess(int index);
	char Data[265];
	HWND GetFirstWindowText(char buf[], unsigned int max_out, int *text_written);
	int GetNextWindowText(char buf[], unsigned int max_out, HWND* handle);
	bool OnColumnClick(LPNMLISTVIEW pLVInfo);
protected:
	void OnPaint(HDC hdc);
	void OnCommand(int id);
	void OnDestroy();
	int OnCreate(CREATESTRUCT* pcs);
	void OnNotify(LPARAM lParam);
};
