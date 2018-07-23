

#include "Game.h"
#include "General.h"
#include <stdlib.h>
#include <stdbool.h>


int main(){

    Game_init();

    while(!General_isShuttingDown()){
        sleep_msec(100);
    }
    Game_cleanup();

    return 1;
}
