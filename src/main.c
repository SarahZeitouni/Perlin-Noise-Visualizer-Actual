#include "voxel.h"
#include "arrchunk.h"
#include "chunk2dtree.h"
#include "perlinNoiseGen.h"
#include "state.h"
#include "player.h"

#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>



int main(void) {

	SetTraceLogLevel(LOG_NONE);
	
	InitWindow(1380, 850, "Perlin-Noise-Visualizer");
	SetTargetFPS(60);

    State myState;

	while(!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);

		myState.Update();
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
