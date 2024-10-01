#include <stdio.h>

#include "GameLoop/game_loop.h"


#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif


int main()
{
    //_CrtSetBreakAlloc(3216);

    gameLoop_setCurrentStage(GS_INIT);
    gameLoop_loop();

#ifdef _CRTDBG_MAP_ALLOC
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}


