#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "Macros.h"
#include "Entity.h"

class Player:Entity{
    public:
        SDL_Rect rect = {0,0,tile_size*2,tile_size*3};
        int hspd = 0;
        int vspd = 0;
        int spd = 4;
        Uint32 *mouse_state;
        const Uint8 *keyboard_state;
        SDL_Event *event;
        int *mouse_x = 0;
        int *mouse_y = 0;
        Player(int *_camx,int *_camy,world *_world_grid,SDL_Renderer *_renderer, const Uint8 *_keyboard_state,Uint32 *_mouse_state,int *_mouse_x, int *_mouse_y):
        Entity(_camx,_camy,_world_grid,_renderer){
            keyboard_state = _keyboard_state;
            mouse_state = _mouse_state;
            mouse_x = _mouse_x;
            mouse_y = _mouse_y;
        }
        void handle_events(SDL_Event *event){
        }
        void draw(){
            SDL_SetRenderDrawColor(renderer,255,0,0,255);
            SDL_Rect _rect = {rect.x-*camx,rect.y-*camy,rect.w,rect.h};
            SDL_RenderFillRect(renderer,&_rect);
        }
        void update(){
            if((*mouse_state) & SDL_BUTTON(SDL_BUTTON_LEFT)){
                std::vector<int> tile = tile_at_point(*mouse_x+*camx,*mouse_y+*camy,*world_grid);
                if((*world_grid)[tile[0]][tile[1]][tile[2]][tile[3]] != BLOCKS_ID.VOID){
                    (*world_grid)[tile[0]][tile[1]][tile[2]][tile[3]] = BLOCKS_ID.VOID;
                }
            }else if((*mouse_state) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
                std::vector<int> tile = tile_at_point(*mouse_x+*camx,*mouse_y+*camy,*world_grid);
                if((*world_grid)[tile[0]][tile[1]][tile[2]][tile[3]] == BLOCKS_ID.VOID){
                    (*world_grid)[tile[0]][tile[1]][tile[2]][tile[3]] = BLOCKS_ID.STONE;
                }
            }
            hspd = (keyboard_state[SDL_SCANCODE_D]-keyboard_state[SDL_SCANCODE_A])*spd;
            if(colliding_world({rect.x,rect.y+1,rect.w,rect.h},*world_grid)){
                vspd = 0;
                if(keyboard_state[SDL_SCANCODE_SPACE]){
                    vspd -= 16;
                }
            }else{
                vspd++;
            }
            if(hspd != 0){
                bool x = colliding_world({rect.x+sign(hspd),rect.y,rect.w,rect.h},*world_grid);
                bool y = !colliding_world({rect.x+sign(hspd),rect.y,rect.w,rect.h/2},*world_grid);
                bool z = !colliding_world({rect.x+sign(hspd),rect.y-tile_size,rect.w,rect.h},*world_grid);
                if(x & y & z){
                    rect.y-=tile_size;
                }
            }
            if(colliding_world({rect.x+hspd,rect.y,rect.w,rect.h},*world_grid)) {
                while(!colliding_world({rect.x+sign(hspd),rect.y,rect.w,rect.h},*world_grid)){
                    rect.x += sign(hspd);
                }
                hspd = 0;
            }
            rect.x += hspd;
            if(colliding_world({rect.x,rect.y+vspd,rect.w,rect.h},*world_grid)) {
                while(!colliding_world({rect.x,rect.y+sign(vspd),rect.w,rect.h},*world_grid)){
                    rect.y += sign(vspd);
                }
                vspd = 0;
            }
            rect.y += vspd;
        }
};

#endif
