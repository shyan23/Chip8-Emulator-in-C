void drawControls(const char* file) {
 char command[256];
    snprintf(command, sizeof(command), "xdg-open %s &", file);
    system(command);
    
SDL_Delay(1000); 
}
