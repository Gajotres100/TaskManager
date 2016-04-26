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
	virtual void OnPaint(HDC dc)  { }
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

class ListView : public Window
{
public:
	std::string ClassName(){ return WC_LISTVIEW; }
	bool AddColumn(int index, int width, char* title, HWND handle)
	{
		bool success = FALSE;
		LVCOLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx = width;
		lvc.cchTextMax = strlen(title);
		lvc.pszText = title;
		success = ListView_InsertColumn(handle, index, &lvc);
		
		return(success);
	}

	bool AddItem(int index, int subindex, char* value, HWND handle)
	{
		bool success = false;
		LVITEM lvi;		
		lvi.iItem = index;
		lvi.iSubItem = subindex;
		lvi.cchTextMax = strlen(value);
		lvi.pszText = value;

		if (subindex != 0)
		{
			lvi.mask = LVIF_TEXT;
			success = ListView_InsertItem(handle, &lvi);
			ListView_SetItem(handle, &lvi);
		}
		else{
			lvi.mask = LVIF_TEXT | LVIF_PARAM;
			success = ListView_InsertItem(handle, &lvi);
		}

		return(success);
	}

	void SetExSyles(char* styles, HWND handle)
	{
		DWORD exflags = WS_EX_LEFT;

		if (strstr(styles, "fullrow") || strstr(styles, "fullrowsel") ||
			strstr(styles, "full row") || strstr(styles, "full sel"))
		{
			exflags |= LVS_EX_FULLROWSELECT;
		}

		if (strstr(styles, "header"))
		{
			exflags |= LVS_EX_HEADERINALLVIEWS;
		}

		if (strstr(styles, "overflow"))
		{
			exflags |= LVS_EX_COLUMNOVERFLOW;
		}

		if (handle)
		{
			SendMessage(handle, LVM_SETEXTENDEDLISTVIEWSTYLE, exflags, exflags);
		}
	}
};

class ListBox : public Window
{
public:
	std::string ClassName(){ return "LISTBOX"; }	
};
