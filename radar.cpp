/*
 * Copyright (C) 2016 Andrew H. Armenia
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <unistd.h>
#include <stdio.h>
#include "ajfft/ajfft.h"

#include <complex>
#include <math.h>

const size_t N_POINTS = 2048;
FFT<float> fft(N_POINTS);
int read_and_process_data( );

int main( ) {
	for (;;) {
		if (read_and_process_data( ) == -1) {
			fprintf(stderr, "Error occurred!\n");
			break;
		}
	}
}

float window(int pos) {
	return 0.54 - 0.46 * cosf( 6.28 * pos / (N_POINTS - 1) );
}

int read_and_process_data( ) {
	float sample_buf[N_POINTS];
	std::complex<float> spectrum[N_POINTS];
	int ret, bytes_read = 0;

	float peak, average, val, freq, speed;
	size_t peak_loc;

	/* read in some audio data */
	while (bytes_read < sizeof(sample_buf)) {
		ret = read(
			STDIN_FILENO, ((char *) sample_buf) + bytes_read, 
			sizeof(sample_buf) - bytes_read
		);
		if (ret == -1) {
			perror("read");
			return -1;
		} else if (ret == 0) {
			fprintf(stderr, "encountered end of input\n");
			return -1;
		}
		
		bytes_read += ret;
	}

	/* multiply by window function */
	for (size_t i = 0; i < N_POINTS; i++) {
		sample_buf[i] *= window(i);
	}

	/* compute the spectrum from the sample buffer */
	fft.compute(spectrum, sample_buf);

	/* find maximum and average magnitudes in the spectrum */
	average = abs(spectrum[0]);

	peak = 0;
	peak_loc = 0;

	for (size_t i = 1; i < N_POINTS / 2; i++) { 
		val = abs(spectrum[i]);
		average += val;

		/* i > 30: only track targets > ~10mph */
		if (val > peak && i > 30) {
			peak = val;
			peak_loc = i;
		}
	}
	average /= (N_POINTS / 2);

	/* if we have a very strong peak then print it */
	if (peak > 50 * average && peak_loc != 0) {
		freq = peak_loc / (float) N_POINTS * 48000.0f;
		speed = freq / 72.023;	/* 72.023 Hz per mph for a K-band gun */
		fprintf(stderr, "target: bin %d / %f Hz %f mph\n", peak_loc, freq, speed);
	} 
	return 0;
}
