#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "Macros.h"
#include "Entity.h"

class Player:Entity{
    public:
        SDL_Rect rect = {0,0,tile_size,tile_size*2};
        int hspd = 0;
        int vspd = 0;
        int spd = 4;
        const Uint8 *keyboard_state;
        Uint32 mouse_state;
        int mouse_x;
        int mouse_y;
        SDL_Event *event;
        Player(int *_camx,int *_camy,World *_wld,SDL_Renderer *_renderer):
        Entity(_camx,_camy,_wld,_renderer){};
        void handle_events(SDL_Event *event){
            switch(event->type){
                case SDL_MOUSEBUTTONDOWN:
                    
                    break;
            }
        }
        void draw(){
            SDL_SetRenderDrawColor(renderer,255,0,0,255);
            SDL_Rect _rect = {rect.x-*camx,rect.y-*camy,rect.w,rect.h};
            SDL_RenderFillRect(renderer,&_rect);
        }
        void update(float deltaTime) override{
            keyboard_state = SDL_GetKeyboardState(NULL);
            mouse_state = SDL_GetMouseState(&mouse_x,&mouse_y);
            if(mouse_state){
                std::vector<int> tile_pos = tile_at_point(*camx+mouse_x,*camy+mouse_y,wld->grid);
                Block &tile = wld->grid[tile_pos[0]][tile_pos[1]][tile_pos[2]][tile_pos[3]];
                if(mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT) && tile.id == BLOCKS_ID.VOID){
                    tile = Block(BLOCKS_ID.STONE);
                }
                if(mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT) && tile.id != BLOCKS_ID.VOID){
                    tile = Block(BLOCKS_ID.VOID);
                }
            }
            hspd = ((keyboard_state[SDL_SCANCODE_D]-keyboard_state[SDL_SCANCODE_A])*spd)*deltaTime;
            if(colliding_world({rect.x,rect.y+1,rect.w,rect.h},wld->grid)){
                vspd = 0;
                if(keyboard_state[SDL_SCANCODE_SPACE]){
                    vspd -= 10*deltaTime;
                }
            }else{
                vspd+=deltaTime;
            }
            if(hspd != 0 & vspd == 0){
                bool x = colliding_world({rect.x+sign(hspd),rect.y,rect.w,rect.h},wld->grid);
                bool y = !colliding_world({rect.x+sign(hspd),rect.y,rect.w,rect.h/2},wld->grid);
                bool z = !colliding_world({rect.x+sign(hspd),rect.y-tile_size,rect.w,rect.h},wld->grid);
                if(x & y & z){
                    rect.y-=tile_size;
                }
            }
            if(colliding_world({rect.x+hspd,rect.y,rect.w,rect.h},wld->grid)) {
                while(!colliding_world({rect.x+sign(hspd),rect.y,rect.w,rect.h},wld->grid)){
                    rect.x += sign(hspd);
                }
                hspd = 0;
            }
            rect.x += hspd;
            if(colliding_world({rect.x,rect.y+vspd,rect.w,rect.h},wld->grid)) {
                while(!colliding_world({rect.x,rect.y+sign(vspd),rect.w,rect.h},wld->grid)){
                    rect.y += sign(vspd);
                }
                vspd = 0;
            }
            rect.y += vspd;
        }
};

#endif
