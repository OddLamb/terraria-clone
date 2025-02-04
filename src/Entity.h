#ifndef ENTITY_H
#define ENTITY_H
#include <SDL2/SDL.h>
#include "Macros.h"
#include "World.h"

class Entity{
  public:
    int *camx = 0;
    int *camy = 0;
    SDL_Renderer *renderer;
    World *wld;
    Entity(int *_camx, int *_camy,World *_wld,SDL_Renderer *_renderer){
      camx = _camx;
      camy = _camy;
      wld = _wld;
      renderer = _renderer;
    }
    virtual ~Entity() = default;  
};

#endif

