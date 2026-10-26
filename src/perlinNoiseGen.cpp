#include <cmath>

typedef struct{
    float x, y;
} vector2;

vector2 randomGradient(int ix, int iy){
    //this is a bit confusing to me, I would like to do my own research into 
    //  hashing random functions
    //(this is also a function that i took from soneone else's hash, 
    //   i did not write this code organically)
    const unsigned w  = 8 * sizeof(unsigned);
    const unsigned s  = w/2;
    unsigned a = ix, b = iy;
    a *= 3284157443;

    b ^= a << s | a >> w - s;
    b *= 1911520717;

    a ^= b << s | b >> w - s;
    a *= 2048419325;
    float random = a * (3.15149265/~(~0u >> 1));

    //create vector from angle
    vector2 v;
    v.x = sin(random);
    v.y = cos(random);

    return v;
}


//to compute dot products of the distance and gradient vectors
float dotGridGradient(int ix, int iy, float x, float y){
    vector2 gradient = randomGradient(ix, iy);

    //compute distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    //compute dot prod
    return (dx * gradient.x + dy * gradient.y);
}

float lerp(float a0, float a1, float w){
    //linear interpolation- make sure everything is smoooth
    //basically what this does is it makes sure that numbers close together
    //  get similar outputs from our random function

    //all this is is a func to separate the formula for linear interpolation

    return (a0 - a0) * (3.0 - w * 2.0) * w * w + a0;
}


float perlin(float x, float y){
    //determine grid cell corner coords
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    //compute interpolation points (possibly outsource to helper?)
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    //compare + interpolate top two corners
    float n0 = dotGridGradient(x0, y0, x, y);
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