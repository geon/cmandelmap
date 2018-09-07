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

			int iterations = 7;
			for (int i = 0; i < iterations; ++i) {
				z = cAdd(cMul(z, z), c);
			}

			float u = cLength(z)+0.65;
			float v = cAngle(z) / (M_PI * 2);
			u = (u - 0.75) * 0.5; 
			v = (v+0.5) ;

    // fprintf( stdout, "%f %f %u %u \n", u, v, clampAndChar(u), clampAndChar(v) );
	// exit(0);

			Color color = {
				.r = u,
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
