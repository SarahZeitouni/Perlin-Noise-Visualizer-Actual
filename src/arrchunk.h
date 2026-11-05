#ifndef ARRCHUNK_H
#define ARRCHUNK_H

#include "voxel.h"
#include <raylib.h>
#include <raymath.h>

typedef struct ArrChunk {
	Vector3	origin;
	Voxel*	voxels;
	int 	vis_size;
	int		vis_cap;
	Voxel**	visible;
} ArrChunk;

ArrChunk* ArrChunk_init(ArrChunk* chunk, const Vector3 origin);

void ArrChunk_delete(ArrChunk* chunk);

//bool ArrChunk_inRange(ArrChunk* chunk, const Vector3 origin);

Voxel* ArrChunk_find(ArrChunk* chunk, const Vector3 pos);

Voxel* ArrChunk_get(ArrChunk* chunk, int x, int y, int z);

Voxel* ArrChunk_emplaceLocal(ArrChunk* chunk, const Vector3 pos);

Voxel* ArrChunk_emplaceFind(ArrChunk* chunk, const Vector3 pos);

Voxel* ArrChunk_removeVoxel(ArrChunk* chunk, Voxel* voxel);

Voxel* ArrChunk_removeLocal(ArrChunk* chunk, const Vector3 pos);

Voxel* ArrChunk_removeFind(ArrChunk* chunk, const Vector3 pos);

Voxel* ArrChunk_insertVis(ArrChunk* chunk, Voxel* voxel);

void ArrChunk_setVis(ArrChunk* chunk);

void ArrChunk_rotateVis(ArrChunk* chunk, const Vector3 axis, const float rotation_speed);

void ArrChunk_translateVis(ArrChunk* chunk, const Vector3 axis, const float move_speed);

void ArrChunk_resetVis(ArrChunk* chunk);

void ArrChunk_drawVis(ArrChunk* chunk, float x_offset, float y_offset, float scale);

Vector3 ArrChunk_getOrigin(const Vector3 pos);

#endif
