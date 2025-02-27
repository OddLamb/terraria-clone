#ifndef WORLD_H
#define WORLD_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "Macros.h"
#include "SpriteSheet.h"

class World{
    private:
        SDL_Renderer *renderer = nullptr;
        int *camx = nullptr;
        int *camy = nullptr; 
        std::vector<SpriteSheet*> TileSetList;
        SpriteSheet *sprMoon = nullptr;
        SpriteSheet *sprSun = nullptr;
        SpriteSheet *layer_0 = nullptr;
        SpriteSheet *layer_1 = nullptr;
        SpriteSheet *layer_2 = nullptr;
        double astros_angle = 0;
        double calculateAngle() {
            double hourAngle = (timer[2] * (M_PI * 2) / 24) + 
                            (timer[1] * (M_PI * 2) / (24 * 60)) + 
                            (timer[0] * (M_PI * 2) / (24 * 60 * 60));
            double angle = hourAngle+M_PI/2; 
            angle = fmod(angle, M_PI * 2);
            if (angle < 0) angle += M_PI * 2;
            return angle;
        }
        int prev_camx = *camx;
        int camx_vel = 0;
    public:
        world grid;
        int timer[3] = {0,0,12};
        World(SDL_Renderer *_renderer,int *_camx,int *_camy):renderer(_renderer), camx(_camx), camy(_camy){
            TileSetList.push_back(new SpriteSheet(loadTexture("./assets/images/tilesets/tileset-grass.png",renderer),16,16,16));
            TileSetList.push_back(new SpriteSheet(loadTexture("./assets/images/tilesets/tileset-dirt.png",renderer),16,16,16));
            TileSetList.push_back(new SpriteSheet(loadTexture("./assets/images/tilesets/tileset-stone.png",renderer),16,16,16));
            TileSetList.push_back(new SpriteSheet(loadTexture("./assets/images/tilesets/tileset-water.png",renderer),16,16,16));
            sprMoon = new SpriteSheet(loadTexture("./assets/images/background/moon.png",renderer),64,64,1);
            sprSun = new SpriteSheet(loadTexture("./assets/images/background/sun.png",renderer),64,64,1);
            layer_0 = new SpriteSheet(loadTexture("./assets/images/background/forest/layer-0.png",renderer),512,320,1);
            layer_1 = new SpriteSheet(loadTexture("./assets/images/background/forest/layer-1.png",renderer),512,320,1);
            layer_2 = new SpriteSheet(loadTexture("./assets/images/background/forest/layer-2.png",renderer),512,320,1);
            gen_grid();
        }
        void reset_grid(){
            grid = world(
                world_wid, std::vector<std::vector<std::vector<Block>>>(
                    world_hei, chunk(
                        chunk_wid, std::vector<Block>(
                            chunk_hei, Block(BLOCK_ID::VOID)))));      
        }
        chunk generate_caves() {
            chunk stone_layer(chunk_hei, std::vector<Block>(chunk_wid));

            int fill_chance = 45;
            for (int y = 0; y < chunk_hei; y++) {
                for (int x = 0; x < chunk_wid; x++) {
                    stone_layer[y][x] = (random_range(0,100) < fill_chance) ? Block(BLOCK_ID::STONE) : Block(BLOCK_ID::VOID);
                }
            }

            int iterations = 5;
            for (int step = 0; step < iterations; step++) {
                chunk temp_layer = stone_layer;

                for (int y = 0; y < chunk_hei; y++) {
                    for (int x = 0; x < chunk_wid; x++) {
                        int wall_count = 0;

                        for (int dy = -1; dy <= 1; dy++) {
                            for (int dx = -1; dx <= 1; dx++) {
                                int nx = x + dx;
                                int ny = y + dy;

                                if (nx < 0 || nx >= chunk_wid || ny < 0 || ny >= chunk_hei) {
                                    wall_count++;
                                } else if (stone_layer[ny][nx].id == BLOCK_ID::VOID) {
                                    wall_count++;
                                }
                            }
                        }

                        if (wall_count >= 5) {
                            temp_layer[y][x] = Block(BLOCK_ID::STONE); 
                        } else {
                            temp_layer[y][x] = Block(BLOCK_ID::VOID);
                        }
                    }
                }
                stone_layer = temp_layer; 
            }
            return stone_layer;
        }
        void gen_grid() {
            reset_grid();
            int chunk_y = (world_hei - 1) / 2;
            int rthei = chunk_hei / 2;

            for (int x = 0; x < grid.size(); x++) {
                for (int xx = 0; xx < grid[x][chunk_y].size(); xx++) {
                    // GRASS
                    grid[x][chunk_y][xx][rthei] = Block(BLOCK_ID::GRASS);
                    // DIRT
                    int dirt_hei_top = rthei + 1;
                    int dirt_hei_bottom = chunk_hei;
                    for (int y = chunk_y; y < (world_hei / 2) + 1; y++) {
                        for (int yy = dirt_hei_top; yy < dirt_hei_bottom; yy++) {
                            grid[x][y][xx][yy] = Block(BLOCK_ID::DIRT);
                        }
                        dirt_hei_top = 0;
                        dirt_hei_bottom = rthei;
                    }
                    // STONE 
                    int stone_hei_top = dirt_hei_bottom;
                    int stone_hei_bottom = chunk_hei;
                    for (int y = (world_hei/2); y < world_hei; y++){
                        for(int yy = stone_hei_top;yy<stone_hei_bottom;yy++){
                            grid[x][y][xx][yy] = Block(BLOCK_ID::STONE);
                        }
                        stone_hei_top = 0;
                    }
                    
                    for (int y = chunk_y+2; y < world_hei; y++) {
                        grid[x][y] = generate_caves();
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
        void convert_pos(int &chunk_x, int &chunk_y, int &tile_x, int &tile_y) {
            // Adjust tile_x and chunk_x
            if (tile_x >= chunk_wid) {
                chunk_x += tile_x / chunk_wid; // Move to the next chunk(s)
                tile_x = tile_x % chunk_wid;  // Keep the remainder as the new tile_x
            } else if (tile_x < 0) {
                chunk_x -= (-tile_x + chunk_wid - 1) / chunk_wid; // Move to the previous chunk(s)
                tile_x = chunk_wid - (-tile_x % chunk_wid);       // Adjust tile_x to wrap around
            }

            // Adjust tile_y and chunk_y
            if (tile_y >= chunk_hei) {
                chunk_y += tile_y / chunk_hei; // Move to the next chunk(s)
                tile_y = tile_y % chunk_hei;  // Keep the remainder as the new tile_y
            } else if (tile_y < 0) {
                chunk_y -= (-tile_y + chunk_hei - 1) / chunk_hei; // Move to the previous chunk(s)
                tile_y = chunk_hei - (-tile_y % chunk_hei);       // Adjust tile_y to wrap around
            }

            // Clamp chunk coordinates to world bounds
            chunk_x = clamp(chunk_x, world_wid - 1, 0);
            chunk_y = clamp(chunk_y, world_hei - 1, 0);
        }
        bool isDay(){
            return timer[2] > 6 && timer[2] < 17;
        }
        bool isNight(){
            return !(timer[2] > 6 && timer[2] < 17);
        }
        void update_light(int x,int y,int x1,int y1){
            std::vector<int> dist_list;
            for(int l = 1;l<=LEN_LIGHT-isNight()?2:0;l++){
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
                grid[x][y][x1][y1].light = (isDay()?255:80)/(*std::min_element(dist_list.begin(),dist_list.end()));
            }else{
                grid[x][y][x1][y1].light = 0;
            }
        }
        void update_bitmask(int x, int y, int x1, int y1){
            if(grid[x][y][x1][y1].light > 0){
                int bitmask = 0;
                // top
                if (y1 > 0) {
                    if(grid[x][y][x1][y1 - 1].id != BLOCK_ID::VOID){
                        bitmask |= 1;
                    }
                }else if(y > 0){
                    if(grid[x][y-1][x1][chunk_hei-1].id != BLOCK_ID::VOID){
                        bitmask |= 1;
                    }
                }   
                // right
                if (x1 + 1 < chunk_wid) { 
                    if(grid[x][y][x1 + 1][y1].id != BLOCK_ID::VOID){
                        bitmask |= 2; 
                    }
                }else if(x + 1 < world_wid){
                    if(grid[x+1][y][0][y1].id != BLOCK_ID::VOID){
                        bitmask |= 2; 
                    }
                }
                // bottom
                if (y1 + 1 < chunk_hei) {
                    if(grid[x][y][x1][y1 + 1].id != BLOCK_ID::VOID){ 
                        bitmask |= 4;
                    }
                }else if(y+1 < world_hei){
                    if(grid[x][y+1][x1][0].id != BLOCK_ID::VOID){ 
                        bitmask |= 4;
                    }
                }
                // left
                if (x1 > 0) {
                    if(grid[x][y][x1 - 1][y1].id != BLOCK_ID::VOID){ 
                        bitmask |= 8; 
                    }
                }else if(x > 0){
                    if(grid[x - 1][y][chunk_wid-1][y1].id != BLOCK_ID::VOID){ 
                        bitmask |= 8; 
                    }
                }
                grid[x][y][x1][y1].tile = bitmask;
            }else{
                grid[x][y][x1][y1].tile = 11;
            }
        }
        void update_water_physics(int x, int y, int x1, int y1){
            if (grid[x][y][x1][y1].id == BLOCK_ID::WATER && grid[x][y][x1][y1].tick == 0) {
                Block &water_tile = grid[x][y][x1][y1];
                int dtx = x;
                int dty = y;
                int dtxx = x1;
                int dtyy = y1+1;
                convert_pos(dtx,dty,dtxx,dtyy);
                Block &block_down = grid[dtx][dty][dtxx][dtyy];
                // if can go down
                if(block_down.id == BLOCK_ID::VOID){
                    block_down = water_tile;
                    water_tile = Block(BLOCK_ID::VOID);
                }else{
                    int ltx = x;
                    int ltxx = x1-1;
                    int rtx = x;
                    int rtxx = x1+1;
                    convert_pos(ltx,y,ltxx,y1);
                    convert_pos(rtx,y,rtxx,y1);
                    Block &left_tile = grid[ltx][y][ltxx][y1];
                    Block &right_tile = grid[rtx][y][rtxx][y1];
                    if(left_tile.id == BLOCK_ID::VOID && right_tile.id == BLOCK_ID::VOID){
                        int dir = random_range(0,1);
                        if(dir){
                            left_tile = water_tile;
                            water_tile = Block(BLOCK_ID::VOID);
                        }else{
                            right_tile = water_tile;
                            water_tile = Block(BLOCK_ID::VOID);
                        }
                    }else{
                        if(left_tile.id == BLOCK_ID::VOID){ // if can go left
                            left_tile = water_tile;
                            water_tile = Block(BLOCK_ID::VOID);
                        }
                        if(right_tile.id == BLOCK_ID::VOID){ // if can go right
                            right_tile = water_tile;
                            water_tile = Block(BLOCK_ID::VOID);
                        }
                    }
                }
            }
        }
        void update_tile(int x, int y, int x1,int y1){
            SDL_Rect chunk_rect = {(x*(chunk_wid*tile_size))-*camx,(y*(chunk_hei*tile_size))-*camy,chunk_wid*tile_size,chunk_hei*tile_size};
            SDL_Rect tile_rect = {chunk_rect.x+x1*tile_size,chunk_rect.y+y1*tile_size,tile_size,tile_size};
            if(grid[x][y][x1][y1].tick > 0){
                grid[x][y][x1][y1].tick--;
            }else{
                grid[x][y][x1][y1].tick = grid[x][y][x1][y1].timer;
            }
            if(grid[x][y][x1][y1].id != BLOCK_ID::VOID){
                update_light(x,y,x1,y1);
                update_bitmask(x,y,x1,y1);
                update_water_physics(x,y,x1,y1);
            }
        }
        void update_chunk(int x, int y){
            int start_tile_x = std::max(0, (*camx - (x * chunk_wid * tile_size)) / tile_size);
            int end_tile_x = std::min(chunk_wid - 1, (*camx + window_resolution[0] - (x * chunk_wid * tile_size)) / tile_size);
            int start_tile_y = std::max(0, (*camy - (y * chunk_hei * tile_size)) / tile_size);
            int end_tile_y = std::min(chunk_hei - 1, (*camy + window_resolution[1] - (y * chunk_hei * tile_size)) / tile_size);
            
            // Loop through visible tiles within the chunk
            for (int x1 = start_tile_x; x1 <= end_tile_x; x1++) {
                for (int y1 = start_tile_y; y1 <= end_tile_y; y1++) {
                    update_tile(x,y,x1,y1);
                }
            }
        }
        void update_world(){
            int start_chunk_x = *camx / (chunk_wid * tile_size);
            int end_chunk_x = (*camx + window_resolution[0]) / (chunk_wid * tile_size);
            int start_chunk_y = *camy / (chunk_hei * tile_size);
            int end_chunk_y = (*camy + window_resolution[1]) / (chunk_hei * tile_size);
            
            for (int x = std::max(0, start_chunk_x); x <= std::min(end_chunk_x, (int)grid.size() - 1); x++) {
                for (int y = std::max(0, start_chunk_y); y <= std::min(end_chunk_y, (int)grid[x].size() - 1); y++) {
                    update_chunk(x,y);
                }
            }
        }
        void update(){
            astros_angle = calculateAngle();
            if(timer[2] >= 24){
                timer[2] = 0;
            }else{
                if(timer[1] >= 60){
                    timer[2]++;
                    timer[1] = 0;
                }else{
                    if(timer[0] >= 60){
                        timer[1]++;
                        timer[0] = 0;
                    }else{
                        timer[0]++;
                    }
                }
            }
            update_world();
        }
        void draw_parallax(SpriteSheet *sprite,double speed=1,int wid = window_resolution[0],int hei = window_resolution[1],int angle=0,SDL_RendererFlip flip = SDL_FLIP_NONE,SDL_Color color = {255,255,255}){
            double lx1 = fmod(-(*camx*speed), wid);
            double lx2 = lx1+window_resolution[0];
            sprite->renderFrame(renderer,0,lx1,0,wid,hei,angle,flip,color);
            sprite->renderFrame(renderer,0,lx2,0,wid,hei,angle,flip,color);
        }
        void draw(){
            int x1 = ((window_resolution[0]/2)-32)+(cos(astros_angle)*((window_resolution[0]/2)-64));
            int y1 = (window_resolution[1]/2)+(sin(astros_angle)*((window_resolution[1]/2)-64));
            
            sprSun->renderFrame(renderer,0,x1,y1,64,64,astros_angle);
            
            int x2 = ((window_resolution[0]/2)-32)+(cos(astros_angle+M_PI)*((window_resolution[0]/2)-64));
            int y2 = (window_resolution[1]/2)+(sin(astros_angle+M_PI)*((window_resolution[1]/2)-64));
            sprMoon->renderFrame(renderer,0,x2,y2,64,64,astros_angle);
            
            SDL_Color col_night = {80,80,80,255};
            SDL_Color col_day = {255,255,255,255};
            SDL_Color color = isNight()?col_night:col_day;
            draw_parallax(layer_2,.12,window_resolution[0],window_resolution[1],0,SDL_FLIP_NONE,color);
            draw_parallax(layer_1,.25,window_resolution[0],window_resolution[1],0,SDL_FLIP_NONE,color);
            draw_parallax(layer_0,.50,window_resolution[0],window_resolution[1],0,SDL_FLIP_NONE,color);
            
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
                    
                    for (int x1 = start_tile_x; x1 <= end_tile_x; x1++) {
                        for (int y1 = start_tile_y; y1 <= end_tile_y; y1++) {
                            SDL_Rect tile_rect = {chunk_rect.x+x1*tile_size,chunk_rect.y+y1*tile_size,tile_size,tile_size};                    
                            tile_rect.y-=(tile_rect.h-tile_size);
                            if(grid[x][y][x1][y1].id != BLOCK_ID::VOID){
                                int l = grid[x][y][x1][y1].light;
                                TileSetList[grid[x][y][x1][y1].id-1]->renderFrame(renderer,grid[x][y][x1][y1].tile,tile_rect.x,tile_rect.y,tile_rect.w,tile_rect.h,0,SDL_FLIP_NONE,{(Uint8)l,(Uint8)l,(Uint8)l,255});
                            }
                        }
                    }
                }
            }
        }
        void destroy(){
            for(int i = 0;i<TileSetList.size();i++){
                delete TileSetList[i];
            }
        }
    
};
#endif