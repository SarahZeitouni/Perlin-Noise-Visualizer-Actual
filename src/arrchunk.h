#ifndef ARRCHUNK_H
#define ARRCHUNK_H

#include "voxel.h"
#include <raylib.h>
#include <raymath.h>

typedef struct ArrChunk {
	Vector3	origin;
	Voxel*	voxels;
	Voxel*	visible;
} ArrChunk;

ArrChunk* ArrChunk_init(ArrChunk* chunk, const Vector3 origin);

void ArrChunk_delete(ArrChunk* chunk);

bool ArrChunk_inRange(ArrChunk* chunk, const Vector3 origin);

Voxel* ArrChunk_get(ArrChunk* chunk, const Vector3 pos);

Voxel* ArrChunk_emplace(ArrChunk* chunk, const Vector3 pos);

Voxel* ArrChunk_remove(ArrChunk* chunk, const Vector3 pos);

#endif
