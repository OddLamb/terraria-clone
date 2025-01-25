#include "Game.h"

int main(){
    Game *game = new Game();
    game->init();
    game->run();
    game->destroy();
    delete game;
    return 0;
}