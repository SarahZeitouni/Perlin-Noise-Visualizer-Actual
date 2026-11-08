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

//bool ArrChunk_inRange(ArrChunk* chunk, const Vector3 pos) {
//	if (!chunk) return false;
//
//	return
//	(	pos.x - chunk->origin.x >= 0 && pos.x - chunk->origin.x < 16
//	&&	pos.y - chunk->origin.y >= 0 && pos.y - chunk->origin.y < 128
//	&&	pos.z - chunk->origin.z >= 0 && pos.z - chunk->origin.z < 16);
//}

Voxel* ArrChunk_get(ArrChunk* chunk, int x, int y, int z) {
	if (!chunk) return nullptr;

	if (x < 0 || x >= 16 || y < 0 || y >= 128 || z < 0 || z >= 16)
		return nullptr;

	Voxel* v = &chunk->voxels[x + 16*y + 2048*z];
	return v->alive ? v : nullptr;
}

Voxel* ArrChunk_find(ArrChunk* chunk, const Vector3 pos) {
	if (!chunk) return nullptr;
	Vector3 o = Vector3Subtract(Voxel_getOrigin(pos), chunk->origin);
	return ArrChunk_get(chunk, (int)o.x, (int)o.y, (int)o.z);
}

Voxel* ArrChunk_emplaceLocal(ArrChunk* chunk, const Vector3 pos) {
	if (!chunk)
		return nullptr;

	Vector3 o = Voxel_getOrigin(pos);
	if (o.x < 0 || o.x >= 16 || o.y < 0 || o.y >= 128 || o.z < 0 || o.z >= 16)
		return nullptr;

	Voxel* v = chunk->voxels + (int)o.x + 16*(int)o.y + 2048*(int)o.z;
	Voxel_init(v, Vector3Add(pos, chunk->origin));
	v->adj[0] = ArrChunk_get(chunk, (int)o.x, 	(int)o.y, 	(int)o.z-1);
	v->adj[1] = ArrChunk_get(chunk, (int)o.x+1, (int)o.y, 	(int)o.z);
	v->adj[2] = ArrChunk_get(chunk, (int)o.x, 	(int)o.y,	(int)o.z+1);
	v->adj[3] = ArrChunk_get(chunk, (int)o.x-1, (int)o.y, 	(int)o.z);
	v->adj[4] = ArrChunk_get(chunk, (int)o.x, 	(int)o.y-1, (int)o.z);
	v->adj[5] = ArrChunk_get(chunk, (int)o.x, 	(int)o.y+1,	(int)o.z);

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

Voxel* ArrChunk_emplaceFind(ArrChunk* chunk, const Vector3 pos) {
	if (!chunk) return nullptr;
	return ArrChunk_emplaceLocal(chunk, Vector3Subtract(pos, chunk->origin));
}

Voxel* ArrChunk_removeVoxel(ArrChunk* chunk, Voxel* v) {
	if (!chunk || !v || !v->alive) return nullptr;

	v->alive 	= 0;
	v->visible 	= 0;
//	int p[6][2] = { {0, 2}, {1, 3}, {2, 0}, {3, 1}, {4, 5}, {5, 4} };
//	for (int i = 0; i < 6; i ++) {
//		if (v->adj[p[i][0]]) {
//			v->adj[p[i][0]]->adj[p[i][1]] = nullptr;
//			if (v->adj[p[i][0]]->alive && !v->adj[p[i][0]]->visible)
//				ArrChunk_insertVis(chunk, v->adj[p[i][0]]);
//		}
//	}
	return v;
}

Voxel* ArrChunk_removeLocal(ArrChunk* chunk, const Vector3 pos) {
	if (!chunk) return nullptr;

	Vector3 o = Voxel_getOrigin(pos);
	if (o.x < 0 || o.x >= 16 || o.y < 0 || o.y >= 128 || o.z < 0 || o.z >= 16)
		return nullptr;

	return ArrChunk_removeVoxel(chunk, ArrChunk_get(chunk, (int)o.x, (int)o.y, (int)o.z));
}

Voxel* ArrChunk_removeFind(ArrChunk* chunk, const Vector3 pos) {
	if (!chunk) return nullptr;
	return ArrChunk_removeLocal(chunk, Vector3Subtract(pos, chunk->origin));
}

Voxel* ArrChunk_insertVis(ArrChunk* chunk, Voxel* voxel) {
	if (!chunk || !voxel || !Voxel_isVisible(voxel) || voxel->visible) return nullptr;

	while (chunk->vis_size >= chunk->vis_cap) {
		Voxel** temp = (Voxel**)realloc(chunk->visible, sizeof(Voxel*) * chunk->vis_cap * 2);
		if (!temp) {
			printf("Bleh\n");
			return nullptr;
		}

		chunk->visible = temp;
		chunk->vis_cap *= 2;
	}
	chunk->visible[chunk->vis_size] = voxel;
	chunk->vis_size ++;
	voxel->visible = 1;
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
				if (!v || !v->alive || !Voxel_isVisible(v))
					continue;
				v->visible = 0;
				ArrChunk_insertVis(chunk, v);
			}
		}
	}
}

void ArrChunk_rotateVis(ArrChunk* chunk, const Vector3 axis, const float rotation_speed) {
	if (!chunk) return;

	for (int i = 0; i < chunk->vis_size; i ++) {
		Voxel* v = chunk->visible[i];
		if (!v || !v->alive)
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
		if (!v || !v->alive)
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
		if (!v || !v->alive)
			continue;

		Voxel_resetDisplay(v);
	}
}

void ArrChunk_drawVis(ArrChunk* chunk, float x_offset, float y_offset, float scale) {
	if (!chunk) return;

	for (int i = 0; i < chunk->vis_size; i ++) {
		Voxel* v = chunk->visible[i];
		if (!v || !v->alive)
			continue;

		Voxel_draw(v, x_offset, y_offset, scale);
	}
}

Vector3 ArrChunk_getOrigin(const Vector3 pos) {
	Vector3 o = Voxel_getOrigin(pos);
    return (Vector3){
		(int)o.x < 0 && (int)o.x % 16 	!= 0 ? ((int)o.x/16)*16 - 16 	: ((int)o.x/16)*16,
		(int)o.y < 0 && (int)o.y % 128 	!= 0 ? ((int)o.y/128)*128 - 128 	: ((int)o.y/128)*128,
		(int)o.z < 0 && (int)o.z % 16 	!= 0 ? ((int)o.z/16)*16 - 16 	: ((int)o.z/16)*16
    };
}
