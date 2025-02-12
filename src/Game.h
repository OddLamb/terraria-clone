#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "Macros.h"
#include "Player.h"
#include "SpriteSheet.h"
#include "World.h"

class Game{
    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Event event;

        bool running = true;
        const Uint8 *keyboard_state;
        int mouse_x;
        int mouse_y;
        Uint32 mouse_state;
        double delta_time = 1;
        Uint32 last_time;
        Uint32 current_time;
        Mix_Music *music;
        std::string curr_music;
        
        int camx = 0;
        int camy = 0;
        Player *plr;
        World *wld;

        TTF_Font *Sans;
        float sky_color[3] = {0.0f,0.0f,0.0f};
        void draw(){
            if(wld->timer[2] > 6 && wld->timer[2] < 17){
                sky_color[0] = lerp(sky_color[0],200,.01);
                sky_color[1] = lerp(sky_color[1],205,.01);
                sky_color[2] = lerp(sky_color[2],255,.01);
            }else{
                sky_color[0] = lerp(sky_color[0],0,.01);
                sky_color[1] = lerp(sky_color[1],0,.01);
                sky_color[2] = lerp(sky_color[2],0,.01);
            }
            SDL_SetRenderDrawColor(renderer, sky_color[0], sky_color[1], sky_color[2], 255);
            SDL_RenderClear(renderer);

            wld->draw();
            plr->draw();
            
            

            std::string text = "Time: " +std::to_string( wld->timer[2]) + ':' + std::to_string(wld->timer[1]) + ':' + std::to_string(wld->timer[0]);

            SDL_Surface* textSurface = TTF_RenderText_Shaded(Sans, text.c_str(), {0,0,0,255},{255,255,255,255});
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

            SDL_FreeSurface(textSurface);
            SDL_Rect renderQuad = { 0, 0, textSurface->w, textSurface->h }; 
            SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);

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
                                wld->init_grid();
                                wld->gen_grid();
                                plr->rect.y = 0;
                                plr->rect.x = 0;
                                wld->timer[0] = 0;
                                wld->timer[1] = 0;
                                wld->timer[2] = 12;
                                break;
                        }
                        break;

                }
                plr->handle_events(&event);
            }
        }
        void update(){
            handle_events();
            keyboard_state = SDL_GetKeyboardState(NULL);
            mouse_state = SDL_GetMouseState(&mouse_x,&mouse_y);
            
            if(wld->timer[2] > 6 && wld->timer[2] < 17 && curr_music != MUSIC_PATHS.DAYTIME_THEME){
                music = Mix_LoadMUS(MUSIC_PATHS.DAYTIME_THEME.c_str());
                Mix_PlayMusic(music,-1);
                curr_music = MUSIC_PATHS.DAYTIME_THEME;

            }else if((wld->timer[2] > 17 || wld->timer[2] < 6) && curr_music != MUSIC_PATHS.NIGHTTIME_THEME){
                music = Mix_LoadMUS(MUSIC_PATHS.NIGHTTIME_THEME.c_str());
                Mix_PlayMusic(music,-1);
                curr_music = MUSIC_PATHS.NIGHTTIME_THEME;
            }
            
            wld->update();
            plr->update(delta_time);

            camx = lerp(camx,plr->rect.x-(window_resolution[0]/2),.1);
            camy = lerp(camy,plr->rect.y-(window_resolution[1]/2),.1);
            camx = clamp(camx,world_wid*(chunk_wid*tile_size)-window_resolution[0],0);
            camy = clamp(camy,world_hei*(chunk_hei*tile_size)-window_resolution[1],0);
        }
    public:
        void init(){
            SDL_Init(SDL_INIT_EVERYTHING);
            TTF_Init();
            Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
            Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
        
            window = SDL_CreateWindow("Made by Asaf Amithiel",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,window_resolution[0],window_resolution[1],0);
            if(!window){
                std::cout << "Error initializing window: " << SDL_GetError() << "\n";
            }
            renderer = SDL_CreateRenderer(window,-1,0);
            if(!window){
                std::cout << "Error initializing renderer: " << SDL_GetError() << "\n";
            }
            keyboard_state = SDL_GetKeyboardState(NULL);
            mouse_state = SDL_GetMouseState(&mouse_x,&mouse_y);

            Sans = TTF_OpenFont("./assets/fonts/OpenSans.ttf",32);
            music = Mix_LoadMUS("./assets/tracks/forest-day.mp3");
            
            wld = new World(renderer,&camx,&camy);
            plr = new Player(&camx,&camy,wld,renderer);
            camx = plr->rect.x-window_resolution[0]/2;
            camy = plr->rect.y-window_resolution[1]/2;
        }
        void run(){
            last_time = SDL_GetPerformanceCounter();
            while(running){
                current_time = SDL_GetPerformanceCounter();
                delta_time = ((double)(current_time - last_time)/SDL_GetPerformanceFrequency())*100.0;
                last_time = current_time;
                Uint32 frame_start = SDL_GetTicks();
        
                update();
                draw();

                Uint32 frame_time = SDL_GetTicks() - frame_start;
                if (frame_delay > frame_time) {
                    SDL_Delay(frame_delay - frame_time);
                }
                
                
            }
        }
        void destroy(){
            wld->destroy();
            delete plr;
            delete wld;
            
            TTF_Quit();
            Mix_FreeMusic(music);
            Mix_Quit();
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();
        }
};
#endif