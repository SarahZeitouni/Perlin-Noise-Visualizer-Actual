#include "voxel.h"
#include "prchunk.h"
#include "prchunktree.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
	PRChunk chunk;
	PRChunk* c1 = PRChunk_init(&chunk, (Vector3){ 0, 0, 0 });
	if (c1)
		printf("c1\n");
	else
		printf("n c1\n");

//	PRVoxelNode* v1 = PRChunk_insertLocal(&chunk, (Vector3){ 0, 0, 0 });
//	if (v1)
//		printf("v1\n");
//	else
//		printf("n v1\n");
//
//	PRVoxelNode* v2 = PRChunk_insertLocal(&chunk, (Vector3){ 1, 0, 0 });
//	if (v2)
//		printf("v2\n");
//	else
//		printf("n v2\n");

	int stop = 0;

	for (int x = 0; x < 16; x ++) {
		for (int z = 0; z < 16; z ++) {
			for (int y = 0; y < 8; y ++) {
				if (!PRChunk_insertLocal(&chunk, (Vector3){ x, y, z })) {
					printf("insrt(%d, %d, %d)\n", x, y, z);
					stop = 1;
					break;
				}
			}
			if (stop) break;
		}
		if (stop) break;
	}

	stop = 0;

	for (int x = 0; x < 16; x ++) {
		for (int z = 0; z < 16; z ++) {
			for (int y = 0; y < 8; y ++) {
				if (!PRChunk_searchLocal(&chunk, (Vector3){ x, y, z })) {
					printf("srch(%d, %d, %d)\n", x, y, z);
					stop = 1;
					break;
				}
			}
			if (stop) break;
		}
		if (stop) break;
	}

	return 0;
}
