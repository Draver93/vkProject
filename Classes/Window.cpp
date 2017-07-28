#include "Window.hpp"

UINT cWindow::gWndIndices = 0;
cWindow::cWindow(WNDCLASSEX wc, DWORD style, POINT position, POINT size)
{
	ready = false;
	wId = gWndIndices;
	gWndIndices++;
	lock = FALSE;
	wndState = W_NULL;

	hWindow = NULL;
	wState = wc; 
	wPos = position;
	wSize = size; 
	dwStyle = style;

	wThread = new std::thread(
		[](cWindow *pWnd)
		{
			pWnd->create();
			pWnd->ready = true;
			pWnd->windowLoop();
		}
		, this);
	wThread->detach();
	while (!isReady()) {}
}
cWindow::~cWindow()
{
	wndState = W_CLOSE;

	if (wThread != NULL)
	{
		delete wThread;
		wThread = NULL;
	}
}

HRESULT cWindow::create()
{
	if (lock) return E_FAIL;
	lock = TRUE;

	wState.lpfnWndProc = wHandler;
	wState.hInstance = GetModuleHandle(NULL);
	if (!RegisterClassEx(&wState)) return E_FAIL;	
	if (!(hWindow = CreateWindowEx(NULL, wState.lpszClassName, wState.lpszMenuName, dwStyle, wPos.x, wPos.y,
		wSize.x, wSize.y, NULL, NULL, wState.hInstance, NULL)))	return E_FAIL;
	UpdateWindow(hWindow);

	lock = FALSE;
	return S_OK;
}
HRESULT cWindow::reСreate(WNDCLASSEX wc, DWORD style, POINT position, POINT size)
{
	if (lock) return E_FAIL;
	lock = TRUE;

	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = wHandler;
	wState = wc;
	wPos = position;
	wSize = size;
	dwStyle = style;
	wndState = W_RECREATE;

	lock = FALSE;
	return S_OK;
}
HRESULT cWindow::edit(DWORD style)
{
	if (lock) return E_FAIL;
	lock = TRUE;

	dwStyle = style;
	wndState = W_EDIT;
	
	lock = FALSE;
	return S_OK;
}
HRESULT cWindow::edit(POINT pos, POINT size)
{
	if (lock) return E_FAIL;
	lock = TRUE;

	wPos.x = pos.x;
	wPos.y = pos.y;
	wSize.x = size.x;
	wSize.y = size.y;
	wndState = W_EDIT;

	lock = FALSE;
	return S_OK;
}

int cWindow::windowLoop()
{
	while (	WM_QUIT != msg.message &&			// App close
			WM_NCLBUTTONDOWN != msg.message &&  // cross button
			wndState != W_CLOSE &&				// call from developer
			msg.wParam != SC_CLOSE)				// Alt + F4
	{
		while (PeekMessage(&msg, hWindow, 0, 0, PM_REMOVE) > 0)
		{
			nextQueue();
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	wndState = W_CLOSE;
	delete wThread;
	wThread = NULL;
	return msg.wParam;
}
int cWindow::nextQueue()
{
	if (lock) return 0;
	switch (wndState)
	{
		case W_CLOSE:
		{	
			msg.message = WM_CLOSE;
			break;
		}
		case W_RECREATE:
		{
			DestroyWindow(hWindow);
			if (!UnregisterClass(wState.lpszClassName, wState.hInstance))return -1;
			if (!RegisterClassEx(&wState))return -1;
			if (!(hWindow = CreateWindowEx(NULL, wState.lpszClassName, wState.lpszMenuName, dwStyle, wPos.x, wPos.y,
				wSize.x, wSize.y, NULL, NULL, wState.hInstance, NULL)))return -1;

			UpdateWindow(hWindow);	
			break;
		}
		case W_EDIT:
		{
			SetWindowPos(hWindow, NULL, wPos.x, wPos.y, wSize.x, wSize.y, NULL);
			SetWindowLong(hWindow, GWL_STYLE, dwStyle);
			break;
		}
	}
	wndState = W_NULL;

	return 0;
}
LRESULT CALLBACK cWindow::wHandler(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_PAINT:
	{
		break;
	}
	case WM_SIZE: case WM_MOVE:
	{
		InvalidateRect(hw, NULL, TRUE);
		break;
	}
	default:
		return DefWindowProc(hw, msg, wp, lp);
	}
	return 0;
}

