#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>

#include <string>
#include "Core/SceneManager.h"
#include "Utils/Debug.h"

int main(int argc, char* args[]) {
	static_assert(sizeof(void*) == 4, "This program is not ready for 64-bit build");

	Debug::DebugInit("GameEngineLog.txt");
	
	SceneManager* gsm = new SceneManager();
	if (gsm->Initialize("Game Engine", 1920, 1080) ==  true) {
		gsm->Run();
	} 
	delete gsm;
	_CrtDumpMemoryLeaks();
	exit(0);
}
