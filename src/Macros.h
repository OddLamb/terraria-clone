#ifndef MACROS_H
#define MACROS_H
#include <random>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

enum BLOCK_ID{
    VOID,
    GRASS,
    DIRT,
    STONE,
    WATER,
};
const struct{
    std::string DAYTIME_THEME = "./assets/tracks/forest-day.mp3";
    std::string NIGHTTIME_THEME = "./assets/tracks/forest-night.mp3";
} MUSIC_PATHS;

struct Block {
    int id;
    int light;
    bool solid;
    int tile;
    int tick;
    int timer = 6;
    constexpr Block(int _id = -1, int _light = 0,bool _solid = true,int _tile = 0,int _tick = 0): 
                    id(_id), light(_light), solid(_solid),tile(_tile),tick(_tick){
        if(id == BLOCK_ID::VOID || id == BLOCK_ID::WATER){
            solid = false;
        }
    }
};
struct Item{

};
const int inv_wid = 6;
const int inv_hei = 4;
typedef std::vector<std::vector<Block>> chunk;
typedef std::vector<std::vector<chunk>> world;
typedef std::vector<std::vector<Item>> inventory;

int DIR_LIGHT = 8;
int LEN_LIGHT = 4;
const int window_resolution[2] = {1024,640};

const int tile_size = 16;

const int chunk_wid = 32;
const int chunk_hei = 32;

const int world_wid = 10;
const int world_hei = 10;

const int framerate = 60;
const int frame_delay = 1000 / framerate;
void draw_text(std::string text, int x, int y,SDL_Renderer *renderer,TTF_Font *font,SDL_Color fg = {0,0,0,255},SDL_Color bg = {0,0,0,0}){
    SDL_Surface* textSurface = TTF_RenderText_Shaded(font, text.c_str(), fg,bg);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect renderQuad = { 0, 0, textSurface->w, textSurface->h }; 
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);

    SDL_FreeSurface(textSurface);
}
double distance(double x1, double y1, double x2, double y2) {
    return std::sqrt(std::pow((x2 - x1), 2) + std::pow((y2 - y1), 2));
}
std::mt19937 global_gen;
void set_global_seed(int seed = -1) {
    if (seed == -1) {
        std::random_device rd;
        global_gen.seed(rd()); 
    } else {
        global_gen.seed(seed); 
    }
}
int random_range(int min, int max) {
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(global_gen);
}
int choose(std::vector<int> nums){
    return nums[random_range(0,nums.size()-1)];
}

int sign(int x){
    if(x > 0){
        return 1;
    }else if(x < 0){
        return -1;
    }
    return 0;
}
float lerp(float a, float b, float f)
{
    return a * (1.0 - f) + (b * f);
}
float clamp(float value,float max, float min){
    if(value>max){
        return max;
    }else if(value<min){
        return min;
    }else{
        return value;
    }
}
bool inside_cam(SDL_Rect rect){
    return rect.x > 0 || rect.y > 0 || rect.x < window_resolution[0] || rect.y < window_resolution[0] || rect.x+rect.w > 0 || rect.y+rect.h > 0 || rect.x+rect.w < window_resolution[0] || rect.y+rect.h < window_resolution[1];
}
bool rect_in_rect(const SDL_Rect a, const SDL_Rect b) {
    return a.x < b.x + b.w &&
           a.x + a.w > b.x &&
           a.y < b.y + b.h &&
           a.y + a.h > b.y;
}
bool colliding_world(SDL_Rect rect,world &_world){
    if(rect.x < 0 || rect.y < 0 || rect.x+rect.w > world_wid*(chunk_wid*tile_size) || rect.y+rect.h > world_hei*(chunk_wid*tile_size)){
        return true;
    }
    int start_chunk_x = rect.x / (chunk_wid * tile_size);
    int end_chunk_x = (rect.x + rect.w) / (chunk_wid * tile_size);
    int start_chunk_y = rect.y / (chunk_hei * tile_size);
    int end_chunk_y = (rect.y + rect.h) / (chunk_hei * tile_size);
            
    for (int x = std::max(0, start_chunk_x); x <= std::min(end_chunk_x, (int)_world.size() - 1); x++) {
        for (int y = std::max(0, start_chunk_y); y <= std::min(end_chunk_y, (int)_world[x].size() - 1); y++) {
            chunk &_chunk = _world[x][y];
            for(int xx = 0;xx<_chunk.size();xx++){
                for(int yy = 0;yy<_chunk[xx].size();yy++){
                    if(_chunk[xx][yy].solid){
                        int tx = (x*(chunk_wid*tile_size))+xx*tile_size;
                        int ty = (y*(chunk_hei*tile_size))+yy*tile_size;
                        if(rect_in_rect(rect,{tx,ty,tile_size,tile_size})){
                            return true;
                        }
                    }
                }
            }
        }
    }
    
    return false;
}
std::vector<int> tile_at_point(int px, int py, world world_grid) {
    int chunk_x = px / (chunk_wid * tile_size);
    int chunk_y = py / (chunk_hei * tile_size);

    if (chunk_x < 0 || chunk_x >= world_wid || 
        chunk_y < 0 || chunk_y >= world_hei) {
        return {0, 0, 0, 0}; 
    }

    int local_x = (px % (chunk_wid * tile_size)) / tile_size;
    int local_y = (py % (chunk_hei * tile_size)) / tile_size;

    if (local_x >= 0 && local_x < chunk_wid &&
        local_y >= 0 && local_y < chunk_hei) {
        return {chunk_x, chunk_y, local_x, local_y};
    }

    return {0, 0, 0, 0};
}
#endif



