#ifndef ENTITY_H
#define ENTITY_H
#include <SDL2/SDL.h>
#include "Macros.h"
#include "World.h"

class Entity{
  public:
    SDL_Rect rect = {0,0,0,0};
    int *camx = nullptr;
    int *camy = nullptr;
    SDL_Renderer *renderer = nullptr;
    World *wld = nullptr;
    Entity(int *_camx, int *_camy,World *_wld,SDL_Renderer *_renderer){
      camx = _camx;
      camy = _camy;
      wld = _wld;
      renderer = _renderer;
    }
    virtual void update(float deltaTime){

    }
    virtual void draw(){

    }
    virtual void handle_events(SDL_Event *event){

    }
    virtual void destroy(){

    }
    virtual ~Entity() = default;  
};

#endif

