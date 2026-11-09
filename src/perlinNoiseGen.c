#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "perlinNoiseGen.h"

typedef struct vector2 {
	float x;
	float y;
} vector2;

static vector2 randomGradient(int ix, int iy, int seed){
    //this is a bit confusing to me, I would like to do my own research into 
    //  hashing random functions
    //(this is also a function that i took from soneone else's hash, 
    //   i did not write this code organically)

    seed = seed % 10;
    if (seed == 0){
        seed += 4;
    }

    const unsigned w  = 8 * sizeof(unsigned);
    const unsigned s  = w/2;
    unsigned a = ix, b = iy;
    a *= 3284157443 * seed;

    b ^= a << s | a >> (w - s);
    b *= 1911520717 * seed;

    a ^= b << s | b >> (w - s);
    a *= 2048419325;
    float random = a * (3.15149265/~(~0u >> 1)); //frm 0 to 2pi

    //create vector from angle
    vector2 v;
    v.x = sin(random);
    v.y = cos(random);

    return v;
}

//to compute dot products of the distance and gradient vectors
static float dotGridGradient(int ix, int iy, float x, float y, int seed){
    vector2 gradient = randomGradient(ix, iy, seed);

    //compute distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    //compute dot prod
    return (dx * gradient.x + dy * gradient.y);
}

static float lerp(float a0, float a1, float w){
    //linear interpolation- make sure everything is smoooth
    //basically what this does is it makes sure that numbers close together
    //  get similar outputs from our random function

    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

static float perlin(float x, float y, int seed){
    //determine grid cell corner coords
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    //compute interpolation points (possibly outsource to helper?)
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    //compare + interpolate top two corners
    float n0 = dotGridGradient(x0, y0,   x, y, seed);
    float n1 = dotGridGradient(x1, y0, x, y, seed);
    float ix0 = lerp(n0, n1, sx);

    //compare + interpolate bottom two corners
    n0 = dotGridGradient(x0, y1, x, y, seed);
    n1 = dotGridGradient(x1, y1, x, y, seed);
    float ix1 = lerp(n0, n1, sx);

    //lerp the final corners
    float value = lerp(ix0, ix1, sy);

    return value;
}

uint8_t* Perlin_generate(const int width, const int height, int seed) {

	uint8_t* pixels = (uint8_t*)calloc(width*height*4, sizeof(uint8_t));

    if (pixels == nullptr) return nullptr;

	const int GRID_SIZE = 400;
    const int OCTAVES = 12;

    float frequencies[OCTAVES];
    float amplitudes[OCTAVES];

    float freq = 1;
    float amp = 1;

    for (int i = 0; i < OCTAVES; i++) {
        frequencies[i] = freq / GRID_SIZE;
        amplitudes[i] = amp;

        freq *= 2.0f;
        amp *= 0.5f;
    }


	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			float val = 0;

			//layering noise on top of each other 12 times
			for (int i = 0; i < OCTAVES; i++){
				val += perlin(x * frequencies[i], y * frequencies[i], seed) * amplitudes[i];
			}
			val *= 1.2;

			//clip from -1 to 1
			if (val > 1.0f)
				val = 1.0f;
			else if (val < -1.0f)
				val = -1.0f;

			//convert (1 to -1) into (255 to 0)
			int index = (y * width + x)*4;
			int color = (int)(((val + 1.0f) * 0.5f) * 255);

			//set pixels color
			pixels[index] 	= color;
			pixels[index+1] = color;
			pixels[index+2] = color;
			pixels[index+3] = 255;
		}
	}

	return pixels;
}
