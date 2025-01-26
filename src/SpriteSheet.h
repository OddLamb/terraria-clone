#ifndef SPRITESHEET_H
#define SPRITESHEET_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* tempSurface = IMG_Load(path.c_str());
    
    if (!tempSurface) {
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    return texture;
}

class SpriteSheet {
private:
    SDL_Texture* texture;    
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

    void renderFrame(SDL_Renderer* renderer, int frame, int x, int y, int wid, int hei) {
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

        SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
    }
    int getFrameWidth() const { return frameWidth; }
    int getFrameHeight() const { return frameHeight; }
    int getTotalFrames() const { return totalFrames; }
};

#endif