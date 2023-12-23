void update_screen(const sdl_t sdl,const config_t config, const chip8_t chip8)
{   SDL_Rect rect = {.x = 0,.y = 0,.w = config.scale_factor,.h = config.scale_factor};

    // Grab color values to draw

    const uint8_t bg_r = (uint8_t)(config.bg_color >> 24) & 0xFF;
    const uint8_t bg_g = (uint8_t)(config.bg_color >> 16) & 0xFF;
    const uint8_t bg_b = (uint8_t)(config.bg_color >> 8) & 0xFF;
    const uint8_t bg_a = (uint8_t)(config.bg_color >> 0) & 0xFF;

    const uint8_t fg_r = (uint8_t)(config.fg_color >> 24) & 0xFF;
    const uint8_t fg_g = (uint8_t)(config.fg_color >> 16) & 0xFF;
    const uint8_t fg_b = (uint8_t)(config.fg_color >> 8) & 0xFF;
    const uint8_t fg_a = (uint8_t)(config.fg_color >> 0) & 0xFF;

    // Loop through display pixels, draw a rectangle per pixel to the SDL window
    for(uint32_t i = 0 ; i < sizeof chip8.display;i++)
    {
        // turning 1D index i value to 2D X/Y coordinates
        rect.x = (i % config.window_width)*config.scale_factor;
        rect.y = (i / config.window_width)*config.scale_factor;

        if(chip8.display[i])
        {
            SDL_SetRenderDrawColor(sdl.renderer,fg_r,fg_g,fg_b,fg_a);
            SDL_RenderFillRect(sdl.renderer,&rect);
           
        
        if(config.pixel_outlines)
        {
            SDL_SetRenderDrawColor(sdl.renderer,bg_r,bg_g,bg_b,bg_a);
            SDL_RenderDrawRect(sdl.renderer,&rect);
        }

        }else{
            SDL_SetRenderDrawColor(sdl.renderer,bg_r,bg_g,bg_b,bg_a);
            SDL_RenderFillRect(sdl.renderer,&rect);            // else draw bg color
        }
    }

    SDL_RenderPresent(sdl.renderer);
}

