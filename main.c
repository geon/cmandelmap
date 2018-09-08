#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bmp.h"

typedef struct Color {
	float r;
	float g;
	float b;
} Color;

typedef struct Complex {
	float r;
	float i;
} Complex;

Complex cAdd(Complex a, Complex b) {
	return (Complex) { .r = a.r + b.r, .i = a.i + b.i };
}

Complex cMul(Complex a, Complex  b) {
	return (Complex) {
		.r = a.r * b.r - a.i * b.i,
		.i = a.r * b.i + a.i * b.r,
	};
}

float cLength(Complex a);
float cAngle(Complex a);
Complex cPow(Complex a, float power) {
	float lengthRaised = powf(cLength(a), power);
	float powerAngle = power * cAngle(a);
	return (Complex) {
		.r = lengthRaised * cosf(powerAngle),
		.i = lengthRaised * sinf(powerAngle)
	};
}

 float cLengthSquared(Complex a) {
	return a.r * a.r + a.i * a.i;
}

 float cLength(Complex a) {
	return sqrtf(cLengthSquared(a));
}

 float cAngle(Complex a) {
	return atan2f(a.i, a.r);
}

unsigned char clampAndChar (float a) {
	int b = (int) (a * 255);
	
	if(b < 0) return 0;
	if(b > 255) return 255;

	return b;
}

// https://www.researchgate.net/figure/Common-neural-network-activation-functions_fig7_305881131
float hyperbolicTangentActivation (float z) {
	float posez = expf(+z);
	float negez = expf(-z);
	return (posez - negez) / (posez + negez);
}

// Segfaults if allocated on the stack.
const int width = 1000;
const int height = 1000;
Color pixels[height][width];

int main (int argc, const char * argv[]) {


	// float x = -0.7463;
	// float y = 0.1102;
	// float r = 0.005;
	
	float x = -0.25;
	float y = 0;
	float r = 2;

	float minX = x - r;
	float maxX = x + r;
	float minY = y - r;
	float maxY = y + r;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Complex c = {
				.r = minX + ((maxX - minX) * x) / width,
				.i = minY + ((maxY - minY) * y) / height,
			};

			Complex z = {
				.r = 0,
				.i = 0,
			};

			int i = 0;
			// const int iterations = 20;
			const int iterations = 35;
			const int continousDistanceImprovmentSteps = 4;
			// The traditional Mandelbrot iterations.
			for (; i < iterations - continousDistanceImprovmentSteps && cLengthSquared(z) < 2*2; ++i) {
				z = cAdd(cMul(z, z), c);
			}

			// Do a couple of extra iterations to get a nice, smooth continous distance.
			// http://linas.org/art-gallery/escape/escape.html
			for (int stop = i + continousDistanceImprovmentSteps; i < stop; ++i) {
				z = cAdd(cMul(z, z), c);
			}

			float u = i + 1 - logf(logf(cLength(z))) / logf(2);

			// More iterations until a fixed number, to get the angle.
			const float cutoffDistance = 100;
			for (; i < iterations; ++i) {
				z = cAdd(cMul(z, z), c);

				// Limit the growth of z, or it overflows the floats quickly.
				const float zLength = cLength(z);
				if (zLength > cutoffDistance) {
					z.r = z.r / zLength * cutoffDistance;
					z.i = z.i / zLength * cutoffDistance;
				}
			}

			float w = cLength(z);

			float q = w < 1 ? w : u;
		

			float v = cAngle(z) / (M_PI * 2);
			v = (v+0.5) ;

			Color color = {
				.r = fmod(q, 1),
				.g = v,
				.b = 0,
			};

			pixels[y][x] = color;
		}
	}



    unsigned char image[height][width][3];
    char* imageFileName = "bitmapImage.bmp";

    int i, j;
    for(i=0; i<height; i++){
        for(j=0; j<width; j++){
            image[i][j][2] = clampAndChar(pixels[i][j].r);
            image[i][j][1] = clampAndChar(pixels[i][j].g);
            image[i][j][0] = clampAndChar(pixels[i][j].b);
        }
    }

    generateBitmapImage((unsigned char *)image, height, width, imageFileName);

    fprintf( stdout, "hello world\n" );
}
