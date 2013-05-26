#pragma once

#include <list>

#include "mandelbrot-chunk.h"

namespace mandelbrot {
    template<class T>
    class Map {
        private:
            std::list<Chunk<T>> chunks;

        public:
            Map (std::list<Chunk<T>> nchunks): chunks(nchunks) {}

            const std::list<Chunk<T>>& getChunks () const {return chunks;}
    };
}
