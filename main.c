#include <stdio.h>
#include <math.h>

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
	return atanf(a.r / a.i);
}

// Segfaults if allocated on the stack.
const int width = 1000;
const int height = 1000;
Color pixels[height][width];

int main (int argc, const char * argv[]) {


	float x = -0.7463;
	float y = 0.1102;
	float r = 0.005;

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

			int iterations = 10;
			for (int i = 0; i < iterations; ++i) {
				z = cAdd(cMul(z, z), c);
			}

			float u = cLength(z);
			float v = cAngle(z) / (M_PI * 2);

			Color color = {
				.r = u,
				.g = v,
				.b = 0,
			};

			pixels[y][x] = color;
		}
	}

    fprintf( stdout, "hello world\n" );
}
