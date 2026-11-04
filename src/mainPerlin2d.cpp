#include <cstdint>
#include <iostream>
#include "raylib.h"
#include "perlinNoiseGen.h"

using namespace std;


static const int windowHeight = 600;
static const int windowWidth = 900;

typedef struct Sample {
    int name;
    unsigned char *pixels;
} sample;

static void show_sample(sample *smp)
{
	InitWindow(windowWidth, windowHeight, "Perlin Noise Generator");
    Image windowImg = {
        .data = smp->pixels,
        .width = windowWidth,
        .height = windowHeight,
        .mipmaps = 1,
		.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE
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

	for (int x = 0; x < windowWidth; x++){
		for (int y = 0; y < windowHeight; y++){
			int index = (y * windowWidth + x) * 4;
			float val = 0;
			float freq = 1;
			float amp = 1;

			for (int i = 0; i < 12; i++){
				val += perlin(x * freq/GRID_SIZE, y * freq*GRID_SIZE) * amp;

				freq *=2;
				amp /=2;
			}
			val *= 1.2;

			if (val > 1.0f)
				val = 1.0f;
			else if (val < -1.0f)
				val = -1.0f;

			//convert (1 to -1) into (255 to 0)
			int color = (int)(((val + 1.0f) * 0.5f) * 255);

			//set pixels color
			pixels[index] = color;
			pixels[index + 1] = color;
			pixels[index + 2] = color;
			pixels[index + 3] = 255;
		}
	}

	sample smp = { .name = 5, .pixels = pixels };

    show_sample(&smp);


	return 0;
}