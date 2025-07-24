# CHIP-8 Emulator Menu System Design

This document outlines the design for the improved menu system for the CHIP-8 emulator.

## Requirements

1. Create a visually appealing menu interface using SDL
2. Implement directory scanning to find .ch8 files in the /games folder
3. Create a navigation system to browse games by folder
4. Update the menu to dynamically list available games
5. Modify the game launching mechanism to work with games in the /games folder

## Menu Interface Design

### Visual Improvements

1. **Background Design**
   - Add a gradient background instead of solid black
   - Include a CHIP-8 logo or title graphic at the top
   - Add decorative elements like pixel art borders

2. **Text Rendering**
   - Use a more appealing font with proper scaling
   - Implement text shadows for better readability
   - Add color transitions for selected items

3. **Layout**
   - Center the menu with proper spacing
   - Add a status bar at the bottom for navigation hints
   - Include a breadcrumb navigation at the top

### Navigation System

1. **Main Menu Structure**
   - Start Game (enters game browser)
   - Controls (displays control information)
   - Settings (optional - for adjusting emulator settings)
   - Exit

2. **Game Browser**
   - Display games from the /games directory
   - Show game titles with proper formatting (remove file extensions, replace underscores with spaces)
   - Support scrolling for long lists
   - Allow navigation into subfolders if present
   - Include a "Back" option to return to the main menu

3. **Controls**
   - Arrow keys for navigation
   - Enter to select
   - Escape to go back
   - Page Up/Down for faster scrolling in long lists

## Technical Implementation

### Directory Scanning

```c
// Function to scan directory for .ch8 files
void scan_directory(const char *dir_path, GameList *games) {
    DIR *dir;
    struct dirent *entry;
    
    dir = opendir(dir_path);
    if (!dir) return;
    
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        char full_path[PATH_MAX];
        snprintf(full_path, PATH_MAX, "%s/%s", dir_path, entry->d_name);
        
        // Check if it's a directory
        struct stat path_stat;
        stat(full_path, &path_stat);
        
        if (S_ISDIR(path_stat.st_mode)) {
            // Add folder to list
            add_folder_to_list(games, entry->d_name, full_path);
            
            // Recursively scan subdirectory
            scan_directory(full_path, games);
        } else {
            // Check if file has .ch8 extension
            char *ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".ch8") == 0) {
                // Add game to list
                add_game_to_list(games, entry->d_name, full_path);
            }
        }
    }
    
    closedir(dir);
}
```

### Data Structures

```c
// Game item structure
typedef struct GameItem {
    char name[256];         // Display name
    char path[PATH_MAX];    // Full path to the file
    bool is_folder;         // Is this a folder or a game file
    struct GameItem *next;  // Next item in the list
} GameItem;

// Game list structure
typedef struct {
    GameItem *items;        // List of games/folders
    int count;              // Number of items
    int current_index;      // Currently selected index
    int scroll_offset;      // Scroll offset for long lists
    char current_path[PATH_MAX]; // Current directory path
} GameList;
```

### Menu State Management

```c
// Menu states
typedef enum {
    MENU_MAIN,
    MENU_GAMES,
    MENU_CONTROLS,
    MENU_SETTINGS,
    MENU_EXIT
} MenuState;

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
```

### Game Launching

```c
// Function to launch a game
void launch_game(const char *game_path) {
    char command[512];
    snprintf(command, sizeof(command), "./chip8 \"%s\"", game_path);
    system(command);
}
```

## Implementation Plan

1. Update menu.h with new data structures and function declarations
2. Implement directory scanning functionality in menu.c
3. Create improved rendering functions for the menu interface
4. Implement the navigation system
5. Update the game launching mechanism
6. Test with various games in the /games folder

## Visual Reference

```
+------------------------------------------+
|              CHIP-8 EMULATOR             |
+------------------------------------------+
|                                          |
|               START GAME                 |
|                                          |
|               CONTROLS                   |
|                                          |
|               SETTINGS                   |
|                                          |
|               EXIT                       |
|                                          |
+------------------------------------------+
|  Arrow keys: Navigate    Enter: Select   |
+------------------------------------------+
```

Game browser view:
```
+------------------------------------------+
|  CHIP-8 EMULATOR > GAMES                 |
+------------------------------------------+
|                                          |
|  [FOLDER] Arcade Games                   |
|  [FOLDER] Puzzle Games                   |
|                                          |
|  Pong                                    |
|  Space Invaders                          |
|  Tetris                                  |
|  Breakout                                |
|  ...                                     |
|                                          |
|  Back to Main Menu                       |
|                                          |
+------------------------------------------+
|  ↑↓: Navigate  Enter: Select  Esc: Back  |
+------------------------------------------+