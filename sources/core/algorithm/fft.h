#ifndef __FFT_H__
#define __FFT_H__

#include <complex>
#include <cmath>

class FFT {
public:
    FFT(unsigned int N);
    ~FFT();
    unsigned int reverse(unsigned int i);
    void run(std::complex<float>* input, std::complex<float>* output, int stride, int offset);

private:
    unsigned int N, which;
    unsigned int log_2_N;
    float pi2;
    unsigned int *reversed;
    std::complex<float>** T;
    std::complex<float>* c[2];
    std::complex<float> t(unsigned int x, unsigned int N);
};

#endif  // !__FFT_H__