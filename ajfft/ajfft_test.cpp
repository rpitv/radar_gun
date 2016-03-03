#include <iostream>
#include "ajfft.h"

typedef double fft_type;

int main( ) {
	const size_t sz = 64;
	FFT<fft_type> fft(sz);
	FFT<fft_type> ifft(sz, FFT<fft_type>::INVERSE);

	fft_type input[sz];
	std::complex<fft_type> output[sz];
	std::complex<fft_type> invert[sz];

	/* initialize input to be the sequence 1..sz */
	for (size_t i = 0; i < sz; i++) {
		input[i] = (fft_type)(i+1);
	}

	/* execute FFT */
	fft.compute(output, input);

	/* print results */
	std::cout << "FORWARD TRANSFORM RESULT" << std::endl;
	for (size_t i = 0; i < sz; i++) {
		std::cout << output[i] << std::endl;
	}
	std::cout << std::endl;

	ifft.compute(invert, output);

	std::cout << "INVERSE TRANSFORM RESULT" << std::endl;
	for (size_t i = 0; i < sz; i++) {
		std::cout << std::abs(invert[i]) / sz << std::endl;
	}
	std::cout << std::endl;
}
