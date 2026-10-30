#include "voxel.h"
#include "arrchunk.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

ArrChunk* ArrChunk_init(ArrChunk* chunk, const Vector3 origin) {
	chunk->origin = origin;
	chunk->voxels = (Voxel*)calloc(sizeof(Voxel) * 16 * 16 * 128);
	return chunk;
}

void ArrChunk_delete(ArrChunk* chunk) {
	free(chunk->voxels);
	chunk->voxels = nullptr;
}

bool ArrChunk_inRange(ArrChunk* chunk, const Vector3 pos) {
	Vector3 o = Voxel_getOrigin(pos);
	return
	(	o.x - chunk->origin.x >= 0 && o.x - chunk->origin.x < 16
	||	o.y - chunk->origin.y >= 0 && o.y - chunk->origin.y < 16
	||	o.z - chunk->origin.z >= 0 && o.z - chunk->origin.z < 128);
}

Voxel* ArrChunk_get(ArrChunk* chunk, const Vector3 pos) {
	if (! ArrChunk_inRange(chunk, pos))
		return nullptr;
	Vector3 o = Voxel_getOrigin(pos);
	Voxel* v = chunk->voxels + (int)o.x + 16*(int)o.y + 256*(int)o.z;
	for (char* ptr = (char*)v; ptr < v + sizeof(Voxel); ptr ++) {
		if ((*ptr) != 0)
			return v;
	}
	return nullptr;
}

Voxel* ArrChunk_emplace(ArrChunk* chunk, const Vector3 pos) {
	if (! ArrChunk_inRange(chunk, pos))
		return nullptr;
	Vector3 o = Voxel_getOrigin(pos);
	Voxel* v = chunk->voxels + (int)o.x + 16*(int)o.y + 256*(int)o.z;
	Voxel_init(v);
	v->adj[0] = ArrChunk_get(chunk, (Vector3){o.x,	 	o.y-1, 	o.z});
	v->adj[1] = ArrChunk_get(chunk, (Vector3){o.x+1, 	o.y, 	o.z});
	v->adj[2] = ArrChunk_get(chunk, (Vector3){o.x,		o.y+1,	o.z});
	v->adj[3] = ArrChunk_get(chunk, (Vector3){o.x-1,	o.y,	o.z});
	v->adj[4] = ArrChunk_get(chunk, (Vector3){o.x,		o.y,	o.z-1});
	v->adj[5] = ArrChunk_get(chunk, (Vector3){o.x,		o.y,	o.z+1});
}

Voxel* ArrChunk_remove(ArrChunk* chunk, const Vector3 pos) {
	Voxel* v = ArrChunk_get(chunk, pos);
	if (!v)
		return nullptr;
	if (v->adj[0])
		v->adj[0]->adj[2] = nullptr;
	if (v->adj[1])
		v->adj[1]->adj[3] = nullptr;
	if (v->adj[2])
		v->adj[2]->adj[0] = nullptr;
	if (v->adj[3])
		v->adj[3]->adj[1] = nullptr;
	if (v->adj[4])
		v->adj[4]->adj[5] = nullptr;
	if (v->adj[5])
		v->adj[5]->adj[4] = nullptr;
	for (char* ptr = (char*)v; ptr < v + sizeof(Voxel); ptr ++)
		(*ptr) = 0;
	return v;
}
