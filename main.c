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

float positiveModulo(float a) {
	double foo;
	a = modf(a, &foo);
	if(a < 0) {
		a += 1;
	}
	return a;
}


Color mandelMap (
	float x,
	float y,
	int iterations,
	int continousDistanceImprovmentSteps,
	int textureWidth,
	int textureHeight,
	unsigned char * texture
) {
	Complex c = {
		.r = x,
		.i = y,
	};

	Complex z = {
		.r = 0,
		.i = 0,
	};

	int i = 0;

	// The traditional Mandelbrot iterations.
	for (; i < iterations - continousDistanceImprovmentSteps && cLengthSquared(z) < 2*2; ++i) {
		z = cAdd(cMul(z, z), c);
	}

	// Do a couple of extra iterations to get a nice, smooth continous distance.
	// http://linas.org/art-gallery/escape/escape.html
	for (int stop = i + continousDistanceImprovmentSteps; i < stop; ++i) {
		z = cAdd(cMul(z, z), c);
	}

	// The contious distance. Looks nice outside the Mandlebrot set.
	float outsideLength = i + 1 - logf(logf(cLength(z))) / logf(2);

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

	// This looks nice inside the Mandlebrot set.
	float insideLength = cLength(z);

	// Combining inside and outside. Since `u` gets smaller, while `w` gets
	// larger further away from origo, they can be combined with a simple min.
	float length = fmin(insideLength, outsideLength);

	float angle = cAngle(z) / (M_PI * 2);
	angle = (angle+0.5) ;

	int texU = positiveModulo(length) * (textureWidth - 1);
	int texV = angle * (textureHeight - 1);

	unsigned char *sample = texture + (int)(floorf(texV + 0.5) * textureWidth + floorf(texU + 0.5)) * 4;

	Color color;
	color.r = sample[2] / 255.0;
	color.g = sample[1] / 255.0;
	color.b = sample[0] / 255.0;

	return color;
}


// Segfaults if allocated on the stack.
const int width = 1000;
const int height = 1000;
Color pixels[height][width];

int main (int argc, const char * argv[]) {

	unsigned char *texture;
	int textureHeight;
	int textureWidth;
	// char* textureFileName = "test.bmp";
	// char* textureFileName = "ceramic-tile.bmp";

	// const float x = -0.9;
	// const float y = 0.2573;
	// const float r = 0.0015;
	// const int iterations = 33;
	// const int continousDistanceImprovmentSteps = 3;
	// char* textureFileName = "fruit.bmp";

	// const float x = -0.90065;
	// const float y = 0.2579;
	// const float r = 0.00065;
	// const int iterations = 33;
	// const int continousDistanceImprovmentSteps = 3;
	// char* textureFileName = "ripley.bmp";
	// http://www.chacuncherchesonfilm.fr/storage/films/f0/f03aaad5-d8b9-4cec-b299-ad2b53f266e8.jpg

	// const float x = -0.727;
	// const float y = 0.282;
	// const float r = 0.01;
	// const int iterations = 40;
	// const int continousDistanceImprovmentSteps = 6;
	// char* textureFileName = "rose.bmp";

	// const float x = -0.7335;
	// const float y = 0.3;
	// const float r = 0.17;
	// const int iterations = 11;
	// const int continousDistanceImprovmentSteps = 5;
	// char* textureFileName = "ceramic-tile2.bmp";

	// const float x = -0.5;
	// const float y = 0;
	// const float r = 2;
	// const int iterations = 7;
	// const int continousDistanceImprovmentSteps = 3;
	// char* textureFileName = "snus.bmp";

	// const float x = 0.4;
	// const float y = 0.15;
	// const float r = 0.1;
	// const int iterations = 8;
	// const int continousDistanceImprovmentSteps = 3;
	// char* textureFileName = "slope-tiles.bmp";

	// const float x = -0.815;
	// const float y = -0.205;
	// const float r = 0.005;
	// const float x = -0.811;
	// const float y = -0.2056;
	// const float r = 0.003;
	// const int iterations = 23;
	// const int continousDistanceImprovmentSteps = 3;
	// char* textureFileName = "puzzle.bmp";

	const float x = -0.5;
	const float y = 0;
	const float r = 2;
	const int iterations = 3;
	const int continousDistanceImprovmentSteps = 1;
	char* textureFileName = "slope-tiles.bmp";
	// const float x = -0.5;
	// const float y = 0;
	// const float r = 2;
	// const int iterations = 3;
	// const int continousDistanceImprovmentSteps = 1;
	// char* textureFileName = "slope-tiles.bmp";

	loadBitmapImage(&texture, &textureHeight, &textureWidth, textureFileName);
    unsigned char* image = malloc(height * width * 3);

	float minX = x - r;
	float maxX = x + r;
	float minY = y - r;
	float maxY = y + r;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {

			Color color = mandelMap(
				minX + ((maxX - minX) * x) / width,
				minY + ((maxY - minY) * y) / height,
				iterations,
				continousDistanceImprovmentSteps,
				textureWidth,
				textureHeight,
				texture
			);

            image[(y*width + x) * 3 + 2] = clampAndChar(color.r);
            image[(y*width + x) * 3 + 1] = clampAndChar(color.g);
            image[(y*width + x) * 3 + 0] = clampAndChar(color.b);
		}
	}

    char* imageFileName = "bitmapImage.bmp";
    generateBitmapImage(image, height, width, imageFileName);

    fprintf( stdout, "Done.\n" );
}
