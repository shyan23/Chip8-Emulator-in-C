#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800

typedef struct {
    int option;
    const char* label;
} MenuItem;

void drawMenu(SDL_Renderer* renderer, TTF_Font* font, MenuItem* menu, int itemCount, int selectedOption) { //window adjustment via text Rect
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Color selectedColor = {255, 0, 0, 255};


    for (int i = 0; i < itemCount; ++i) {
        SDL_Rect textRect = {SCREEN_WIDTH / 2 - 64, SCREEN_HEIGHT / 2 + i * 40, 128, 32}; // Adjusted text design
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, menu[i].label, (i == selectedOption) ? selectedColor : textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface); 

        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    SDL_RenderPresent(renderer);
}

void drawROMList(SDL_Renderer* renderer, TTF_Font* font, MenuItem* romList, int itemCount, int selectedOption) {  //Next one
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Color selectedColor = {255, 0, 0, 255};

int listStart=100;    

    for (int i = 0; i < itemCount; ++i) {
        SDL_Rect textRect = {SCREEN_WIDTH / 2 - 64,listStart + i * 40, 128, 32}; // Adjusted text design with text Rect
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, romList[i].label, (i == selectedOption) ? selectedColor : textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    SDL_RenderPresent(renderer);
}
