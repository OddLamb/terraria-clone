#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include "Macros.h"
#include "Player.h"
#include "SpriteSheet.h"

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
        
        world world_grid;
        int camx = 0;
        int camy = 0;
        Player *plr;

        int frame_count = 0;
        int fps = 0;
        Uint32 last_time = SDL_GetTicks();
        
        SpriteSheet *DirtTiles = new SpriteSheet(loadTexture("./assets/images/tileset-dirt.png",renderer),16,16,16);
        SpriteSheet *GrassTiles = new SpriteSheet(loadTexture("./assets/images/tileset-grass.png",renderer),16,16,16);
        SpriteSheet *StoneTiles = new SpriteSheet(loadTexture("./assets/images/tileset-stone.png",renderer),16,16,16);
        void draw(){
            SDL_SetRenderDrawColor(renderer,255,200,200,255);
            SDL_RenderClear(renderer);
            draw_world();
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
        void draw_world(){
            int start_chunk_x = camx / (chunk_wid * tile_size);
            int end_chunk_x = (camx + window_resolution[0]) / (chunk_wid * tile_size);
            int start_chunk_y = camy / (chunk_hei * tile_size);
            int end_chunk_y = (camy + window_resolution[1]) / (chunk_hei * tile_size);
            for (int x = std::max(0, start_chunk_x); x <= std::min(end_chunk_x, (int)world_grid.size() - 1); x++) {
                for (int y = std::max(0, start_chunk_y); y <= std::min(end_chunk_y, (int)world_grid[x].size() - 1); y++) {
                    SDL_Rect chunk_rect = {(x*(chunk_wid*tile_size))-camx,(y*(chunk_hei*tile_size))-camy,chunk_wid*tile_size,chunk_hei*tile_size};
                    chunk Chunk = world_grid[x][y];
                    for(int x1 = 0;x1<Chunk.size();x1++){
                        for(int y1 = 0;y1<Chunk[x1].size();y1++){
                            SDL_Rect tile_rect = {chunk_rect.x+x1*tile_size,chunk_rect.y+y1*tile_size,tile_size,tile_size};
                            if(inside_cam(tile_rect)){
                                if(Chunk[x1][y1] != BLOCKS_ID.VOID){
                                    int bitmask = 0;
                                    // top
                                    if (y1 > 0) {
                                        if(Chunk[x1][y1 - 1] != -1){
                                            bitmask |= 1;
                                        }
                                    }else if(y > 0){
                                        if(world_grid[x][y-1][x1][chunk_hei-1] != -1){
                                            bitmask |= 1;
                                        }
                                    }   
                                    // right
                                    if (x1 + 1 < chunk_wid) { 
                                        if(Chunk[x1 + 1][y1] != BLOCKS_ID.VOID){
                                            bitmask |= 2; 
                                        }
                                    }else if(x + 1 < world_wid){
                                        if(world_grid[x+1][y][0][y1] != BLOCKS_ID.VOID){
                                            bitmask |= 2; 
                                        }
                                    }
                                    // bottom
                                    if (y1 + 1 < chunk_hei) {
                                        if(Chunk[x1][y1 + 1] != BLOCKS_ID.VOID){ 
                                            bitmask |= 4;
                                        }
                                    }else if(y+1 < world_hei){
                                        if(world_grid[x][y+1][x1][0] != BLOCKS_ID.VOID){ 
                                            bitmask |= 4;
                                        }
                                    }
                                    // left
                                    if (x1 > 0) {
                                        if(Chunk[x1 - 1][y1] != BLOCKS_ID.VOID){ 
                                            bitmask |= 8; 
                                        }
                                    }else if(x > 0){
                                        if(world_grid[x - 1][y][chunk_wid-1][y1] != BLOCKS_ID.VOID){ 
                                            bitmask |= 8; 
                                        }
                                    }
                                    switch(Chunk[x1][y1]){
                                        case BLOCKS_ID.DIRT:
                                            DirtTiles->renderFrame(renderer,bitmask,tile_rect.x,tile_rect.y,tile_size,tile_size);
                                            break;
                                        case BLOCKS_ID.GRASS:   
                                            GrassTiles->renderFrame(renderer,bitmask,tile_rect.x,tile_rect.y,tile_size,tile_size);
                                            break;
                                        case BLOCKS_ID.STONE:
                                            StoneTiles->renderFrame(renderer,bitmask,tile_rect.x,tile_rect.y,tile_size,tile_size);
                                            break;
                                    }               
                                    //TileSpriteSheet->renderFrame(renderer,bitmask,tile_rect.x,tile_rect.y,tile_size,tile_size);
                                }
                            }
                        }
                    }
                    SDL_SetRenderDrawColor(renderer,255,0,0,255);
                    SDL_RenderDrawRect(renderer,&chunk_rect);
                }
            }
        }
        void init_world(){
            world_grid.resize(world_wid);
            for(int x = 0;x<world_grid.size();x++){
                world_grid[x].resize(world_hei);
                for(int y = 0;y<world_grid[x].size();y++){
                    world_grid[x][y].resize(chunk_wid);
                    for(int cx = 0;cx<world_grid[x][y].size();cx++){
                        world_grid[x][y][cx].resize(chunk_hei);
                        for(int cy = 0;cy<world_grid[x][y][cx].size();cy++){
                            world_grid[x][y][cx][cy] = BLOCKS_ID.VOID;
                        }
                    }
                }
            }
        }
        void generate_caves(std::vector<std::vector<int>> &stone_layer, int width, int height) {
            int fill_chance = 45;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    stone_layer[y][x] = (rand() % 100 < fill_chance) ? BLOCKS_ID.STONE : BLOCKS_ID.VOID; 
                }
            }

            int iterations = 5;
            for (int step = 0; step < iterations; step++) {
                std::vector<std::vector<int>> temp_layer = stone_layer;

                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        int wall_count = 0;

                        for (int dy = -1; dy <= 1; dy++) {
                            for (int dx = -1; dx <= 1; dx++) {
                                int nx = x + dx;
                                int ny = y + dy;

                                if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                                    wall_count++;
                                } else if (stone_layer[ny][nx] == BLOCKS_ID.VOID) {
                                    wall_count++;
                                }
                            }
                        }

                        if (wall_count >= 5) {
                            temp_layer[y][x] = BLOCKS_ID.STONE; // Becomes wall
                        } else {
                            temp_layer[y][x] = BLOCKS_ID.VOID; // Becomes empty
                        }
                    }
                }
                stone_layer = temp_layer; // Update the layer after smoothing
            }
        }

        void gen_world() {
            SDL_SetWindowTitle(window,"Generating World...");
            int chunk_y = (world_hei - 1) / 2;
            int rthei = chunk_hei / 2;

            for (int x = 0; x < world_grid.size(); x++) {
                for (int xx = 0; xx < world_grid[x][chunk_y].size(); xx++) {
                    // GRASS
                    world_grid[x][chunk_y][xx][rthei] = BLOCKS_ID.GRASS;
                    // DIRT
                    int dirt_hei_top = rthei + 1;
                    int dirt_hei_bottom = chunk_hei;
                    for (int y = chunk_y; y < (world_hei / 2) + 1; y++) {
                        for (int yy = dirt_hei_top; yy < dirt_hei_bottom; yy++) {
                            world_grid[x][y][xx][yy] = BLOCKS_ID.DIRT;
                        }
                        dirt_hei_top = 0;
                        dirt_hei_bottom = rthei;
                    }
                    // STONE 
                    int stone_hei_top = dirt_hei_bottom;
                    int stone_hei_bottom = chunk_hei;
                    for (int y = (world_hei/2); y < world_hei; y++){
                        for(int yy = stone_hei_top;yy<stone_hei_bottom;yy++){
                            world_grid[x][y][xx][yy] = BLOCKS_ID.STONE;
                        }
                        stone_hei_top = 0;
                    }
                    for (int y = (world_hei / 2)+1; y < world_hei; y++) {
                        chunk stone_layer(chunk_hei, std::vector<int>(chunk_wid));
                        generate_caves(stone_layer, chunk_wid, chunk_hei);
                        for (int yy = 0; yy < chunk_hei; yy++) {
                            for (int xx2 = 0; xx2 < chunk_wid; xx2++) {
                                world_grid[x][y][xx2][yy] = stone_layer[yy][xx2];
                            }
                        }
                    }

                    int rnd = choose({-1, 0, 0, 0, 0, 0, 0, 1});
                    if (rthei + rnd < 0) {
                        chunk_y--;
                        rthei = world_grid[x][chunk_y].size() - 1;
                    } else if (rthei + rnd > world_grid[x][chunk_y].size() - 1) {
                        chunk_y++;
                        rthei = 0;
                    } else {
                        rthei += rnd;
                    }
                }
            }
        }
        std::pair<int,int> middle_map(){
            for(int y = 0;y<world_grid[world_wid/2].size();y++){
                for(int yy = 0;yy<world_grid[world_wid/2][y][chunk_wid/2].size();yy++){
                    if(world_grid[world_wid/2][y][chunk_wid/2][yy] != -1){
                        return {(int(world_wid/2))*(chunk_wid*tile_size)+int(chunk_wid/2)*tile_size,(y*(chunk_hei*tile_size))+(yy-1)*tile_size};
                    }
                }
            }
            return {0,0};
        }
    public:
        void init(){
            init_world();
            gen_world();
            plr = new Player(&camx,&camy,&world_grid,renderer,keyboard_state,&mouse_state,&mouse_x,&mouse_y);
            plr->rect.x = middle_map().first;
            plr->rect.y = middle_map().second-plr->rect.h;
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
            delete DirtTiles;
            delete GrassTiles;
            delete StoneTiles;
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();
        }
};
#endif