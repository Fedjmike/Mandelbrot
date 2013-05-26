#include "mandelbrot-engine.h"

using namespace std;
using namespace mandelbrot;

int main (int argc, char** argv) {
    (void) argc, (void) argv;

    int res = 512;
    int samplerate = 1;

    Viewer<double> viewer(make_shared<Engine<double>>(37, 100, 4),
                          res, res, samplerate, samplerate,
                          {-2.0, -1.5}, {1.0, 1.5});

    {
        SDL_Event event;
        do SDL_WaitEvent(&event);
        while (event.type != SDL_QUIT);
    }

    return 0;
}
