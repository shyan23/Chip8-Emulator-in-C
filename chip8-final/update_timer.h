void update_timers(const sdl_t sdl, chip8_t *chip8) {
    if (chip8->delay_timer > 0) 
        chip8->delay_timer--;

    if (chip8->sound_timer > 0) {
        chip8->sound_timer--;
        SDL_PauseAudioDevice(sdl.dev, 0); // Play sound
    } else {
        SDL_PauseAudioDevice(sdl.dev, 1); // Pause sound
    }
}