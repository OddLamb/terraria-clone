#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Macros.h"
#include "Player.h"
#include "SpriteSheet.h"
#include "World.h"

class Game{
    private:
        SDL_Window *window = SDL_CreateWindow("",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,window_resolution[0],window_resolution[1],0);
        SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
        SDL_Event event;

        bool running = true;
        const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
        int mouse_x;
        int mouse_y;
        Uint32 mouse_state = SDL_GetMouseState(&mouse_x,&mouse_y);
        
        int camx = 0;
        int camy = 0;
        Player *plr;
        World *wld;
        int frame_count = 0;
        int fps = 0;
        Uint32 last_time = SDL_GetTicks();

        void draw(){
            SDL_SetRenderDrawColor(renderer,200,200,255,255);
            SDL_RenderClear(renderer);
            wld->draw();
            plr->draw();
            SDL_RenderPresent(renderer);
        }
        void handle_events(){
            while(SDL_PollEvent(&event)){
                switch(event.type){
                    case SDL_QUIT:
                        running = false;
                        break;
                    case SDL_KEYDOWN:
                        switch(event.key.keysym.sym){
                            case SDLK_r:
                                init();
                                break;
                        }
                        break;

                }
                plr->handle_events(&event);
            }
        }
    public:
        void init(){
            wld = new World(renderer,&camx,&camy);
            plr = new Player(&camx,&camy,wld,renderer,keyboard_state,&mouse_state,&mouse_x,&mouse_y);
            camx = plr->rect.x-window_resolution[0]/2;
            camy = plr->rect.y-window_resolution[1]/2;
        }
        void run(){
            while(running){
                auto frame_start = SDL_GetTicks(); 
                handle_events();

                keyboard_state = SDL_GetKeyboardState(NULL);
                mouse_state = SDL_GetMouseState(&mouse_x,&mouse_y);

                plr->update();
                camx = lerp(camx,plr->rect.x-(window_resolution[0]/2),.1);
                camy = lerp(camy,plr->rect.y-(window_resolution[1]/2),.1);
                camx = clamp(camx,world_wid*(chunk_wid*tile_size)-window_resolution[0],0);
                camy = clamp(camy,world_hei*(chunk_hei*tile_size)-window_resolution[1],0);

                draw();
                auto frame_time = SDL_GetTicks() - frame_start;
                if (frame_delay > frame_time) {
                    SDL_Delay(frame_delay - frame_time);
                }
                frame_count++;
                Uint32 current_time = SDL_GetTicks();
                if (current_time - last_time >= 1000) { 
                    fps = frame_count;
                    frame_count = 0;
                    last_time = current_time;
                    char title[32];
                    snprintf(title,sizeof(title),"%d",fps);
                    SDL_SetWindowTitle(window,title);
                }
            }
        }
        void destroy(){
            delete plr;
            delete wld;
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();
        }
};
#endif