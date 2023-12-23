bool init_sdl(sdl_t *sdl, config_t *config) 
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("SDL chaloo hoy na! %s\n", SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow
    (
        "CHIP8 Emulator", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        config->window_width * config->scale_factor, 
        config->window_height * config->scale_factor, 
        0
    );

    if (!sdl->window) {
        SDL_Log("window hoy nai %s\n", SDL_GetError());
        return false;
    }

    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
    if (!sdl->renderer) {
        SDL_Log("window Render hochhe na %s\n", SDL_GetError());
        return false;
    }

    // Init Audio stuff
    sdl->want = (SDL_AudioSpec){
        .freq = 44100,          // 44100hz "CD" quality
        .format = AUDIO_S16LSB, // Signed 16 bit little endian
        .channels = 1,          // Mono, 1 channel
        .samples = 512,
        .callback = audio_callback,
        .userdata = config,        // Userdata passed to audio callback
    };

    sdl->dev = SDL_OpenAudioDevice(NULL, 0, &sdl->want, &sdl->have, 0);

    if (sdl->dev == 0) {
        SDL_Log("Audio device khuje paitesina!! %s\n", SDL_GetError());
        return false;
    }

    if ((sdl->want.format != sdl->have.format) ||
        (sdl->want.channels != sdl->have.channels)) {

        SDL_Log("Audio specs mile nai bhai!!\n");
        return false;
    }

    return true;    // Success
}