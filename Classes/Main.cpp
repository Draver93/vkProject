#include "Main.hpp"


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

	cWindow wnd(wClass, WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	cGraphic *grp = new cGraphic(&wnd);
	assert(grp->initInstance());
	grp->setupDebugCallback();
	assert(grp->initSurface());
	assert(grp->initPhysicalDevice());
	assert(grp->initLogicalDevice());
	assert(grp->initSwapChain());

	//grp->initDeviceQueue();
	//grp->initFramebuffer();
	delete grp;
	while (true) 
	{
		//grp->presentStep();
	}

	return 0;
}