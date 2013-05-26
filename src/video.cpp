#include <SDL/SDL.h>

#include "video.h"

namespace Video {
	SDL_Surface* Screen;



	int Init (int W, int H) {
		if (SDL_Init( SDL_INIT_VIDEO ) < 0){
			printf("Unable to initiate SDL: %s\n", SDL_GetError());
			return 1;
		}

		atexit(SDL_Quit);

		freopen("con", "w", stdout);

		Video::Screen = SDL_SetVideoMode(W, H, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

		if (!Video::Screen) {
			printf("Unable to open window: %s\n", SDL_GetError());
			return 1;
		}

		return 0;
	}

    RgbColor HsvToRgb(HsvColor hsv)
    {
        RgbColor rgb;
        unsigned char region, remainder, p, q, t;

        if (hsv.s == 0)
        {
            rgb.r = hsv.v;
            rgb.g = hsv.v;
            rgb.b = hsv.v;
            return rgb;
        }

        region = hsv.h / 43;
        remainder = (hsv.h - (region * 43)) * 6;

        p = (hsv.v * (255 - hsv.s)) >> 8;
        q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
        t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

        switch (region)
        {
            case 0:
                rgb.r = hsv.v; rgb.g = t; rgb.b = p;
                break;
            case 1:
                rgb.r = q; rgb.g = hsv.v; rgb.b = p;
                break;
            case 2:
                rgb.r = p; rgb.g = hsv.v; rgb.b = t;
                break;
            case 3:
                rgb.r = p; rgb.g = q; rgb.b = hsv.v;
                break;
            case 4:
                rgb.r = t; rgb.g = p; rgb.b = hsv.v;
                break;
            default:
                rgb.r = hsv.v; rgb.g = p; rgb.b = q;
                break;
        }

        return rgb;
    }
}
