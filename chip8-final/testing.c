#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include <stdint.h>
#include<time.h>
#include "SDL.h"
#include<string.h>

typedef struct 
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_AudioSpec want,have;
    SDL_AudioDeviceID dev;
}sdl_t; 


// SDL CONFIGURATION OBJECT

typedef struct
{
    uint32_t window_width;
    uint32_t window_height;
    uint32_t fg_color; 
    uint32_t bg_color; 
    uint32_t scale_factor;
    bool pixel_outlines;

}config_t; 


// EMULATOR STATES
typedef enum
{
    QUIT,
    RUNNING,
    PAUSED,
}emulator_state;


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
typedef struct 
{
    emulator_state state;
    uint8_t ram[4096];      // memory of the chip8 
    bool display[2048];     // Emulate original chip8 resolution
    uint16_t stack[12];     // subroutine stack
    uint16_t *stack_ptr;
    uint8_t V[16];          // Data register from V0-VF ; a storage to store 
    uint16_t I;             // Index register
    uint8_t Delay_timer;    // decreases at 60hz till 0
    uint8_t Sound_timer;    // decreases at 60hz till 0
    bool keypad[16];        // Hexadecimal keypad 0x0-0xF
    const char *rom_name;   // currently running rom
    uint16_t PC;            // program counter
    instruction_t inst;     // currently exectuting instructions

}chip8_t;

#include"performance.h"
#include"UI.h"
#include"innit_chip8.h"
#include"innit_SDL.h"
#include"clear_screen.h"
#include"screen_update.h"
#include "config.h"
#include "end.h"

// Main function starts
int main (int argc ,char ** argv) 
{
if(argc < 2)
{
    fprintf(stderr,"Usage:%s <rom_name>\n",argv[0]);
    exit(EXIT_FAILURE);
}

// initialize emulator config/option
config_t config = {0};
if(!set_config(&config)) exit(EXIT_FAILURE);


// Initialize SDL
sdl_t sdl = {0};
if(!(init_sdl(&sdl,config)))exit(EXIT_FAILURE);


// initialize chip8 machine
chip8_t chip8 = {0};
const char *rom_name = argv[1];
if(!init_chip8(&chip8,rom_name))exit(EXIT_FAILURE);


// initial screen clear to bacckground color
clear_screen (sdl,config);


// Main emulator loop
while(chip8.state != QUIT){
    // handle user input
    handle_input(&chip8);
    if(chip8.state == PAUSED) continue;
    // Emulate chip8 instruction
    emulate_instruction(&chip8,config);


    SDL_Delay(16);


// update Window with changes
update_screen(sdl,config,chip8);
}


//final cleanup
final_cleanup(sdl);
exit(EXIT_SUCCESS);
}