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
protected:
	void OnPaint(HDC hdc);
	void OnCommand(int id);
	void OnDestroy();
};
