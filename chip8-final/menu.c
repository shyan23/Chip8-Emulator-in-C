    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <dirent.h>
    #include <sys/stat.h>
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
    #include "menu.h"

    // Main menu items
    static const MenuItem main_menu_items[] = {
        {"START GAME", MENU_GAMES},
        {"CONTROLS", MENU_CONTROLS},
        {"EXIT", MENU_EXIT}
    };
    #define MAIN_MENU_ITEMS_COUNT (sizeof(main_menu_items) / sizeof(main_menu_items[0]))

    // Initialize the menu context
    void init_menu(MenuContext *ctx, SDL_Renderer *renderer) {
        // Initialize menu state
        ctx->state = MENU_MAIN;
        ctx->running = true;
        
        // Initialize game list
        ctx->games.items = NULL;
        ctx->games.count = 0;
        ctx->games.current_index = 0;
        ctx->games.scroll_offset = 0;
        strcpy(ctx->games.current_path, "./games");
        
        // Load fonts
        ctx->title_font = TTF_OpenFont("chip8-final/LiberationSerif-Regular.ttf", 48);
        if (!ctx->title_font) {
            fprintf(stderr, "Warning: Couldn't open title font - %s\n", TTF_GetError());
            ctx->title_font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSerif-Regular.ttf", 48);
            if (!ctx->title_font) {
                fprintf(stderr, "Error: Could not load any title font\n");
                exit(EXIT_FAILURE);
            }
        }
        
        ctx->menu_font = TTF_OpenFont("chip8-final/LiberationSerif-Regular.ttf", 32);
        if (!ctx->menu_font) {
            fprintf(stderr, "Warning: Couldn't open menu font - %s\n", TTF_GetError());
            ctx->menu_font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSerif-Regular.ttf", 32);
            if (!ctx->menu_font) {
                fprintf(stderr, "Error: Could not load any menu font\n");
                exit(EXIT_FAILURE);
            }
        }
        
        // Create background texture (gradient will be rendered directly)
        ctx->background = NULL;
        
        // Create logo texture
        SDL_Color logo_color = {COLOR_TITLE};
        SDL_Surface *logo_surface = TTF_RenderText_Blended(ctx->title_font, "CHIP-8 EMULATOR", logo_color);
        ctx->logo = SDL_CreateTextureFromSurface(renderer, logo_surface);
        SDL_FreeSurface(logo_surface);
        
        // Scan games directory
        scan_directory(ctx->games.current_path, &ctx->games);
    }

    // Clean up menu resources
    void cleanup_menu(MenuContext *ctx) {
        // Free game list
        clear_game_list(&ctx->games);
        
        // Free textures
        if (ctx->logo) SDL_DestroyTexture(ctx->logo);
        if (ctx->background) SDL_DestroyTexture(ctx->background);
        
        // Close fonts
        if (ctx->title_font) TTF_CloseFont(ctx->title_font);
        if (ctx->menu_font) TTF_CloseFont(ctx->menu_font);
    }

    // Render the menu based on current state
    void render_menu(MenuContext *ctx, SDL_Renderer *renderer) {
        // Clear screen with gradient background
        render_gradient_background(renderer);
        
        // Render based on current state
        switch (ctx->state) {
            case MENU_MAIN:
                render_main_menu(ctx, renderer);
                break;
            case MENU_GAMES:
                render_game_browser(ctx, renderer);
                break;
            case MENU_CONTROLS:
                render_controls(ctx, renderer);
                break;
            default:
                break;
        }
        
        // Present renderer
        SDL_RenderPresent(renderer);
    }

    // Handle menu input
    void handle_menu_input(MenuContext *ctx, SDL_Event *event) {
        if (event->type == SDL_QUIT) {
            ctx->running = false;
            return;
        }
        
        if (event->type == SDL_KEYDOWN) {
            switch (ctx->state) {
                case MENU_MAIN:
                    // Main menu navigation
                    switch (event->key.keysym.sym) {
                        case SDLK_UP:
                            if (ctx->games.current_index > 0)
                                ctx->games.current_index--;
                            break;
                        case SDLK_DOWN:
                            if (ctx->games.current_index < (int)MAIN_MENU_ITEMS_COUNT - 1)
                                ctx->games.current_index++;
                            break;
                        case SDLK_RETURN:
                            // Handle selection
                            ctx->state = main_menu_items[ctx->games.current_index].next_state;
                            if (ctx->state == MENU_GAMES) {
                                // Reset game browser state
                                ctx->games.current_index = 0;
                                ctx->games.scroll_offset = 0;
                                // Clear and rescan games directory
                                clear_game_list(&ctx->games);
                                scan_directory(ctx->games.current_path, &ctx->games);
                            } else if (ctx->state == MENU_EXIT) {
                                ctx->running = false;
                            }
                            break;
                        case SDLK_ESCAPE:
                            ctx->running = false;
                            break;
                        default:
                            break;
                    }
                    break;
                    
                case MENU_GAMES:
                    // Game browser navigation
                    switch (event->key.keysym.sym) {
                        case SDLK_UP:
                            if (ctx->games.current_index > 0) {
                                ctx->games.current_index--;
                                // Adjust scroll if needed
                                if (ctx->games.current_index < ctx->games.scroll_offset)
                                    ctx->games.scroll_offset = ctx->games.current_index;
                            }
                            break;
                        case SDLK_DOWN:
                            if (ctx->games.current_index < ctx->games.count - 1) {
                                ctx->games.current_index++;
                                // Adjust scroll if needed
                                if (ctx->games.current_index >= ctx->games.scroll_offset + MAX_ITEMS_VISIBLE)
                                    ctx->games.scroll_offset = ctx->games.current_index - MAX_ITEMS_VISIBLE + 1;
                            }
                            break;
                        case SDLK_RETURN: {
                            // Get selected item
                            GameItem *item = ctx->games.items;
                            for (int i = 0; i < ctx->games.current_index && item; i++)
                                item = item->next;
                            
                            if (item) {
                                if (item->is_folder) {
                                    // Navigate to folder
                                    strcpy(ctx->games.current_path, item->path);
                                    // Clear and rescan directory
                                    clear_game_list(&ctx->games);
                                    scan_directory(ctx->games.current_path, &ctx->games);
                                    // Reset selection
                                    ctx->games.current_index = 0;
                                    ctx->games.scroll_offset = 0;
                                } else {
                                    // Launch game
                                    launch_game(item->path);
                                }
                            }
                            break;
                        }
                        case SDLK_ESCAPE:
                            // Go back to main menu or parent directory
                            if (strcmp(ctx->games.current_path, "./games") == 0) {
                                // Back to main menu
                                ctx->state = MENU_MAIN;
                                ctx->games.current_index = 0;
                            } else {
                                // Go to parent directory
                                char *last_slash = strrchr(ctx->games.current_path, '/');
                                if (last_slash) {
                                    *last_slash = '\0'; // Truncate at last slash
                                    // If we're at root of games, make sure path is correct
                                    if (strcmp(ctx->games.current_path, ".") == 0)
                                        strcpy(ctx->games.current_path, "./games");
                                    // Clear and rescan directory
                                    clear_game_list(&ctx->games);
                                    scan_directory(ctx->games.current_path, &ctx->games);
                                    // Reset selection
                                    ctx->games.current_index = 0;
                                    ctx->games.scroll_offset = 0;
                                }
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                    
                case MENU_CONTROLS:
                    // Controls screen - any key goes back
                    if (event->key.keysym.sym == SDLK_ESCAPE || 
                        event->key.keysym.sym == SDLK_RETURN) {
                        ctx->state = MENU_MAIN;
                    }
                    break;
                    
                default:
                    break;
            }
        }
    }

    // Scan directory for games and folders
    void scan_directory(const char *dir_path, GameList *games) {
        DIR *dir;
        struct dirent *entry;
        
        dir = opendir(dir_path);
        if (!dir) {
            fprintf(stderr, "Error: Could not open directory %s\n", dir_path);
            return;
        }
        
        // First, add a "Back" option if not in root games directory
        if (strcmp(dir_path, "./games") != 0) {
            char parent_path[MAX_PATH_LENGTH];
            strcpy(parent_path, dir_path);
            char *last_slash = strrchr(parent_path, '/');
            if (last_slash) {
                *last_slash = '\0';
                if (strcmp(parent_path, ".") == 0)
                    strcpy(parent_path, "./games");
            }
            add_game_to_list(games, "..", parent_path, true);
        }
        
        // Then scan for folders and games
        while ((entry = readdir(dir)) != NULL) {
            // Skip . and ..
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            
            char full_path[MAX_PATH_LENGTH];
            snprintf(full_path, MAX_PATH_LENGTH, "%s/%s", dir_path, entry->d_name);
            
            // Check if it's a directory
            struct stat path_stat;
            stat(full_path, &path_stat);
            
            if (S_ISDIR(path_stat.st_mode)) {
                // Add folder to list
                add_game_to_list(games, entry->d_name, full_path, true);
            } else {
                // Check if file has .ch8 extension
                char *ext = strrchr(entry->d_name, '.');
                if (ext && strcasecmp(ext, ".ch8") == 0) {
                    // Format the name for display (remove extension)
                    char display_name[256];
                    format_game_name(display_name, entry->d_name);
                    // Add game to list
                    add_game_to_list(games, display_name, full_path, false);
                }
            }
        }
        
        closedir(dir);
    }

    // Add a game or folder to the list
    void add_game_to_list(GameList *games, const char *name, const char *path, bool is_folder) {
        GameItem *new_item = (GameItem *)malloc(sizeof(GameItem));
        if (!new_item) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            return;
        }
        
        strncpy(new_item->name, name, sizeof(new_item->name) - 1);
        new_item->name[sizeof(new_item->name) - 1] = '\0';
        
        strncpy(new_item->path, path, sizeof(new_item->path) - 1);
        new_item->path[sizeof(new_item->path) - 1] = '\0';
        
        new_item->is_folder = is_folder;
        new_item->next = NULL;
        
        // Add to end of list
        if (games->items == NULL) {
            games->items = new_item;
        } else {
            GameItem *current = games->items;
            while (current->next != NULL)
                current = current->next;
            current->next = new_item;
        }
        
        games->count++;
    }

    // Clear the game list
    void clear_game_list(GameList *games) {
        GameItem *current = games->items;
        while (current != NULL) {
            GameItem *next = current->next;
            free(current);
            current = next;
        }
        
        games->items = NULL;
        games->count = 0;
        games->current_index = 0;
        games->scroll_offset = 0;
    }

    // Format game name for display
    void format_game_name(char *dest, const char *src) {
        // Copy source to destination
        strcpy(dest, src);
        
        // Remove .ch8 extension if present
        char *ext = strrchr(dest, '.');
        if (ext && strcasecmp(ext, ".ch8") == 0)
            *ext = '\0';
        
        // Replace underscores with spaces
        for (char *p = dest; *p; p++) {
            if (*p == '_')
                *p = ' ';
        }
    }

    // Launch a game
    void launch_game(const char *game_path) {
        char command[512];
        snprintf(command, sizeof(command), "./chip8 \"%s\"", game_path);
        system(command);
    }

    // Render the main menu
    void render_main_menu(MenuContext *ctx, SDL_Renderer *renderer) {
        // Render logo at top
        SDL_Rect logo_rect = {SCREEN_WIDTH / 2 - 200, 100, 400, 60};
        SDL_RenderCopy(renderer, ctx->logo, NULL, &logo_rect);
        
        // Render menu items
        for (int i = 0; i < (int)MAIN_MENU_ITEMS_COUNT; i++) {
            SDL_Color color = (i == ctx->games.current_index) ? 
                            (SDL_Color){COLOR_SELECTED} : 
                            (SDL_Color){COLOR_TEXT};
            
            SDL_Texture *text_texture = create_text_texture(
                renderer, ctx->menu_font, main_menu_items[i].label, color);
            
            int text_width, text_height;
            SDL_QueryTexture(text_texture, NULL, NULL, &text_width, &text_height);
            
            SDL_Rect text_rect = {
                SCREEN_WIDTH / 2 - text_width / 2,
                300 + i * 60,
                text_width,
                text_height
            };
            
            SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
            SDL_DestroyTexture(text_texture);
        }
        
        // Render status bar
        SDL_SetRenderDrawColor(renderer, COLOR_STATUS_BAR);
        SDL_Rect status_bar = {0, SCREEN_HEIGHT - 40, SCREEN_WIDTH, 40};
        SDL_RenderFillRect(renderer, &status_bar);
        
        // Render status text
        SDL_Color status_color = {COLOR_TEXT};
        SDL_Texture *status_texture = create_text_texture(
            renderer, ctx->menu_font, "Arrow keys: Navigate    Enter: Select    Esc: Exit", status_color);
        
        int status_width, status_height;
        SDL_QueryTexture(status_texture, NULL, NULL, &status_width, &status_height);
        
        SDL_Rect status_rect = {
            SCREEN_WIDTH / 2 - status_width / 2,
            SCREEN_HEIGHT - 35,
            status_width,
            status_height
        };
        
        SDL_RenderCopy(renderer, status_texture, NULL, &status_rect);
        SDL_DestroyTexture(status_texture);
    }

    // Render the game browser
    void render_game_browser(MenuContext *ctx, SDL_Renderer *renderer) {
        // Render title
        SDL_Color title_color = {COLOR_TITLE};
        
        // Create breadcrumb navigation
        char title[256];
        snprintf(title, sizeof(title), "CHIP-8 EMULATOR > GAMES%s", 
                strcmp(ctx->games.current_path, "./games") == 0 ? "" : " > ...");
        
        SDL_Texture *title_texture = create_text_texture(
            renderer, ctx->title_font, title, title_color);
        
        int title_width, title_height;
        SDL_QueryTexture(title_texture, NULL, NULL, &title_width, &title_height);
        
        SDL_Rect title_rect = {
            SCREEN_WIDTH / 2 - title_width / 2,
            50,
            title_width,
            title_height
        };
        
        SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
        SDL_DestroyTexture(title_texture);
        
        // Render game list
        int y_pos = 150;
        GameItem *current = ctx->games.items;
        
        // Skip items before scroll offset
        for (int i = 0; i < ctx->games.scroll_offset && current; i++)
            current = current->next;
        
        // Render visible items
        for (int i = 0; i < MAX_ITEMS_VISIBLE && current; i++) {
            // Determine color based on selection and item type
            SDL_Color color;
            if (ctx->games.scroll_offset + i == ctx->games.current_index) {
                color = (SDL_Color){COLOR_SELECTED};
            } else if (current->is_folder) {
                color = (SDL_Color){COLOR_FOLDER};
            } else {
                color = (SDL_Color){COLOR_TEXT};
            }
            
            // Create display text with folder indicator
            char display_text[300];
            if (current->is_folder) {
                snprintf(display_text, sizeof(display_text), "[FOLDER] %s", current->name);
            } else {
                strcpy(display_text, current->name);
            }
            
            SDL_Texture *text_texture = create_text_texture(
                renderer, ctx->menu_font, display_text, color);
            
            int text_width, text_height;
            SDL_QueryTexture(text_texture, NULL, NULL, &text_width, &text_height);
            
            SDL_Rect text_rect = {
                SCREEN_WIDTH / 2 - text_width / 2,
                y_pos,
                text_width,
                text_height
            };
            
            SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
            SDL_DestroyTexture(text_texture);
            
            y_pos += 50;
            current = current->next;
        }
        
        // Render scroll indicators if needed
        if (ctx->games.scroll_offset > 0) {
            SDL_Color scroll_color = {COLOR_TEXT};
            SDL_Texture *up_texture = create_text_texture(
                renderer, ctx->menu_font, "▲ More games above", scroll_color);
            
            int up_width, up_height;
            SDL_QueryTexture(up_texture, NULL, NULL, &up_width, &up_height);
            
            SDL_Rect up_rect = {
                SCREEN_WIDTH / 2 - up_width / 2,
                120,
                up_width,
                up_height
            };
            
            SDL_RenderCopy(renderer, up_texture, NULL, &up_rect);
            SDL_DestroyTexture(up_texture);
        }
        
        if (ctx->games.scroll_offset + MAX_ITEMS_VISIBLE < ctx->games.count) {
            SDL_Color scroll_color = {COLOR_TEXT};
            SDL_Texture *down_texture = create_text_texture(
                renderer, ctx->menu_font, "▼ More games below", scroll_color);
            
            int down_width, down_height;
            SDL_QueryTexture(down_texture, NULL, NULL, &down_width, &down_height);
            
            SDL_Rect down_rect = {
                SCREEN_WIDTH / 2 - down_width / 2,
                y_pos,
                down_width,
                down_height
            };
            
            SDL_RenderCopy(renderer, down_texture, NULL, &down_rect);
            SDL_DestroyTexture(down_texture);
        }
        
        // Render status bar
        SDL_SetRenderDrawColor(renderer, COLOR_STATUS_BAR);
        SDL_Rect status_bar = {0, SCREEN_HEIGHT - 40, SCREEN_WIDTH, 40};
        SDL_RenderFillRect(renderer, &status_bar);
        
        // Render status text
        SDL_Color status_color = {COLOR_TEXT};
        SDL_Texture *status_texture = create_text_texture(
            renderer, ctx->menu_font, "↑↓: Navigate    Enter: Select    Esc: Back", status_color);
        
        int status_width, status_height;
        SDL_QueryTexture(status_texture, NULL, NULL, &status_width, &status_height);
        
        SDL_Rect status_rect = {
            SCREEN_WIDTH / 2 - status_width / 2,
            SCREEN_HEIGHT - 35,
            status_width,
            status_height
        };
        
        SDL_RenderCopy(renderer, status_texture, NULL, &status_rect);
        SDL_DestroyTexture(status_texture);
    }

    // Render the controls screen
    void render_controls(MenuContext *ctx, SDL_Renderer *renderer) {
        // Render title
        SDL_Color title_color = {COLOR_TITLE};
        SDL_Texture *title_texture = create_text_texture(
            renderer, ctx->title_font, "CHIP-8 EMULATOR > CONTROLS", title_color);
        
        int title_width, title_height;
        SDL_QueryTexture(title_texture, NULL, NULL, &title_width, &title_height);
        
        SDL_Rect title_rect = {
            SCREEN_WIDTH / 2 - title_width / 2,
            50,
            title_width,
            title_height
        };
        
        SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
        SDL_DestroyTexture(title_texture);
        
        // Render controls information
        const char *controls[] = {
            "EMULATOR CONTROLS:",
            "",
            "ESC - Quit the emulator",
            "SPACE - Pause/Resume emulation",
            "H - Reset the current game",
            "J/K - Decrease/Increase color transition rate",
            "O/P - Decrease/Increase volume",
            "N/M - Increase/Decrease frame rate",
            "",
            "GAME CONTROLS (CHIP-8 Keypad):",
            "",
            "1 2 3 4    ->    1 2 3 C",
            "Q W E R    ->    4 5 6 D",
            "A S D F    ->    7 8 9 E",
            "Z X C V    ->    A 0 B F"
        };
        
        int y_pos = 150;
        for (size_t i = 0; i < sizeof(controls) / sizeof(controls[0]); i++) {
            SDL_Color color = {COLOR_TEXT};
            if (i == 0 || i == 9) color = (SDL_Color){COLOR_TITLE}; // Headings
            
            SDL_Texture *text_texture = create_text_texture(
                renderer, ctx->menu_font, controls[i], color);
            
            int text_width, text_height;
            SDL_QueryTexture(text_texture, NULL, NULL, &text_width, &text_height);
            
            SDL_Rect text_rect = {
                SCREEN_WIDTH / 2 - text_width / 2,
                y_pos,
                text_width,
                text_height
            };
            
            SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
            SDL_DestroyTexture(text_texture);
            
            y_pos += 40;
        }
        
        // Render status bar
        SDL_SetRenderDrawColor(renderer, COLOR_STATUS_BAR);
        SDL_Rect status_bar = {0, SCREEN_HEIGHT - 40, SCREEN_WIDTH, 40};
        SDL_RenderFillRect(renderer, &status_bar);
        
        // Render status text
        SDL_Color status_color = {COLOR_TEXT};
        SDL_Texture *status_texture = create_text_texture(
            renderer, ctx->menu_font, "Press ESC or ENTER to return to the main menu", status_color);
        
        int status_width, status_height;
        SDL_QueryTexture(status_texture, NULL, NULL, &status_width, &status_height);
        
        SDL_Rect status_rect = {
            SCREEN_WIDTH / 2 - status_width / 2,
            SCREEN_HEIGHT - 35,
            status_width,
            status_height
        };
        
        SDL_RenderCopy(renderer, status_texture, NULL, &status_rect);
        SDL_DestroyTexture(status_texture);
    }

    // Render a gradient background
    void render_gradient_background(SDL_Renderer *renderer) {
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            // Calculate gradient color
            Uint8 r = 0;
            Uint8 g = 0;
            Uint8 b = 40 + (Uint8)(20.0 * (1.0 - (double)y / SCREEN_HEIGHT));
            Uint8 a = 255;
            
            SDL_SetRenderDrawColor(renderer, r, g, b, a);
            SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
        }
    }

    // Create a texture from text
    SDL_Texture* create_text_texture(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color) {
        SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
        if (!surface) {
            fprintf(stderr, "Error creating text surface: %s\n", TTF_GetError());
            return NULL;
        }
        
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        
        if (!texture) {
            fprintf(stderr, "Error creating texture: %s\n", SDL_GetError());
            return NULL;
        }
        
        return texture;
    }

    // Main function
    int main() {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
            return EXIT_FAILURE;
        }
        
        // Initialize SDL_ttf
        if (TTF_Init() < 0) {
            fprintf(stderr, "Error initializing SDL_ttf: %s\n", TTF_GetError());
            SDL_Quit();
            return EXIT_FAILURE;
        }
        
        // Create window
        SDL_Window *window = SDL_CreateWindow(
            "CHIP-8 Emulator",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
        );
        
        if (!window) {
            fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
            TTF_Quit();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        
        // Create renderer
        SDL_Renderer *renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );
        
        if (!renderer) {
            fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        
        // Initialize menu context
        MenuContext ctx;
        init_menu(&ctx, renderer);
        
        // Main loop
        SDL_Event event;
        while (ctx.running) {
            // Handle events
            while (SDL_PollEvent(&event)) {
                handle_menu_input(&ctx, &event);
            }
            
            // Render menu
            render_menu(&ctx, renderer);
            
            // Delay to limit frame rate
            SDL_Delay(16); // ~60 FPS
        }
        
        // Clean up
        cleanup_menu(&ctx);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        
        return EXIT_SUCCESS;
    }
