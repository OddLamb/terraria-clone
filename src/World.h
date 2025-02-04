#ifndef WORLD_H
#define WORLD_H
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "Macros.h"
#include "SpriteSheet.h"

class World{
    SDL_Renderer *renderer;
    int *camx;
    int *camy; 
    std::vector<SpriteSheet*> TileSetList;
    public:
        world grid;
        void init_grid(){
            grid.resize(world_wid);
            for(int x = 0;x<grid.size();x++){
                grid[x].resize(world_hei);
                for(int y = 0;y<grid[x].size();y++){
                    grid[x][y].resize(chunk_wid);
                    for(int cx = 0;cx<grid[x][y].size();cx++){
                        grid[x][y][cx].resize(chunk_hei);
                        for(int cy = 0;cy<grid[x][y][cx].size();cy++){
                            grid[x][y][cx][cy] = BLOCKS_ID.VOID;
                        }
                    }
                }
            }
        }
        void generate_caves(chunk &stone_layer, int width, int height) {
            int fill_chance = 45;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    stone_layer[y][x] = (random_range(0,100) < fill_chance) ? Block(BLOCKS_ID.STONE) : Block(BLOCKS_ID.VOID);
                }
            }

            int iterations = 5;
            for (int step = 0; step < iterations; step++) {
                chunk temp_layer = stone_layer;

                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        int wall_count = 0;

                        for (int dy = -1; dy <= 1; dy++) {
                            for (int dx = -1; dx <= 1; dx++) {
                                int nx = x + dx;
                                int ny = y + dy;

                                if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                                    wall_count++;
                                } else if (stone_layer[ny][nx].id == BLOCKS_ID.VOID) {
                                    wall_count++;
                                }
                            }
                        }

                        if (wall_count >= 5) {
                            temp_layer[y][x] = Block(BLOCKS_ID.STONE); 
                        } else {
                            temp_layer[y][x] = Block(BLOCKS_ID.VOID);
                        }
                    }
                }
                stone_layer = temp_layer; 
            }
        }

        void gen_grid() {
            int chunk_y = (world_hei - 1) / 2;
            int rthei = chunk_hei / 2;

            for (int x = 0; x < grid.size(); x++) {
                for (int xx = 0; xx < grid[x][chunk_y].size(); xx++) {
                    // GRASS
                    grid[x][chunk_y][xx][rthei] = Block(BLOCKS_ID.GRASS);
                    // DIRT
                    int dirt_hei_top = rthei + 1;
                    int dirt_hei_bottom = chunk_hei;
                    for (int y = chunk_y; y < (world_hei / 2) + 1; y++) {
                        for (int yy = dirt_hei_top; yy < dirt_hei_bottom; yy++) {
                            grid[x][y][xx][yy] = Block(BLOCKS_ID.DIRT);
                        }
                        dirt_hei_top = 0;
                        dirt_hei_bottom = rthei;
                    }
                    // STONE 
                    int stone_hei_top = dirt_hei_bottom;
                    int stone_hei_bottom = chunk_hei;
                    for (int y = (world_hei/2); y < world_hei; y++){
                        for(int yy = stone_hei_top;yy<stone_hei_bottom;yy++){
                            grid[x][y][xx][yy] = Block(BLOCKS_ID.STONE);
                        }
                        stone_hei_top = 0;
                    }
                    for (int y = (world_hei / 2)+1; y < world_hei; y++) {
                        chunk stone_layer(chunk_hei, std::vector<Block>(chunk_wid));
                        generate_caves(stone_layer, chunk_wid, chunk_hei);
                        for (int yy = 0; yy < chunk_hei; yy++) {
                            for (int xx2 = 0; xx2 < chunk_wid; xx2++) {
                                grid[x][y][xx2][yy] = stone_layer[yy][xx2];
                            }
                        }
                    }

                    int rnd = choose({-1, 0, 0, 0, 0, 0, 0, 1});
                    if (rthei + rnd < 0) {
                        chunk_y--;
                        rthei = grid[x][chunk_y].size() - 1;
                    } else if (rthei + rnd > grid[x][chunk_y].size() - 1) {
                        chunk_y++;
                        rthei = 0;
                    } else {
                        rthei += rnd;
                    }
                }
            }
        }
        void convert_pos(int &chunk_x,int &chunk_y,int &tile_x,int &tile_y){
            int tx = chunk_x;
            int ty = chunk_y;
            int txx = tile_x;
            int tyy = tile_y;
            if (txx >= chunk_wid) {  // Check if txx exceeds the chunk width
                txx = 0; // Wrap to the start of the next chunk
                tx += 1; // Move to the next chunk
            } else if (txx < 0) { // Check if txx is less than 0
                txx = chunk_wid - 1; // Wrap to the end of the previous chunk
                tx -= 1; // Move to the previous chunk
            }
            if (tyy >= chunk_hei) { // Check if tyy exceeds the chunk height
                tyy = 0; // Wrap to the start of the next chunk
                ty += 1; // Move to the next chunk
            } else if (tyy < 0) { // Corrected: Check if tyy is less than 0
                tyy = chunk_hei - 1; // Wrap to the end of the previous chunk
                ty -= 1; // Move to the previous chunk
            }
            tx = clamp(tx,world_wid,0);
            ty = clamp(ty,world_hei,0);
            chunk_x = tx;
            chunk_y = ty;
            tile_x = txx;
            tile_y = tyy;
        }
        World(SDL_Renderer *_renderer,int *_camx,int *_camy):renderer(_renderer), camx(_camx), camy(_camy){
            TileSetList.push_back(new SpriteSheet(loadTexture("./assets/images/tileset-grass.png",renderer),16,16,16));
            TileSetList.push_back(new SpriteSheet(loadTexture("./assets/images/tileset-dirt.png",renderer),16,16,16));
            TileSetList.push_back(new SpriteSheet(loadTexture("./assets/images/tileset-stone.png",renderer),16,16,16));
            TileSetList.push_back(new SpriteSheet(loadTexture("./assets/images/tileset-water.png",renderer),16,16,16));
            init_grid();
            gen_grid();
        }
        void transfer_life(Block &from, Block &to){
            to.life+=from.life;
            from.life=0;
            if(to.life > BLOCK_MAX_LIFE){
                int excess = BLOCK_MAX_LIFE-to.life;
                to.life -= excess;
                from.life += excess;
            }else{
                from = Block(BLOCKS_ID.VOID);
            }
        }
        void update(){
            int start_chunk_x = *camx / (chunk_wid * tile_size);
            int end_chunk_x = (*camx + window_resolution[0]) / (chunk_wid * tile_size);
            int start_chunk_y = *camy / (chunk_hei * tile_size);
            int end_chunk_y = (*camy + window_resolution[1]) / (chunk_hei * tile_size);
            
            for (int x = std::max(0, start_chunk_x); x <= std::min(end_chunk_x, (int)grid.size() - 1); x++) {
                for (int y = std::max(0, start_chunk_y); y <= std::min(end_chunk_y, (int)grid[x].size() - 1); y++) {
                    SDL_Rect chunk_rect = {(x*(chunk_wid*tile_size))-*camx,(y*(chunk_hei*tile_size))-*camy,chunk_wid*tile_size,chunk_hei*tile_size};
                    int start_tile_x = std::max(0, (*camx - (x * chunk_wid * tile_size)) / tile_size);
                    int end_tile_x = std::min(chunk_wid - 1, (*camx + window_resolution[0] - (x * chunk_wid * tile_size)) / tile_size);
                    int start_tile_y = std::max(0, (*camy - (y * chunk_hei * tile_size)) / tile_size);
                    int end_tile_y = std::min(chunk_hei - 1, (*camy + window_resolution[1] - (y * chunk_hei * tile_size)) / tile_size);
                    
                    // Loop through visible tiles within the chunk
                    for (int x1 = start_tile_x; x1 <= end_tile_x; x1++) {
                        for (int y1 = start_tile_y; y1 <= end_tile_y; y1++) {
                            SDL_Rect tile_rect = {chunk_rect.x+x1*tile_size,chunk_rect.y+y1*tile_size,tile_size,tile_size};
                            if(grid[x][y][x1][y1].id != BLOCKS_ID.VOID){
                                int bitmask = 0;
                                // top
                                if (y1 > 0) {
                                    if(grid[x][y][x1][y1 - 1].id != BLOCKS_ID.VOID){
                                        bitmask |= 1;
                                    }
                                }else if(y > 0){
                                    if(grid[x][y-1][x1][chunk_hei-1].id != BLOCKS_ID.VOID){
                                        bitmask |= 1;
                                    }
                                }   
                                // right
                                if (x1 + 1 < chunk_wid) { 
                                    if(grid[x][y][x1 + 1][y1].id != BLOCKS_ID.VOID){
                                        bitmask |= 2; 
                                    }
                                }else if(x + 1 < world_wid){
                                    if(grid[x+1][y][0][y1].id != BLOCKS_ID.VOID){
                                        bitmask |= 2; 
                                    }
                                }
                                // bottom
                                if (y1 + 1 < chunk_hei) {
                                    if(grid[x][y][x1][y1 + 1].id != BLOCKS_ID.VOID){ 
                                        bitmask |= 4;
                                    }
                                }else if(y+1 < world_hei){
                                    if(grid[x][y+1][x1][0].id != BLOCKS_ID.VOID){ 
                                        bitmask |= 4;
                                    }
                                }
                                // left
                                if (x1 > 0) {
                                    if(grid[x][y][x1 - 1][y1].id != BLOCKS_ID.VOID){ 
                                        bitmask |= 8; 
                                    }
                                }else if(x > 0){
                                    if(grid[x - 1][y][chunk_wid-1][y1].id != BLOCKS_ID.VOID){ 
                                        bitmask |= 8; 
                                    }
                                }
                                grid[x][y][x1][y1].tile = bitmask;
                                std::vector<int> dist_list;
                                for(int l = 1;l<=LEN_LIGHT;l++){
                                    for(int j = 1;j<=DIR_LIGHT*l;j++){
                                        int tx = x;
                                        int ty = y;
                                        double radian = j * (M_PI * 2) / (DIR_LIGHT*l);
                                        
                                        int txx = x1 + (std::round(std::cos(radian) * l));
                                        int tyy = y1 + (std::round(std::sin(radian) * l));
                                        
                                        convert_pos(tx,ty,txx,tyy);
                                        if(grid[tx][ty][txx][tyy].solid == false){
                                            int xt1 = (x*(chunk_wid))+x1;
                                            int yt1 = (y*(chunk_hei))+y1;
                                            int xt2 = (tx*(chunk_wid))+txx;
                                            int yt2 = (ty*(chunk_hei))+tyy;
                                            dist_list.push_back(distance(xt1,yt1,xt2,yt2));
                                        }
                                    }
                                }
                                if(!dist_list.empty()){
                                    grid[x][y][x1][y1].light = 255/(*std::min_element(dist_list.begin(),dist_list.end()));
                                }else{
                                    grid[x][y][x1][y1].light = 0;
                                }
                                if(grid[x][y][x1][y1].tick > 0){
                                    grid[x][y][x1][y1].tick--;
                                }else{
                                    grid[x][y][x1][y1].tick = grid[x][y][x1][y1].timer;
                                }
                                if (grid[x][y][x1][y1].id == BLOCKS_ID.WATER && grid[x][y][x1][y1].tick == 0) {
                                    Block &water_tile = grid[x][y][x1][y1];
                                    int dtx = x;
                                    int dty = y;
                                    int dtxx = x1;
                                    int dtyy = y1+1;
                                    convert_pos(dtx,dty,dtxx,dtyy);
                                    Block &block_down = grid[dtx][dty][dtxx][dtyy];
                                    // if can go down
                                    if(block_down.id == BLOCKS_ID.VOID){
                                        block_down = water_tile;
                                        water_tile = Block(BLOCKS_ID.VOID);
                                    }else if(block_down.id == BLOCKS_ID.WATER && block_down.life < BLOCK_MAX_LIFE){
                                        transfer_life(water_tile,block_down);
                                    }else if(water_tile.life > 2){
                                        int ltx = x;
                                        int ltxx = x1-1;
                                        int rtx = x;
                                        int rtxx = x1+1;
                                        convert_pos(ltx,y,ltxx,y1);
                                        convert_pos(rtx,y,rtxx,y1);
                                        Block &left_tile = grid[ltx][y][ltxx][y1];
                                        Block &right_tile = grid[rtx][y][rtxx][y1];
                                        if(left_tile.id == BLOCKS_ID.VOID){ // if can go left
                                            left_tile = Block(BLOCKS_ID.WATER);
                                            left_tile.life = water_tile.life-2;
                                        }else if(left_tile.id == BLOCKS_ID.WATER){
                                            transfer_life(water_tile,left_tile);
                                        }
                                        if(right_tile.id == BLOCKS_ID.VOID){ // if can go right
                                            right_tile = Block(BLOCKS_ID.WATER);
                                            right_tile.life = water_tile.life-2;
                                        }else if(right_tile.id == BLOCKS_ID.WATER){
                                            transfer_life(water_tile,right_tile);
                                        }    
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        void draw(){
            
            update();
            int start_chunk_x = *camx / (chunk_wid * tile_size);
            int end_chunk_x = (*camx + window_resolution[0]) / (chunk_wid * tile_size);
            int start_chunk_y = *camy / (chunk_hei * tile_size);
            int end_chunk_y = (*camy + window_resolution[1]) / (chunk_hei * tile_size);
            
            for (int x = std::max(0, start_chunk_x); x <= std::min(end_chunk_x, (int)grid.size() - 1); x++) {
                for (int y = std::max(0, start_chunk_y); y <= std::min(end_chunk_y, (int)grid[x].size() - 1); y++) {
                    
                    SDL_Rect chunk_rect = {(x*(chunk_wid*tile_size))-*camx,(y*(chunk_hei*tile_size))-*camy,chunk_wid*tile_size,chunk_hei*tile_size};
                    int start_tile_x = std::max(0, (*camx - (x * chunk_wid * tile_size)) / tile_size);
                    int end_tile_x = std::min(chunk_wid - 1, (*camx + window_resolution[0] - (x * chunk_wid * tile_size)) / tile_size);
                    int start_tile_y = std::max(0, (*camy - (y * chunk_hei * tile_size)) / tile_size);
                    int end_tile_y = std::min(chunk_hei - 1, (*camy + window_resolution[1] - (y * chunk_hei * tile_size)) / tile_size);
                    
                    // Loop through visible tiles within the chunk
                    for (int x1 = start_tile_x; x1 <= end_tile_x; x1++) {
                        for (int y1 = start_tile_y; y1 <= end_tile_y; y1++) {
                            SDL_Rect tile_rect = {chunk_rect.x+x1*tile_size,chunk_rect.y+y1*tile_size,tile_size,(grid[x][y][x1][y1].life * tile_size) / BLOCK_MAX_LIFE};                    
                            tile_rect.y-=(tile_rect.h-tile_size);
                            if(grid[x][y][x1][y1].id != BLOCKS_ID.VOID){
                                TileSetList[grid[x][y][x1][y1].id]->renderFrame(renderer,grid[x][y][x1][y1].tile,tile_rect.x,tile_rect.y,tile_rect.w,tile_rect.h,0,SDL_FLIP_NONE,grid[x][y][x1][y1].light,grid[x][y][x1][y1].light,grid[x][y][x1][y1].light);
                            }
                        }
                    }
                }
            }
        }
    
};
#endif
