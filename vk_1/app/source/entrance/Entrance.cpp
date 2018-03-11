#include "Entrance.hpp"

int main()
{
	WNDCLASSEX wClass = { 0 };
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.style = CS_HREDRAW | CS_VREDRAW;
	wClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hIcon = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 48, 48, LR_LOADFROMFILE);
	wClass.hIconSm = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	wClass.lpszClassName = "cNone";
	wClass.lpszMenuName = "Window_1";
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;

	cWindow wnd(wClass, WS_VISIBLE);

	cGraphics *g = new cGraphics(wnd.getHWND(), wnd.getHInstance());
	sData *data = g->getData();

	while (true)
	{
		g->drawFrame();
	}

	return 0;
}