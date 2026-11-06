#include <cstdint>
#include <iostream>
#include "raylib.h"
#include "perlinNoiseGen.h"

using namespace std;


static const int windowHeight = 600;
static const int windowWidth = 900;

typedef struct Sample {
    int name;
    uint8_t *pixels;
} sample;

static void show_sample(sample *smp)
{
	InitWindow(windowWidth, windowHeight, "Perlin Noise Generator");
    Image windowImg = {
        .data = smp->pixels,
        .width = windowWidth,
        .height = windowHeight,
        .mipmaps = 1,
		.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

	Texture2D windowTexture = LoadTextureFromImage(windowImg);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();

			ClearBackground(BLACK);

			DrawTexture(windowTexture, 0, 0, WHITE);
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
    }

    CloseWindow();
}

int main() {

	uint8_t* pixels = new uint8_t[windowHeight * windowWidth * 4];

	const int GRID_SIZE = 400;

	int seed = 383299;

	pixels = Perlin_generate(windowWidth, windowHeight, seed);

	sample smp = { .name = 5, .pixels = pixels };

    show_sample(&smp);

	delete[] pixels;
	return 0;
}