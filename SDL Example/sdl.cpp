#include <iostream>
#include <SDL/SDL.h>
#include <time.h>

using namespace std;

SDL_Surface* initVideoDisplay();
void display_bmp(SDL_Surface* screen, const char *file_name);
void displayOverlay(SDL_Surface* screen);

int main(){
    SDL_Surface* screen = initVideoDisplay();
    //display_bmp(screen, "blackbuck.bmp");
    displayOverlay(screen);    

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
    screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
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

void displayOverlay(SDL_Surface* screen){
    SDL_Overlay* my_overlay = SDL_CreateYUVOverlay(640, 480, SDL_YV12_OVERLAY, screen);
    if(my_overlay == NULL){
         cerr << "Sorry Nick" << endl;
         exit(-1);
    }    

    /* Display number of planes */
    printf("Planes: %d\n", my_overlay->planes);

    /* Fill in video data */
    char* y_video_data = new char[my_overlay->pitches[0]];
    char* u_video_data = new char[my_overlay->pitches[1]];
    char* v_video_data = new char[my_overlay->pitches[2]];
    
    
    for(int i = 0; i < my_overlay->pitches[0]; i++){
        y_video_data[i] = 0x10;
    }
    
    for(int i = 0; i < my_overlay->pitches[1]; i++){
        u_video_data[i] = 0x80;
    }
    
    for(int i = 0; i < my_overlay->pitches[2]; i++){
        v_video_data[i] = 0x80;
    }
    

    cout << "Unlock Baby!" << endl;

    /* Fill in pixel data - the pitches array contains the length of a line in each plane */
    SDL_LockSurface(screen);
    int ret = SDL_LockYUVOverlay(my_overlay);
    if (ret < 0){
        cerr << "Couldn't lock!";
        exit(-1);
    }

    memcpy(my_overlay->pixels[0], y_video_data, my_overlay->pitches[0]);
    memcpy(my_overlay->pixels[1], u_video_data, my_overlay->pitches[1]);
    memcpy(my_overlay->pixels[2], v_video_data, my_overlay->pitches[2]);

    /* Draw a single pixel on (x, y) 
    *(my_overlay->pixels[0] + y * my_overlay->pitches[0] + x) = 0x10;
    *(my_overlay->pixels[1] + y/2 * my_overlay->pitches[1] + x/2) = 0x80;
    *(my_overlay->pixels[2] + y/2 * my_overlay->pitches[2] + x/2) = 0x80; 
*/

    SDL_UnlockSurface(screen);
    SDL_UnlockYUVOverlay(my_overlay);
    
    cout << "Createing Rectangle" << endl;

    SDL_Rect video_rect;
    video_rect.x = 0;
    video_rect.y = 0;
    video_rect.w = 640;
    video_rect.h = 480;

    SDL_Flip(screen);
    cerr << "Flip Complete" << endl;
    SDL_DisplayYUVOverlay(my_overlay, &video_rect);
    
    SDL_Delay(3000);
    
    delete[] y_video_data;
    delete[] u_video_data;
    delete[] v_video_data;
}




