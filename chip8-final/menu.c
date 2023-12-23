#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "menu.h"


void drawControls(const char* file) {
 char command[256];
    snprintf(command, sizeof(command), "xdg-open %s &", file);
    system(command);
    
SDL_Delay(1000);    
}

int main() {
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Init();

    TTF_Font* font = TTF_OpenFont("LiberationSerif-Regular.ttf", 32);
    if (!font) {
        fprintf(stderr, "Warning: Couldn't open specified font - %s\n", TTF_GetError());
        font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSerif-Regular.ttf", 32);

        return 1;
    }

    MenuItem mainMenu[] = {
        {1, "START"},
        
        
        {2, "CONTROL"},
        
        
        {3, "EXIT"}
    };

    MenuItem romList[] = {
        {1, "1. TANK"},
        
        {2, "2. TETRIS"},
        
        {3, "3. PONG SOLO"},
        
        {4, "4. PONG"},
        
        {5, "5. HIDDEN"},
        
        {6, "6. BRIX"},
        
        {7, "7. UFO"},
        
        {8, "8. SUBMARINE"},
        
        {9, "9. SPACE INVADER"},
        
        {10, "10. FILTER"},
        
        {11, "Back"}
    };

    int selected = 0;
    SDL_Event event;
    int quit = 0;  // Flag to control the main loop
    

    while (!quit) {
        drawMenu(renderer, font, mainMenu, sizeof(mainMenu) / sizeof(mainMenu[0]), selected);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;  // Set the flag to exit the main loop
                break;
            } 
            
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        selected = (selected - 1 + sizeof(mainMenu) / sizeof(mainMenu[0])) % (sizeof(mainMenu) / sizeof(mainMenu[0]));
                        break;
                    case SDLK_DOWN:
                        selected = (selected + 1) % (sizeof(mainMenu) / sizeof(mainMenu[0]));
                        break;
                    case SDLK_RETURN:
                        // Handle the selected option
                        if (selected == 0) { // "Chip 8 ROMs"
                            int romListOption = 0;

                            SDL_Window* romWindow = SDL_CreateWindow("ROM List", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
                            SDL_Renderer* romRenderer = SDL_CreateRenderer(romWindow, -1, SDL_RENDERER_ACCELERATED);

                            while (1) {
                                drawROMList(romRenderer, font, romList, sizeof(romList) / sizeof(romList[0]), romListOption);

                                while (SDL_PollEvent(&event)) {
                                    if (event.type == SDL_QUIT) {
                                        SDL_DestroyRenderer(romRenderer);
                                        SDL_DestroyWindow(romWindow);
                                        romWindow = NULL;
                                        quit = 1;  // Exit the main loop if the ROM window is closed
                                        break;
                                    } else if (event.type == SDL_KEYDOWN) {
                                        switch (event.key.keysym.sym) {
                                            case SDLK_UP:
                                                romListOption = (romListOption - 1 + sizeof(romList) / sizeof(romList[0])) % (sizeof(romList) / sizeof(romList[0]));
                                                break;
                                            case SDLK_DOWN:
                                                romListOption = (romListOption + 1) % (sizeof(romList) / sizeof(romList[0]));
                                                break;
                                            case SDLK_RETURN:
                                                // Handle the selected ROM option (romList[romListOption])
                                                 printf("Selected ROM: %s\n", romList[romListOption].label);
                                                    // Add logic to handle the selected ROM
                                                if (romListOption == 10) { // "Back"
                                                    SDL_DestroyRenderer(romRenderer);
                                                    SDL_DestroyWindow(romWindow);
                                                    romWindow = NULL;
                                                    break;  // Break out of the inner loop to go back to the main menu
                                                } else {
                                                    if (romListOption == 0) { // "ROM 1"
                                                        system("./chip8 './chip8-roms/games/Tank.ch8'");
                                                    }
                                                    if (romListOption == 1) { // "ROM 2"
                                                        system("./chip8 './chip8-roms/games/Tetris [Fran Dachille, 1991].ch8'");
                                                    }
                                                    if (romListOption == 2) { // "ROM 3"
                                                        system("./chip8 './chip8-roms/games/Pong (1 player).ch8' ");
                                                    }
                                                     if (romListOption == 3) { // "ROM 4"
                                                        system("./chip8 './chip8-roms/games/Pong 2 (Pong hack) [David Winter, 1997].ch8'");
                                                    }
                                                    if (romListOption == 4) { // "ROM 5"
                                                        system("./chip8 './chip8-roms/games/Hidden [David Winter, 1996].ch8'");
                                                    }
                                                    if (romListOption == 5) { // "ROM 6"
                                                        system("./chip8 './chip8-roms/games/Brix [Andreas Gustafsson, 1990].ch8'");
                                                    }
                                                    if (romListOption == 6) { // "ROM 7"
                                                        system("./chip8 './chip8-roms/games/UFO [Lutz V, 1992].ch8'");
                                                    }
                                                    if (romListOption == 7) { // "ROM 8"
                                                        system("./chip8 './chip8-roms/games/Submarine [Carmelo Cortez, 1978].ch8'");
                                                    }
                                                     if (romListOption == 8) { // "ROM 9"
                                                        system("./chip8 './chip8-roms/games/Space Invaders [David Winter].ch8'");
                                                    }
                                                    if (romListOption == 9) { // "ROM 10"
                                                        system("./chip8 './chip8-roms/games/Filter.ch8'");
                                                    }                                                       
                                                   
                                                }
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                }

                                SDL_Delay(60);
                                if (romWindow == NULL) {
                                    break;
                                }
                            }
                        }
                        else if (selected == 1) { // "Controls"
    // Display the controls
    drawControls("controls.txt");

    // Wait for a key press before returning to the main menu
    int ctrlopen=1;
    while (ctrlopen) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1; 
                ctrlopen=0;// Exit the main loop
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                ctrlopen=0;  // Break out of the loop to return to the main menu
            }
        }
        SDL_Delay(60);
    }
}

/*if(selected==2){
                  printf("Chip-8 Emulator\nCreated by SAKIB HOSSAIN and NAFIS SHAYAN\n\n\t\t24/12/2023");
    int exitEvent=1;
    while (exitEvent) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1; 
                exitEvent=0;// Exit the main loop
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                exitEvent=0;  // Break out of the loop to return to the main menu
            }
        }
                  
      }
    }   */         

    if(selected==2){
                   quit=1; 
                  }      
                  break;
                                             
                    default:
                  break;
                }
            }
        }

        SDL_Delay(60);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

return 0;
}    
