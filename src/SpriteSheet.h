#ifndef SPRITESHEET_H
#define SPRITESHEET_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cerr << "Error loading image: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "Error creating texture: " << SDL_GetError() << std::endl;
    }
    return texture;
}

class SpriteSheet {
private:
    SDL_Texture* texture = nullptr;    
    int frameWidth;        
    int frameHeight;   
    int totalFrames;
    int columns;
     
public:
    SpriteSheet(SDL_Texture* texture, int frameWidth, int frameHeight, int totalFrames)
        : texture(texture), frameWidth(frameWidth), frameHeight(frameHeight), totalFrames(totalFrames) {
        int texWidth, texHeight;
        SDL_QueryTexture(texture, nullptr, nullptr, &texWidth, &texHeight);
        columns = texWidth / frameWidth; 
    }

    ~SpriteSheet() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }

    void renderFrame(SDL_Renderer* renderer, int frame, int x, int y, int wid, int hei,double angle = 0,SDL_RendererFlip flip = SDL_FLIP_NONE,SDL_Color color = {255,255,255,255}) {
        if (frame >= totalFrames) {
            return;
        }

        SDL_Rect srcRect;
        srcRect.x = (frame % columns) * frameWidth;
        srcRect.y = (frame / columns) * frameHeight;
        srcRect.w = frameWidth;
        srcRect.h = frameHeight;

        SDL_Rect dstRect;
        dstRect.x = x;
        dstRect.y = y;
        dstRect.w = wid;
        dstRect.h = hei;
        
        SDL_SetTextureAlphaMod(texture, color.a);
        SDL_SetTextureColorMod(texture, color.r,color.g,color.b);
        SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, angle * 180/M_PI, NULL, flip);
        SDL_SetTextureColorMod(texture, 255, 255, 255); 
        SDL_SetTextureAlphaMod(texture, 255);
    }
    int getFrameWidth() const { return frameWidth; }
    int getFrameHeight() const { return frameHeight; }
    int getTotalFrames() const { return totalFrames; }
    SDL_Texture *getTexture() const { return texture; }
};
class Animation{
    SpriteSheet *spriteSheet = nullptr;
    int frame_speed;
    int frame_clock = 0;
    int frame_current = 0;
    public:
        Animation(SpriteSheet *_spriteSheet,int _frame_speed):
        spriteSheet(_spriteSheet),frame_speed(_frame_speed){}
        void update(int deltaTime=1){
            if(frame_clock > 0){
                frame_clock--;
            }else{
                if(frame_current+deltaTime <= spriteSheet->getTotalFrames()-1){
                    frame_current+=deltaTime;
                }else{
                    frame_current = 0;
                }
                frame_clock+=frame_speed;
            }
        }
        void render(SDL_Renderer *renderer,int x,int y,int wid,int hei,int angle = 0,SDL_RendererFlip flip = SDL_FLIP_NONE,SDL_Color color={255,255,255,255}){
            spriteSheet->renderFrame(renderer,frame_current,x,y,wid,hei,angle,flip,color);
        }
        void destroy(){
            delete spriteSheet;
        }
};
#endif