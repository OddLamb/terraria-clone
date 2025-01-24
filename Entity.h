#ifndef ENTITY_H
#define ENTITY_H
#include <SDL2/SDL.h>
#include "Macros.h"

class Entity{
  public:
    int *camx = 0;
    int *camy = 0;
    SDL_Renderer *renderer;
    world *world_grid;
    Entity(int *_camx, int *_camy,world *_world_grid,SDL_Renderer *_renderer){
      camx = _camx;
      camy = _camy;
      world_grid = _world_grid;
      renderer = _renderer;
    }
    virtual ~Entity() = default;  
};

#endif

