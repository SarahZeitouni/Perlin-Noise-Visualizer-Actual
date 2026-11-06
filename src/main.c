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

	State myState;

	InitWindow(1080, 800, "Perlin-Noise-Visualizer");

	while(!WindowShouldClose()) {
		//bunch of if statements
		myState.Update();
	}

	CloseWindow();

	return 0;
}
