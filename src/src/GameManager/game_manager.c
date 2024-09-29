#include "game_manager.h"
#include "../GameLoop/game_loop.h"

void gameManager_startGame()
{
	gameLoop_setCurrentStage(GS_INGAME);
}