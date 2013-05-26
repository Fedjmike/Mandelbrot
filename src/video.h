#ifndef __MDL_H_VIDEO
#define __MDL_H_VIDEO

	#include <SDL/SDL.h>

	namespace Video {
		extern SDL_Surface* Screen;

		int Init (int W, int H);

        typedef struct RgbColor
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
        } RgbColor;

        typedef struct HsvColor
        {
            unsigned char h;
            unsigned char s;
            unsigned char v;
        } HsvColor;

        RgbColor HsvToRgb(HsvColor hsv);

		inline void Plot (int X, int Y, Uint32 C) {
            //printf("[ %i, %i ]\n", X, Y);
            Uint32* Pixel = ( Uint32* ) Video::Screen->pixels + X + Y*Video::Screen->w;
            *Pixel = C;
        }
	}

#endif //__MDL_H_VIDEO
