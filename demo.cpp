#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <stdlib.h>

#include "IMG.h"
#include "Scene.h"
#include "Overworld.h"

#include "Game.h"

//time per frame in ms
const int DELAY         = 25;

Game::Container*    gGame;

int main( int argc, char* args[] )
{
    //Main loop flag
    bool quit = false;
    gGame    = new Game::Container();

    //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
    //While application is running
    int now;
    int delta_t;

    while( !quit )
    {
        now = SDL_GetTicks();

        quit = gGame->Update(DELAY);
        delta_t = now + DELAY - SDL_GetTicks();
        if(delta_t > 0)
            SDL_Delay(delta_t);
        //Use proper timing; lazy foo' sdl guide should give some insight...
    }

	//}
	printf("nuke everything\n\n");
    delete gGame;

	return 0;
}
