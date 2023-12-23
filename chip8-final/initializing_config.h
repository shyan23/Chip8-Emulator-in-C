bool init_config(config_t *config) {

    // Set defaults
    *config = (config_t){
        .window_width  = 64,    // CHIP8 original X resolution
        .window_height = 32,    // CHIP8 original Y resolution
        .fg_color = 0xFFFFFFFF, // WHITE
        .bg_color = 0x000000FF, // BLACK
        .scale_factor = 25,     // Default resolution will be 1280x640
        .pixel_outlines = true, // Draw pixel "outlines" by default
        .insts_per_second = 700, // Number of instructions to emulate in 1 second (clock rate of CPU)
        .square_wave_freq = 440,    // 440hz for middle A
        .audio_sample_rate = 44100, // CD quality, 44100hz
        .volume = 3000,             // INT16_MAX would be max volume
        .color_lerp_rate = 0.7,     // Color lerp rate, between [0.1, 1.0]
        .current_extension = CHIP8, // Set default quirks/extension to plain OG CHIP-8
        .refresh_rate = 60          // refresh rate of the CPU
    };


    return true;    // Success
}
