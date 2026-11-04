#pragma once


typedef struct{
    float x, y;
} vector2;

vector2 randomGradient(int ix, int iy);

float dotGridGradient(int ix, int iy, float x, float y);

float lerp(float a0, float a1, float w);

float perlin(float x, float y);