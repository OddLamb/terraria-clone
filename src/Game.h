#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <thread>
#include "Macros.h"
#include "Player.h"
#include "SpriteSheet.h"
#include "World.h"

class Game{
    private:
        SDL_Window *window = nullptr;
        SDL_Renderer *renderer = nullptr;
        SDL_Event event;

        bool running = true;
        const Uint8 *keyboard_state = nullptr;
        int mouse_x;
        int mouse_y;
        Uint32 mouse_state;
        double delta_time = 1;
        Uint32 last_time;
        Uint32 current_time;
        Mix_Music *music = nullptr;
        std::string curr_music;
        
        int camx = 0;
        int camy = 0;
        std::vector<Entity*> EntityList; 
        World *wld = nullptr;

        TTF_Font *Sans = nullptr;
        float sky_color[3] = {0.0f,0.0f,0.0f};
        Entity *camTarget;
        void draw(){
            if(wld->isDay()){
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
            for(auto &entity : EntityList){
                entity->draw();
            }
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
                                wld->gen_grid();
                                break;
                        }
                        break;

                }
                for(auto &entity : EntityList){
                    entity->handle_events(&event);
                }
            }
        }
        void update(){
            handle_events();
            keyboard_state = SDL_GetKeyboardState(NULL);
            mouse_state = SDL_GetMouseState(&mouse_x,&mouse_y);
            
            if(wld->isDay() && curr_music != MUSIC_PATHS.DAYTIME_THEME){
                if (music) Mix_FreeMusic(music);
                music = Mix_LoadMUS(MUSIC_PATHS.DAYTIME_THEME.c_str());
                Mix_PlayMusic(music,-1);
                curr_music = MUSIC_PATHS.DAYTIME_THEME;
            }else if(wld->isNight() && curr_music != MUSIC_PATHS.NIGHTTIME_THEME){
                if (music) Mix_FreeMusic(music);
                music = Mix_LoadMUS(MUSIC_PATHS.NIGHTTIME_THEME.c_str());
                Mix_PlayMusic(music,-1);
                curr_music = MUSIC_PATHS.NIGHTTIME_THEME;
            }
            
            std::thread WorldThread(&World::update, wld);
            WorldThread.detach();

            for(auto &entity : EntityList){
                entity->update(delta_time);
            }
            if(keyboard_state[SDL_SCANCODE_E]){
                wld->timer[1]+=6;
            }
            
            camx = lerp(camx,camTarget->rect.x-(window_resolution[0]/2),.1);
            camy = lerp(camy,camTarget->rect.y-(window_resolution[1]/2),.1);
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
            int middle_world = std::round(world_wid/2);
            int middle_chunk = std::round(chunk_wid/2);
            
            EntityList.push_back(new Player(&camx,&camy,wld,renderer));
            camTarget = EntityList.back();
            camx = camTarget->rect.x-window_resolution[0]/2;
            camy = camTarget->rect.y-window_resolution[1]/2;
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
            for(auto &entity : EntityList){
                entity->destroy();
                delete entity;
            }
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