

#include "Game.h"
#include <stdlib.h>

int main(){
    //Initialize Game
    Game *game = CreateGame();
    if (game == NULL) return 0;

    //Begin
    Play(game);
    //Finalize Game
    DeleteGame(&game);



    return 1;
}
