#pragma once
#include <string>
class XCtrl {};

typedef std::basic_string<TCHAR> tstring;

class Dialog
{
	HWND hw;
public:
	static int CALLBACK Proc(HWND hw, UINT msg, WPARAM wp, LPARAM lp);
	operator HWND() const { return hw; }
	int DoModal(HINSTANCE, HWND parent=0);
protected:
//	operations
	void SetInt(int idCtrl, int val);
	int GetInt(int idCtrl) throw (XCtrl);
	void SetText(int idCtrl, const tstring& val);
	tstring GetText(int idCtrl);
	void SetReal(int idCtrl, double val);
	double GetReal(int idCtrl);

//	overridables
	virtual int IDD() = 0;
	virtual bool OnInitDialog() { return false; }
	virtual bool OnCommand(int id, int code) { return false; }
	virtual bool OnOK() { return true; }
	virtual void OnCancel() { }
};
