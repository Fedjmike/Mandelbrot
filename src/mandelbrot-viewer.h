#pragma once

#include <memory>

#include <pthread.h>

#include "video.h"
#include "mandelbrot-map.h"

namespace mandelbrot {
    template<class T>
    class Engine;

    template<class T>
    class Viewer {
        private:
            std::shared_ptr<Engine<T>> engine;

            int width, height;
            int xsamp, ysamp;

            std::complex<T> topleft, bottomright;

            pthread_t thread;
            bool threadEnd = false;

            pthread_mutex_t drawLock = PTHREAD_MUTEX_INITIALIZER;

        public:
            Viewer (std::shared_ptr<Engine<T>> nengine,
                    int nwidth, int nheight,
                    int nxsamp, int nysamp,
                    std::complex<T> ntopleft,
                    std::complex<T> nbottomright):
                engine(nengine),
                width(nwidth), height(nheight),
                xsamp(nxsamp), ysamp(nysamp),
                topleft(ntopleft), bottomright(nbottomright),
                thread(), queue() {
                if (Video::Init(width, height) != 0)
                    exit(1);

                engine->acceptViewer(this);
                engine->move(topleft, bottomright);
                engine->start();
                start();
            }

            ~Viewer () {
                join();
                engine->join();
            }

            void start () {
                pthread_create(&thread, NULL,
                               (void* (*)(void*)) threadMain, this);
            }

            void join () {
                threadEnd = true;
                pthread_join(thread, NULL);
                puts("Viewer thread joined");
            }

            void receive (int phase, int period, Map<T> map) {
                pthread_mutex_lock(&drawLock);
                puts("Viewer receiving");

                for (auto chunk: map.getChunks()) {
                    for (int index = phase; index < chunk.getSize(); index += period) {
                        int y = index % chunk.getPitch(), x = index / chunk.getPitch();
                        Video::Plot(x, y, colourIterSmooth(chunk.getIters(index), chunk.getItermax(),
                                                           chunk.getLength(index), chunk.getEscapeRadius()) << 16);
                    }
                }

                SDL_Flip(Video::Screen);

                puts("Viewer end");
                pthread_mutex_unlock(&drawLock);
            }

        private:
            Viewer (const Viewer<T>&) = delete;
            void operator= (Viewer<T>&) = delete;

            static inline Uint32 colourIterSmooth (int iters, int itermax, T length, T escapeRadius) {
                return sqrt((T) iters - log2(log2(length)/log2(escapeRadius))) * 255 / sqrt(itermax);
            }

            static void threadMain (Viewer* me) {
                (void) me;

                puts("Viewer thread");
            }
    };
}
