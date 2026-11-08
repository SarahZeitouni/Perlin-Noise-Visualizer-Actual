#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "perlinNoiseGen.h"

typedef struct vector2 {
	float x;
	float y;
} vector2;

static vector2 randomGradient(int ix, int iy){
    //this is a bit confusing to me, I would like to do my own research into
    //  hashing random functions
    //(this is also a function that i took from soneone else's hash,
    //   i did not write this code organically)
    const unsigned w  = 8 * sizeof(unsigned);
    const unsigned s  = w/2;
    unsigned a = ix, b = iy;
    a *= 3284157443;

    b ^= a << s | a >> (w - s);
    b *= 1911520717;

    a ^= b << s | b >> (w - s);
    a *= 2048419325;
    float random = a * (3.15149265/~(~0u >> 1)); //frm 0 to 2pi

    //create vector from angle
    return (vector2){ sin(random), cos(random) };
}

//to compute dot products of the distance and gradient vectors
static float dotGridGradient(int ix, int iy, float x, float y){
    vector2 gradient = randomGradient(ix, iy);

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

static float perlin(float x, float y){
    //determine grid cell corner coords
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    //compute interpolation points (possibly outsource to helper?)
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    //compare + interpolate top two corners
    float n0 = dotGridGradient(x0, y0,   x, y);
    float n1 = dotGridGradient(x1, y0, x, y);
    float ix0 = lerp(n0, n1, sx);

    //compare + interpolate bottom two corners
    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    float ix1 = lerp(n0, n1, sx);

    //lerp the final corners
    float value = lerp(ix0, ix1, sy);

    return value;
}

uint8_t* Perlin_generate(const uint32_t width, const uint32_t height) {

	uint8_t* pixels = (uint8_t*)calloc(width*height*4, sizeof(uint8_t));

	const int GRID_SIZE = 400;

	for (uint32_t x = 0; x < width; x++){
		for (uint32_t y = 0; y < height; y++){
			float val = 0;
			float freq = 1;
			float amp = 1;

			//layering noise on top of each other 12 times
			for (int i = 0; i < 12; i++){
				val += perlin(x * (float)freq/GRID_SIZE, y * (float)freq/GRID_SIZE) * amp;

				freq *=2;
				amp /=2;
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
