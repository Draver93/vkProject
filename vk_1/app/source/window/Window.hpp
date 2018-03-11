/*
Реализация класса по работе с окном основанная на WinAPI.
Разработчик Финошкин Алексей.
*/

#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>
#include <string>
#include <iostream>
#include <thread>

enum WSTATE { W_NULL, W_RECREATE, W_EDIT, W_CLOSE };

class cWindow
{
private:
	UINT wId;
	MSG msg;
	HWND hWindow;
	WNDCLASSEX wState;
	std::thread *wThread;
	static UINT gWndIndices;

	WSTATE wndState;
	bool lock;

	POINT wPos, wSize;
	bool cursorState;
	DWORD dwStyle;

	bool ready;

	HRESULT create();
	int windowLoop();
	int nextQueue();
	static LRESULT CALLBACK wHandler(HWND hw, UINT msg, WPARAM wp, LPARAM lp);
public:
	cWindow(WNDCLASSEX wc, DWORD style, POINT position = POINT{ 0, 0 }, POINT size = POINT{ 800, 600 });

	~cWindow();

	HRESULT reСreate(WNDCLASSEX wc, DWORD style, POINT position = POINT{ 0, 0 }, POINT size = POINT{ 800, 600 });
	HRESULT edit(DWORD style);
	HRESULT edit(POINT pos , POINT size);

	MSG* getMessage() { return &msg; }
	HWND getHWND() { return hWindow; }
	HINSTANCE getHInstance() { return wState.hInstance; }

	bool isReady() { return ready; }

	WSTATE getState() { return wndState; }
};


#endif //WINDOW_H