#include "ajfft.h"

int main( ) {
	const size_t sz = 1024;
	const size_t nruns = 10000;
	FFT<float> fft(sz);
	float input[sz];
	std::complex<float> output[sz];

	/* initialize input to be the sequence 1..sz */
	for (size_t i = 0; i < sz; i++) {
		input[i] = (float)(i+1);
	}

	for (size_t i = 0; i < nruns; i++) {
		/* execute FFT */
		fft.compute(output, input);
	}
}

