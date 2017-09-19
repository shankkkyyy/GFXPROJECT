#include "Engine.h"


int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _prevInstance, PSTR cmdLine, int showCmd)
{
	Engine App(_hInstance);

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetBreakAlloc(-1);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // defined(DEBUG) | defined(_DEBUG)
	
	if (App.InitApp())
	{
		return App.Run();
	}
	else return 0;

}