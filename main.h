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
	ListView listView;
	ListBox listBox;
	bool GetTasks();
	bool GetProcesses();
	char Data[265];
	HWND GetFirstWindowText(char buf[], unsigned int max_out, int *text_written);
	int GetNextWindowText(char buf[], unsigned int max_out, HWND* handle);
protected:
	void OnPaint(HDC hdc);
	void OnCommand(int id);
	void OnDestroy();
	int OnCreate(CREATESTRUCT* pcs);
};
