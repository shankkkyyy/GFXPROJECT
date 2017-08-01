#include "CubeApp.h"


int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // defined(DEBUG) | defined(_DEBUG)


	CubeApp App(_hInstance);
	
	if (App.InitApp())
	{
		return App.Run();
	}
	else return 0;

}