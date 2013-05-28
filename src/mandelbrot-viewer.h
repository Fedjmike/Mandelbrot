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

            int width, height, samplerate;

            std::complex<T> topleft, bottomright;

            pthread_t thread;
            bool threadEnd = false;

            pthread_mutex_t drawLock = PTHREAD_MUTEX_INITIALIZER;

        public:
            Viewer (std::shared_ptr<Engine<T>> nengine,
                    int nwidth, int nheight, int nsamplerate,
                    std::complex<T> ntopleft,
                    std::complex<T> nbottomright):
                engine(nengine),
                width(nwidth), height(nheight), samplerate(nsamplerate),
                topleft(ntopleft), bottomright(nbottomright),
                thread() {
                if (Video::Init(width, height) != 0)
                    exit(1);

                engine->acceptViewer(this);
                engine->samplerate(width*samplerate);
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
                               (void* (*)(void*)) threadMainDelegate, this);
            }

            void join () {
                threadEnd = true;
                pthread_join(thread, NULL);
                puts("Viewer thread joined");
            }

            void receive (Map<T> map) {
                pthread_mutex_lock(&drawLock);
                puts("Viewer receiving");

                for (auto chunk: map.getChunks()) {
                    for (int x = 0; x < width; x++) {
                        for (int y = 0; y < height; y++) {
                            int value = 0;

                            for (int i = 0; i < samplerate; i++) {
                                for (int j = 0; j < samplerate; j++) {
                                    int index = (samplerate*x + i) + (samplerate*y + j)*chunk.getPitch();
                                    value +=   chunk.getIters(index) != 0
                                             ? colourIterSmooth(chunk.getIters(index), chunk.getItermax(),
                                                                chunk.getLength(index), chunk.getEscapeRadius())
                                             : 0;
                                }
                            }

                            value /= samplerate*samplerate;

                            Video::Plot(x, y, value << 16);
                        }
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

            static void threadMainDelegate (Viewer* viewer) {
                viewer->threadMain();
            }

            void threadMain () {
                puts("Viewer thread");
                engine->join();
            }
    };
}
