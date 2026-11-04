#include "voxel.h"
#include "arrchunk.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>

ArrChunk* ArrChunk_init(ArrChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	chunk->origin 	= origin;
	chunk->voxels 	= (Voxel*)calloc(32768, sizeof(Voxel));
	chunk->vis_size = 0;
	chunk->vis_cap	= 128;
	chunk->visible 	= (Voxel**)calloc(chunk->vis_cap, sizeof(Voxel*));
	return chunk;
}

void ArrChunk_delete(ArrChunk* chunk) {
	if (!chunk) return;

	free(chunk->voxels);
	free(chunk->visible);
	chunk->voxels 	= nullptr;
	chunk->visible	= nullptr;
}

bool ArrChunk_inRange(ArrChunk* chunk, const Vector3 pos) {
	if (!chunk) return false;

	return
	(	pos.x - chunk->origin.x >= 0 && pos.x - chunk->origin.x < 16
	&&	pos.y - chunk->origin.y >= 0 && pos.y - chunk->origin.y < 128
	&&	pos.z - chunk->origin.z >= 0 && pos.z - chunk->origin.z < 16);
}

Voxel* ArrChunk_get(ArrChunk* chunk, int x, int y, int z) {
	if (!chunk) return nullptr;

	if (x < 0 || x >= 16 || y < 0 || y >= 128 || z < 0 || z >= 16) {
		return nullptr;
	}

	Voxel* v = &chunk->voxels[x + 16*y + 2048*z];
	if (v->disp_v[0].x == v->disp_v[1].x)
		return nullptr;
	return v;
//	for (unsigned i = 0; i < sizeof(Voxel)/sizeof(char); i ++) {
//		if (*(char*)(v + i) != 0)
//			return v;
//	}
//	return nullptr;
}

Voxel* ArrChunk_find(ArrChunk* chunk, const Vector3 pos) {
	Vector3 o = Voxel_getOrigin(pos);
	return ArrChunk_get(chunk, (int)(o.x - chunk->origin.x), (int)(o.y - chunk->origin.y), (int)(o.z - chunk->origin.z));
}

Voxel* ArrChunk_emplace(ArrChunk* chunk, const Vector3 pos) {
	if (!chunk || !ArrChunk_inRange(chunk, pos))
		return nullptr;

	Vector3 o = Voxel_getOrigin(pos);
	Voxel* v = chunk->voxels + (int)o.x + 16*(int)o.y + 2048*(int)o.z;
	Voxel_init(v, pos);
	v->adj[0] = ArrChunk_find(chunk, (Vector3){o.x,	 	o.y, 	o.z-1});
	v->adj[1] = ArrChunk_find(chunk, (Vector3){o.x+1, 	o.y, 	o.z});
	v->adj[2] = ArrChunk_find(chunk, (Vector3){o.x,		o.y,	o.z+1});
	v->adj[3] = ArrChunk_find(chunk, (Vector3){o.x-1,	o.y,	o.z});
	v->adj[4] = ArrChunk_find(chunk, (Vector3){o.x,		o.y-1,	o.z});
	v->adj[5] = ArrChunk_find(chunk, (Vector3){o.x,		o.y+1,	o.z});

	if (v->adj[0])
		v->adj[0]->adj[2] = v;
	if (v->adj[1])
		v->adj[1]->adj[3] = v;
	if (v->adj[2])
		v->adj[2]->adj[0] = v;
	if (v->adj[3])
		v->adj[3]->adj[1] = v;
	if (v->adj[4])
		v->adj[4]->adj[5] = v;
	if (v->adj[5])
		v->adj[5]->adj[4] = v;
	return v;
}

Voxel* ArrChunk_remove(ArrChunk* chunk, const Vector3 pos) {
	if (!chunk) return nullptr;

	Voxel* v = ArrChunk_find(chunk, pos);
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
	for (unsigned i = 0; i < sizeof(Voxel)/sizeof(char); i ++) {
		if (*(char*)(v + i) != 0)
			return v;
	}
	return nullptr;
}

Voxel* ArrChunk_insertVis(ArrChunk* chunk, Voxel* voxel) {
	if (!chunk || !voxel) return nullptr;

	while (chunk->vis_size + 1 >= chunk->vis_cap) {
		Voxel** temp = (Voxel**)realloc(chunk->visible, sizeof(Voxel*) * chunk->vis_cap * 2);
		if (!temp)
			return nullptr;

		chunk->visible = temp;
		chunk->vis_cap *= 2;
	}
	chunk->visible[chunk->vis_size] = voxel;
	chunk->vis_size ++;
	return voxel;
}

void ArrChunk_setVis(ArrChunk* chunk) {
	if (!chunk) return;

	free(chunk->visible);
	chunk->vis_size = 0;
	chunk->vis_cap 	= 128;
	chunk->visible 	= (Voxel**)calloc(chunk->vis_cap, sizeof(Voxel*));

	for (int x = 0; x < 16; x ++) {
		for (int y = 0; y < 128; y ++) {
			for (int z = 0; z < 16; z ++) {
				Voxel* v = ArrChunk_get(chunk, x, y, z);
				if (!v || !Voxel_isVisible(v))
					continue;
				ArrChunk_insertVis(chunk, v);
			}
		}
	}
}

void ArrChunk_rotateVis(ArrChunk* chunk, const Vector3 axis, const float rotation_speed) {
	if (!chunk) return;

	for (int i = 0; i < chunk->vis_size; i ++) {
		Voxel* v = chunk->visible[i];
		if (!v)
			continue;

		for (int j = 0; j < 8; j ++) {
			v->disp_v[j] = Vector3RotateByAxisAngle(v->disp_v[j], axis, rotation_speed);
		}
	}
}

void ArrChunk_translateVis(ArrChunk* chunk, const Vector3 axis, const float move_speed) {
	if (!chunk) return;

	for (int i = 0; i < chunk->vis_size; i ++) {
		Voxel* v = chunk->visible[i];
		if (!v)
			continue;

		for (int j = 0; j < 8; j ++) {
			v->disp_v[j] = Vector3Add(v->disp_v[j], Vector3Scale(axis, move_speed));
		}
	}
}

void ArrChunk_resetVis(ArrChunk* chunk) {
	if (!chunk) return;

	for (int i = 0; i < chunk->vis_size; i ++) {
		Voxel* v = chunk->visible[i];
		if (!v)
			continue;

		Voxel_resetDisplay(v);
	}
}

void ArrChunk_drawVis(ArrChunk* chunk, float x_offset, float y_offset, float scale) {
	if (!chunk) return;

	for (int i = 0; i < chunk->vis_size; i ++) {
		Voxel* v = chunk->visible[i];
		if (!v)
			continue;

		Voxel_draw(v, x_offset, y_offset, scale);
	}
}
