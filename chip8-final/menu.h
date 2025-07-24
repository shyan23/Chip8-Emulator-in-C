#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800
#define MAX_ITEMS_VISIBLE 12
#define MAX_PATH_LENGTH 512

// Colors
#define COLOR_BACKGROUND 0, 0, 40, 255
#define COLOR_TEXT 220, 220, 220, 255
#define COLOR_SELECTED 0, 255, 0, 255
#define COLOR_TITLE 0, 200, 255, 255
#define COLOR_FOLDER 255, 255, 0, 255
#define COLOR_STATUS_BAR 40, 40, 80, 255

// Menu states
typedef enum {
    MENU_MAIN,
    MENU_GAMES,
    MENU_CONTROLS,
    MENU_SETTINGS,
    MENU_EXIT
} MenuState;

// Game item structure
typedef struct GameItem {
    char name[256];         // Display name
    char path[MAX_PATH_LENGTH];    // Full path to the file
    bool is_folder;         // Is this a folder or a game file
    struct GameItem *next;  // Next item in the list
} GameItem;

// Game list structure
typedef struct {
    GameItem *items;        // List of games/folders
    int count;              // Number of items
    int current_index;      // Currently selected index
    int scroll_offset;      // Scroll offset for long lists
    char current_path[MAX_PATH_LENGTH]; // Current directory path
} GameList;

// Menu context
typedef struct {
    MenuState state;        // Current menu state
    GameList games;         // Game list
    SDL_Texture *background; // Background texture
    SDL_Texture *logo;      // Logo texture
    TTF_Font *title_font;   // Font for titles
    TTF_Font *menu_font;    // Font for menu items
    bool running;           // Menu running flag
} MenuContext;

// Main menu items
typedef struct {
    const char *label;
    MenuState next_state;
} MenuItem;

// Function declarations
void init_menu(MenuContext *ctx, SDL_Renderer *renderer);
void cleanup_menu(MenuContext *ctx);
void render_menu(MenuContext *ctx, SDL_Renderer *renderer);
void handle_menu_input(MenuContext *ctx, SDL_Event *event);
void scan_directory(const char *dir_path, GameList *games);
void add_game_to_list(GameList *games, const char *name, const char *path, bool is_folder);
void clear_game_list(GameList *games);
void format_game_name(char *dest, const char *src);
void launch_game(const char *game_path);
void render_main_menu(MenuContext *ctx, SDL_Renderer *renderer);
void render_game_browser(MenuContext *ctx, SDL_Renderer *renderer);
void render_controls(MenuContext *ctx, SDL_Renderer *renderer);
void render_gradient_background(SDL_Renderer *renderer);
SDL_Texture* create_text_texture(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color);

#endif // MENU_H
