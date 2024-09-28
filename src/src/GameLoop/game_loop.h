#ifndef GAME_LOOP_H
#define GAME_LOOP_H

enum GameStage {
	GS_INIT,
	GS_DEINIT,
	GS_MENU,
	GS_INGAME
};
typedef enum GameStage GameStage;

//pwindow: GLFWwindow*
void gameLoop_loop();

void gameLoop_setCurrentStage(GameStage stage);


#endif