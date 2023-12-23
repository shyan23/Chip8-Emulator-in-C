bool set_config(config_t *config)
{
    // Set defaults
    *config = (config_t){
        .window_width = 64, //original chip8 resolution
        .window_height = 32,
        .fg_color = 0xFFFFFFFF, // black
        .bg_color = 0x00000000, // white
        .scale_factor = 20,// the resulting resolution will 20X than the default
        .pixel_outlines = true, //Draw pixels
        .inst_per_sec =  800  // number of instructions executed per second
    };
    return true;
}