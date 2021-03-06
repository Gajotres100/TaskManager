// synchronize UNICODE options: UNICODE is for windows.h, _UNICODE is for tchar.h
#if defined(_UNICODE) && !defined(UNICODE)
        #define UNICODE
#endif
#if defined(UNICODE) && !defined(_UNICODE)
        #define _UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <string>
#include <sstream>
#include <tchar.h>
#include <commctrl.h>

typedef std::basic_string<TCHAR> tstring;
typedef std::basic_stringstream<TCHAR> tsstream;

class Application 
{
public:
	int Run();
};

class Window
{
	HWND hw;

protected:
	virtual tstring ClassName();
	bool Register(const tstring& name);
	tstring GenerateClassName();

public:
	Window();
	bool Create(HWND parent, DWORD style, LPCTSTR caption=0, int IdOrMenu=0, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int width = CW_USEDEFAULT, int height = CW_USEDEFAULT);
	operator HWND();
	static LRESULT CALLBACK Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//	messages
protected:
	virtual int OnCreate(CREATESTRUCT*) { return 0; }
	virtual void OnCommand(int)  { }
	virtual void OnDestroy()  { }
	virtual void OnKeyDown(int key)  { }
	virtual void OnKeyUp(int key)  { }
	virtual void OnChar(TCHAR c)  { }
	virtual void OnLButtonDown(POINT p)  { }
	virtual void OnRButtonDown(POINT p)  { }
	virtual void OnLButtonUp(POINT p)  { }
	virtual void OnLButtonDblClk(POINT p)  { }
	virtual void OnTimer(int id)  { }
	virtual void OnNotify(LPARAM lParam)  { }
};


class Button : public Window
{
protected:
	virtual tstring ClassName(){ return tstring(_T("BUTTON")); }
};

class Edit : public Window
{
protected:
	virtual tstring ClassName(){ return tstring(_T("EDIT")); }
};

struct ListItem {	
	std::string szExeFile;
	std::string procID;
	std::string threadCount;
	std::string location;
};

class ListView : public Window
{
public:	
	virtual bool AddColumn(int index, int width, TCHAR* title, HWND handle);
	virtual void SetExSyles(TCHAR* styles, HWND handle);
	std::string ClassName(){ return WC_LISTVIEW; }
};

class ListBox : public Window
{
public:
	std::string ClassName(){ return "LISTBOX"; }	
};
