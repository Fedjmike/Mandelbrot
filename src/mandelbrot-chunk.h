#include <complex>
#include <vector>

namespace mandelbrot {
    template<class T>
    class Chunk {
        public:
            std::complex<T> topleft;
            T step;
            int pitch;

            int itermax;
            T escapeRadius;

        private:
            std::vector<std::complex<T>> z;
            std::vector<int> iters;

        public:
            Chunk (std::complex<T> ntopleft, T nstep, int npitch,
                   int nitermax, T nescapeRadius,
                   std::vector<std::complex<T>> nz,
                   std::vector<int> niters):
                topleft(ntopleft), step(nstep), pitch(npitch),
                itermax(nitermax), escapeRadius(nescapeRadius),
                z(nz), iters(niters) {
            }

            inline int getPitch () const {return pitch;}

            inline int getItermax () const {return itermax;}
            inline T getEscapeRadius () const {return escapeRadius;}

            inline int getSize () const {return iters.size();}

            inline std::complex<T> getZ (int index) const {return z[index];}
            inline int getIters (int index) const {return iters[index];}
            inline T getLength (int index) const {return abs(z[index]);}
    };
}
