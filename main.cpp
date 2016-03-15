#include "main.h"
#include "rc.h"

HWND Windows;

int NumberDialog::IDD(){
	return IDD_NUMBER; 
}
bool NumberDialog::OnInitDialog(){
	
	return true;
}
bool NumberDialog::OnOK(){
	try
	{		
		return true;
	}
	catch (XCtrl e)
	{
		return false;
	}
}

 
void MainWindow::OnPaint(HDC hdc)
{
	
}

int MainWindow::OnCreate(CREATESTRUCT* pcs)
{
	listBox.Create(*this, WS_CHILD | WS_VISIBLE | WS_BORDER, "", IDC_LB, 0, 0, 640, 640);
	GetTasks();
	return 0;
}

void MainWindow::OnCommand(int id){		
	
	NumberDialog ndl;	
	switch(id){	
		case ID_NEWTASK: 
			break;
		case ID_EXIT: 
			DestroyWindow(*this);
			break;
	}
}

void MainWindow::OnDestroy(){
	::PostQuitMessage(0);
}

MainWindow::MainWindow(){

};

bool MainWindow::GetTasks()
{
	SendMessage(listBox, LB_RESETCONTENT, 0, 0);


	Windows = GetFirstWindowText(Data, sizeof(Data), 0);

	SendMessage(listBox, LB_ADDSTRING, (WPARAM)-1, (LPARAM)Data);

	while (Windows)
	{
		GetNextWindowText(Data, sizeof(Data), &Windows);

		if (*Data && IsWindowVisible(Windows))
		{
			if (strcmp(Data, "Program Manager"))
				SendMessage(listBox, LB_ADDSTRING, (WPARAM)-1, (LPARAM)Data);
		}
	}

	return false;
}

HWND MainWindow::GetFirstWindowText(char buf[], unsigned int max_out, int *text_written){
	HWND ret_val;
	ret_val = GetForegroundWindow();
	if (!text_written) GetWindowText(ret_val, buf, max_out);
	else *text_written = GetWindowText(ret_val, buf, max_out);
	return ret_val;
}

int MainWindow::GetNextWindowText(char buf[], unsigned int max_out, HWND* handle){
	*handle = GetNextWindow(*handle, GW_HWNDNEXT);
	return GetWindowText(*handle, buf, max_out);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	Application app;
	MainWindow* wnd = new MainWindow();
	
	wnd->Create(NULL, WS_OVERLAPPEDWINDOW | WS_VISIBLE, "NWP", 
		(int)LoadMenu(hInstance, MAKEINTRESOURCE(IDM_MAIN)));	

	return app.Run();
}
