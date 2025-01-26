#ifndef MACROS_H
#define MACROS_H
#include <random>
#include <vector>
#include <SDL2/SDL.h>
constexpr struct {
    int VOID = -1;
    int GRASS = 0;
    int DIRT = 1;
    int STONE = 2;
} BLOCKS_ID;

typedef std::vector<std::vector<int>> chunk;
typedef std::vector<std::vector<chunk>> world;

const int window_resolution[2] = {1024,640};

const int tile_size = 32;

const int chunk_wid = 64;
const int chunk_hei = 64;

const int world_wid = 4;
const int world_hei = 4;

const int framerate = 60;
const int frame_delay = 1000 / framerate;

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
    std::vector<std::vector<int>> list_chunks;
    for(int wx = 0;wx<_world.size();wx++){
        for(int wy = 0;wy<_world[wx].size();wy++){
            if(rect_in_rect(rect,{wx*(chunk_wid*tile_size),wy*(chunk_hei*tile_size),chunk_wid*tile_size,chunk_hei*tile_size})){
                list_chunks.push_back({wx,wy});
            }
        }
    }
    for(int i = 0;i<list_chunks.size();i++){
        int chunk_x = list_chunks[i][0];
        int chunk_y = list_chunks[i][1];
        chunk &_chunk = _world[chunk_x][chunk_y];
        for(int x = 0;x<_chunk.size();x++){
            for(int y = 0;y<_chunk[x].size();y++){
                if(_chunk[x][y] != -1){
                    int tx = (chunk_x*(chunk_wid*tile_size))+x*tile_size;
                    int ty = (chunk_y*(chunk_hei*tile_size))+y*tile_size;
                    if(rect_in_rect(rect,{tx,ty,tile_size,tile_size})){
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
std::vector<int> tile_at_point(int px,int py,world world_grid){
    for(int x = 0;x < world_grid.size();x++){
        for(int y = 0;y < world_grid[x].size();y++){
            if(rect_in_rect({x*(chunk_wid*tile_size),y*(chunk_hei*tile_size),chunk_wid*tile_size,chunk_hei*tile_size},{px,py,1,1})){
                for(int xx = 0;xx < world_grid[x][y].size();xx++){
                    for(int yy = 0;yy < world_grid[x][y][xx].size();yy++){
                        if(rect_in_rect({(x*(chunk_wid*tile_size))+xx*tile_size,
                                        (y*(chunk_hei*tile_size))+yy*tile_size,tile_size,tile_size},{px,py,1,1})){  
                            return {x,y,xx,yy};
                        }
                    }
                }
            }
        }
    }
    return {0,0,0,0};
}
#endif



