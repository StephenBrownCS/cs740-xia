#include <iostream>
#include <SDL/SDL.h>
#include <time.h>

using namespace std;

SDL_Surface* initVideoDisplay();
void display_bmp(SDL_Surface* screen, const char *file_name);


int main(){
    SDL_Surface* screen = initVideoDisplay();
    display_bmp(screen, "blackbuck.bmp");
    
    struct timespec tim, tim2;
    tim.tv_sec = 10;
    tim.tv_nsec = 500;

    if(nanosleep(&tim, &tim2) < 0 ){
        cerr << "Not happening bro" << endl;
        return -1;
    }

    return 0;
}

SDL_Surface* initVideoDisplay(){
    SDL_Surface *screen;

    /* Initialize the SDL library */
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr,
                "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    /* Clean up on exit */
    atexit(SDL_Quit);
    
    /*
     * Initialize the display in a 640x480 8-bit palettized mode,
     * requesting a software surface
     */
    screen = SDL_SetVideoMode(640, 480, 8, SDL_SWSURFACE);
    if ( screen == NULL ) {
        fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n",
                        SDL_GetError());
        exit(1);
    }

    return screen;
}


void display_bmp(SDL_Surface* screen, const char *file_name)
{
    SDL_Surface *image;

    /* Load the BMP file into a surface */
    image = SDL_LoadBMP(file_name);
    if (image == NULL) {
        fprintf(stderr, "Couldn't load %s: %s\n", file_name, SDL_GetError());
        return;
    }



    /*
     * Palettized screen modes will have a default palette (a standard
     * 8*8*4 colour cube), but if the image is palettized as well we can
     * use that palette for a nicer colour matching
     */
    if (image->format->palette && screen->format->palette) {
    SDL_SetColors(screen, image->format->palette->colors, 0,
                  image->format->palette->ncolors);
    }

/*
    image = SDL_DisplayFormat(image);
    if (image == NULL){
       cerr << "Sorry, not happening bro" << endl;
       return;
    }
    */

    /* Blit onto the screen surface */
    if(SDL_BlitSurface(image, NULL, screen, NULL) < 0)
        fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
    
    // SDL_UpdateRect doesn't work!
    SDL_UpdateRect(screen, 0, 0, image->w, image->h);
    
    // For some reason, we need to call SDL_Flip
    SDL_Flip(screen);
    //SDL_Delay(3000);


    /* Free the allocated BMP surface */
    SDL_FreeSurface(image);
}
