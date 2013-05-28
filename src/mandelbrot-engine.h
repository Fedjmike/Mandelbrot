#pragma once

#include <complex>
#include <memory>
#include <list>
#include <iostream>

#include <pthread.h>

#include "mandelbrot-map.h"
#include "mandelbrot-viewer.h"

namespace mandelbrot {
    template<class T>
    class Engine {
        private:
            Viewer<T>* viewer;

            std::list<pthread_t> threads;
            int threadno;
            bool threadsEnd; ///< Commands the threads to end

            std::complex<T> topleft, bottomright;

            int itermax;
            int escapeRadius;

            std::vector<std::complex<T>> c;
            std::vector<std::complex<T>> z;
            std::vector<int> iters;
            int pitch = 1;
            T step = 0;

        public:
            Engine (int nthreadno,
                    int nitermax,
                    int nescapeRadius):
                viewer(nullptr),
                threads(), threadno(nthreadno), threadsEnd(false),
                topleft({0, 0}), bottomright({0, 0}),
                itermax(nitermax), escapeRadius(nescapeRadius),
                c(), z(), iters() {
            }

            void acceptViewer (Viewer<T>* nviewer) {
                viewer = nviewer;
            }

            void start () {
                for (int i = 0; i < threadno; i++) {
                    pthread_t t;
                    pthread_create(&t, NULL,
                                   (void* (*)(void*)) threadMainDelegate, new threadArgs({this, i}));
                    threads.push_back(t);
                }
            }

            void join () {
                threadsEnd = true;

                for (auto& thread: threads)
                    pthread_join(thread, NULL);

                puts("Engine threads joined");

                viewer->receive(Map<T>({
                    Chunk<T>(topleft, step, pitch, itermax, escapeRadius, z, iters)
                }));
            }

            //

            void samplerate (int n) {
                pitch = n;
            }

            void move (std::complex<T> ntopleft, std::complex<T> nbottomright) {
                topleft = ntopleft;
                bottomright = nbottomright;

                initialize();
            }

            void zoom (int exp) {
                auto centre = (topleft+bottomright)/2;
                topleft = centre + (topleft-centre)*pow(2, exp);
                bottomright = centre + (centre-topleft);
            }

            void translate (std::complex<T> offset) {
                topleft += offset;
                bottomright += offset;
            }

        private:
            typedef struct {
                Engine* engine;
                int id;
            } threadArgs;

            Engine (const Engine<T>&) = delete;
            void operator= (Engine<T>&) = delete;

            void initialize () {
                step = (bottomright-topleft).real()/pitch;
                int height = (bottomright-topleft).imag()/step;

                for (int j = 0; j < pitch; j++) {
                    for (int i = 0; i < height; i++) {
                        c.push_back(topleft + std::complex<T>(i, j)*step);
                        //std::cout << i << ", " << j << ": " << c.back() << "\n";
                        z.push_back(0);
                        iters.push_back(0);
                    }
                }
            }

            static void threadMainDelegate (threadArgs* args) {
                args->engine->threadMain(args);
                delete args;
            }

            void threadMain (threadArgs* args) {
                printf("+ Engine worker #%d\n", args->id);

                for (int i = args->id; i < (int) z.size(); i += threadno) {
                    for (int iterno = 1; iterno <= itermax; iterno++) {
                        z[i] = z[i] * z[i] + c[i];

                        if (abs(z[i]) >= escapeRadius) {
                            iters[i] = iterno;
                            break;
                        }
                    }
                }

                printf("  - Engine worker #%d\n", args->id);
            }
    };
}
