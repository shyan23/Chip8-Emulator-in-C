#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include <stdint.h>
#include<time.h>
#include "SDL.h"
#include<string.h>

#define RAM_SIZE 4096
#define DISPLAY_SIZE 2048
#define stack_size 12
#define register_size 16
#define keypad_size 16

// SDL STUFF
typedef struct 
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_AudioSpec want, have;
    SDL_AudioDeviceID dev;
} sdl_t;

//EMULATOR STATES

typedef enum
{
    QUIT,
    RUNNING,
    PAUSED,
}emulator_state_t;

// EXTENSION PARTS

typedef enum 
{
    CHIP8,
    SUPERCHIP,
    XOCHIP,
} extension_t;


typedef struct {
    uint32_t window_width;      // SDL window width
    uint32_t window_height;     // SDL window height
    uint32_t fg_color;          // Foreground color RGBA8888
    uint32_t bg_color;          // Background color RGBA8888
    uint32_t scale_factor;      // Amount to scale a CHIP8 pixel
    bool pixel_outlines;        // Draw pixel "outlines" yes/no
    uint32_t insts_per_second;  // CHIP8 CPU "clock rate" or hz
    uint32_t square_wave_freq;  // Frequency of square wave sound
    uint32_t audio_sample_rate; 
    int16_t volume;             // How loud or not is the sound
    float color_lerp_rate;      // Amount to lerp colors by, between [0.1, 1.0]
    extension_t current_extension;  // Current extension support for e.g. CHIP8 vs. SUPERCHIP
    uint32_t refresh_rate ;
} config_t;

//Chip8 instruction Format
typedef struct 
{
    uint16_t opcode;
    uint16_t NNN;       // 12 bits address
    uint8_t NN;         // 8 bits address
    uint8_t N;          // 4 bits constant
    uint8_t X;          // 4 bit register identifier
    uint8_t Y;          // 4 bit register identifier
}instruction_t;



// CHIP8 Machine object

typedef struct {
    emulator_state_t state;
    uint8_t ram[RAM_SIZE];
    bool display[DISPLAY_SIZE];    // Emulate original CHIP8 resolution pixels
    uint32_t pixel_color[DISPLAY_SIZE];    // CHIP8 pixel colors to draw 
    uint16_t stack[stack_size];     // Subroutine stack
    uint16_t *stack_ptr;
    uint8_t V[register_size];          // Data registers V0-VF
    uint16_t I;             // Index register
    uint16_t PC;            // Program Counter
    uint8_t delay_timer;    // Decrements at 60hz when >0
    uint8_t sound_timer;    // Decrements at 60hz and plays tone when >0 
    bool keypad[keypad_size];        // Hexadecimal keypad 0x0-0xF
    const char *rom_name;   // Currently running ROM
    instruction_t inst;     // Currently executing instruction
    bool draw;              // Update the screen yes/no
} chip8_t;

#include "smooth_transition.h"
#include "audio_callback.h"
#include "initialize_SDL_stuff.h"
#include "initializing_config.h"
#include "initializing_chip8_machine.h"
#include "input.h"
#include "emulate.h"
#include "Draw_sprite.h"
#include "update_timer.h"
#include "clear_screen.h"
#include "close.h"

int main(int argc, char **argv) {
    // Default Usage message for args
    if (argc < 2) {
       fprintf(stderr, "Usage: %s <rom_name>\n", argv[0]);
       exit(EXIT_FAILURE);
    }

    // Initialize emulator configuration/options
    config_t config = {0};
    if (!init_config(&config)) exit(EXIT_FAILURE);

    // Initialize SDL

    sdl_t sdl = {0};
    if (!init_sdl(&sdl, &config)) exit(EXIT_FAILURE);

    // Initialize CHIP8 machine
    chip8_t chip8 = {0};
    const char *rom_name = argv[1];
    if (!init_chip8(&chip8, config, rom_name)) exit(EXIT_FAILURE);

    // Initial screen clear to background color
    clear_screen(sdl, config);

    // Seed random number generator
    srand(time(NULL));

    // Main emulator loop
    while (chip8.state != QUIT) {
        // Handle user input
        handle_input(&chip8, &config);

        if (chip8.state == PAUSED) continue;

        // Get time before running instructions 
        const uint64_t start_frame_time = SDL_GetPerformanceCounter();
        
        // Emulate CHIP8 Instructions for this emulator "frame" (60hz initial)
        for (uint32_t i = 0; i < config.insts_per_second / config.refresh_rate; i++)
            emulate_instruction(&chip8, config);

        // Get time elapsed after running instructions
        const uint64_t end_frame_time = SDL_GetPerformanceCounter();

        // Delay for approximately 60hz/60fps (initial) (16.67ms) or actual time elapsed
        // it can be incremented / decreemented
        const double time_elapsed = (double)((end_frame_time - start_frame_time) * 1000) / SDL_GetPerformanceFrequency();

        SDL_Delay((float)(1000/config.refresh_rate) > time_elapsed ? (float)(1000/config.refresh_rate) - time_elapsed : 0);

        // Update window with changes every changed hz
        if (chip8.draw) {
          pixel_draw(sdl, config, &chip8);
          chip8.draw = false;
        }
        
        // Update delay & sound timers every chnaged hz
        update_timers(sdl, &chip8);
    }

    // Final cleanup
    END(sdl); 

    exit(EXIT_SUCCESS);
}
