#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    SDL_Window* window = SDL_CreateWindow(
        "SDL2 Test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event event;
    
    while (!quit)
    {
        SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);
        SDL_RenderClear(renderer);

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;

                default:
                    break;
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}