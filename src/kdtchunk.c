#include "voxel.h"
#include "kdtchunk.h"
#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

KDTVoxelNode* KDTVoxelNode_init(KDTVoxelNode* node, const Vector3 origin, const uint8_t axis) {
	if (!node) return nullptr;

	node->voxel = (Voxel*)malloc(sizeof(Voxel));
	Voxel_init(node->voxel, origin);
	node->axis 			= axis;
	node->children[0]	= nullptr;
	node->children[1] 	= nullptr;

	return node;
}

KDTChunk* KDTChunk_init(KDTChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	Vector3 o = KDTChunk_getOrigin(origin);

	chunk->origin 	= o;
	chunk->root 	= nullptr;
	chunk->vis 			= (KDTChunk*)malloc(sizeof(KDTChunk));
	chunk->vis->origin 	= o;
	chunk->vis->root 	= nullptr;

	return chunk;
}

static void KDTChunk_deleteHelper(KDTVoxelNode* node) {
	if (!node) return;

	KDTChunk_deleteHelper(node->children[0]);
	KDTChunk_deleteHelper(node->children[1]);
	free(node->voxel);
	free(node);
}
void KDTChunk_delete(KDTChunk* chunk) {
	if (!chunk) return;

	KDTChunk_deleteHelper(chunk->root);
	chunk->root = nullptr;
	KDTChunk_deleteVis(chunk);
	free(chunk->vis);
}

static KDTVoxelNode* KDTChunk_insertHelper(KDTVoxelNode* node, const Vector3 origin, const uint8_t axis) {
	if (!node) {
		node = (KDTVoxelNode*)malloc(sizeof(KDTVoxelNode));
		KDTVoxelNode_init(node, origin, axis);
		return node;
	}

	float xyz[3] 	= { origin.x, origin.y, origin.z };
	float xyzc[3]	= { node->voxel->origin.x, node->voxel->origin.y, node->voxel->origin.z };
	if (xyz[0] == xyzc[0] && xyz[1] == xyzc[1] && xyz[2] == xyzc[2])
		return node;

	int c = (int)(xyz[node->axis] >= xyzc[node->axis]);
	node->children[c] = KDTChunk_insertHelper(node->children[c], origin, (node->axis + 1)%3);
	return node;
}
void KDTChunk_insertRaw(KDTChunk* chunk, const Vector3 pos) {
	if (!chunk || !KDTChunk_inRangeRaw(chunk, pos)) return;

	chunk->root = KDTChunk_insertHelper(chunk->root, Voxel_getOrigin(pos), X_AXIS);
//	printf("n\n");
}

void KDTChunk_insertLocal(KDTChunk* chunk, const Vector3 origin) {
	if (!chunk || !KDTChunk_inRangeLocal(origin)) return;

	KDTChunk_insertRaw(chunk, Vector3Add(origin, chunk->origin));
}

KDTVoxelNode* KDTChunk_searchRaw(KDTChunk* chunk, const Vector3 pos) {
	if (!chunk) return nullptr;

	Vector3 o = Voxel_getOrigin(pos);

	float xyz[3] = { o.x, o.y, o.z };
	KDTVoxelNode* curr = chunk->root;
	while (curr) {
		float xyzc[3] = { curr->voxel->origin.x, curr->voxel->origin.y, curr->voxel->origin.z };
		if (xyz[0] == xyzc[0] && xyz[1] == xyzc[1] && xyz[2] == xyzc[2])
			return curr;
		int c = (int)(xyz[curr->axis] >= xyzc[curr->axis]);
		curr = curr->children[c];
	}
	return nullptr;
}

Voxel* KDTChunk_searchRawVoxel(KDTChunk* chunk, const Vector3 pos) {
	KDTVoxelNode* node = KDTChunk_searchRaw(chunk, pos);
	return node ? node->voxel : nullptr;
}

KDTVoxelNode* KDTChunk_searchLocal(KDTChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	return KDTChunk_searchRaw(chunk, Vector3Add(origin, chunk->origin));
}

Voxel* KDTChunk_searchLocalVoxel(KDTChunk* chunk, const Vector3 origin) {
	KDTVoxelNode* node = KDTChunk_searchLocal(chunk, origin);
	return node ? node->voxel : nullptr;
}

//void KDTChunk_removeVoxel(KDTChunk* chunk, KDTVoxelNode* voxel);

//void KDTChunk_removeRaw(KDTChunk* chunk, const Vector3 pos);

//void KDTChunk_removeLocal(KDTChunk* chunk, const Vector3 origin);

static void KDTChunk_deleteVisHelper(KDTVoxelNode* node) {
	if (!node) return;

	KDTChunk_deleteVisHelper(node->children[0]);
	KDTChunk_deleteVisHelper(node->children[1]);
	free(node);
}
void KDTChunk_deleteVis(KDTChunk* chunk) {
	if (!chunk) return;

	KDTChunk_deleteVisHelper(chunk->vis->root);
}

static KDTVoxelNode* KDTChunk_insertVisHelper(KDTVoxelNode* node, Voxel* voxel, const uint8_t axis) {
	if (!node) {
		node = (KDTVoxelNode*)malloc(sizeof(KDTVoxelNode));
		node->voxel = voxel;
		node->axis 	= axis;
		node->children[0] = nullptr;
		node->children[1] = nullptr;
		return node;
	}

	float xyz[3] 	= { voxel->origin.x, voxel->origin.y, voxel->origin.z };
	float xyzc[3]	= { node->voxel->origin.x, node->voxel->origin.y, node->voxel->origin.z };
	if (xyz[0] == xyzc[0] && xyz[1] == xyzc[1] && xyz[2] == xyzc[2])
		return node;

	int c = (int)(xyz[node->axis] >= xyzc[node->axis]);
	node->children[c] = KDTChunk_insertVisHelper(node->children[c], voxel, (node->axis + 1)%3);
	return node;
}
void KDTChunk_insertVis(KDTChunk* chunk, Voxel* voxel) {
	if (!chunk || !voxel || !Voxel_isVisible(voxel)) return;

	chunk->vis->root = KDTChunk_insertVisHelper(chunk->vis->root, voxel, X_AXIS);
}

static void KDTChunk_initVisHelper(KDTChunk* chunk, KDTVoxelNode* node) {
	if (!chunk || !node) return;
	printf("0\n");
	KDTChunk_insertVis(chunk, node->voxel);
	printf("1\n");
	KDTChunk_initVisHelper(chunk, node->children[0]);
	printf("2\n");
	KDTChunk_initVisHelper(chunk, node->children[1]);
	printf("3\n");
}
void KDTChunk_initVis(KDTChunk* chunk) {
	if (!chunk) return;

	KDTChunk_deleteVis(chunk);
	printf("d\n");
	KDTChunk_initVisHelper(chunk, chunk->vis->root);
	printf("int\n");
}

static void KDTChunk_rotateVisHelper(KDTVoxelNode* node, const Vector3 axis, const float rotation_speed) {
	if (!node) return;

	Voxel_rotateDisplay(node->voxel, axis, rotation_speed);
	KDTChunk_rotateVisHelper(node->children[0], axis, rotation_speed);
	KDTChunk_rotateVisHelper(node->children[1], axis, rotation_speed);
}
void KDTChunk_rotateVis(KDTChunk* chunk, const Vector3 axis, const float rotation_speed) {
	if (!chunk) return;

	KDTChunk_rotateVisHelper(chunk->vis->root, axis, rotation_speed);
}

static void KDTChunk_translateVisHelper(KDTVoxelNode* node, const Vector3 axis, const float move_speed) {
	if (!node) return;

	Voxel_translateDisplay(node->voxel, axis, move_speed);
	KDTChunk_translateVisHelper(node->children[0], axis, move_speed);
	KDTChunk_translateVisHelper(node->children[1], axis, move_speed);
}
void KDTChunk_translateVis(KDTChunk* chunk, const Vector3 axis, const float move_speed) {
	if (!chunk) return;

	KDTChunk_translateVisHelper(chunk->vis->root, axis, move_speed);
}

static void KDTChunk_resetVisHelper(KDTVoxelNode* node) {
	if (!node) return;

	Voxel_resetDisplay(node->voxel);
	KDTChunk_resetVisHelper(node->children[0]);
	KDTChunk_resetVisHelper(node->children[1]);
}
void KDTChunk_resetVis(KDTChunk* chunk) {
	if (!chunk) return;

	KDTChunk_resetVisHelper(chunk->vis->root);
}

static void KDTChunk_drawVisHelper(KDTVoxelNode* node, float x_offset, float y_offset, float scale) {
	if (!node) return;

	Voxel_draw(node->voxel, x_offset, y_offset, scale);
	KDTChunk_drawVisHelper(node->children[0], x_offset, y_offset, scale);
	KDTChunk_drawVisHelper(node->children[1], x_offset, y_offset, scale);
}
void KDTChunk_drawVis(KDTChunk* chunk, float x_offset, float y_offset, float scale) {
	if (!chunk) return;

	KDTChunk_drawVisHelper(chunk->vis->root, x_offset, y_offset, scale);
}

Vector3 KDTChunk_getOrigin(const Vector3 pos) {
    Vector3 o = Voxel_getOrigin(pos);
    return (Vector3){
        (int)o.x < 0 && (int)o.x % 16   != 0 ? ((int)o.x/16)*16 - 16    : ((int)o.x/16)*16,
        (int)o.y < 0 && (int)o.y % 128  != 0 ? ((int)o.y/128)*128 - 128     : ((int)o.y/128)*128,
        (int)o.z < 0 && (int)o.z % 16   != 0 ? ((int)o.z/16)*16 - 16    : ((int)o.z/16)*16
    };
}

bool KDTChunk_inRangeLocal(const Vector3 origin) {
	Vector3 o = Voxel_getOrigin(origin);
	return o.x >= 0 && o.x < 16 && o.y >= 0 && o.y < 128 && o.z >= 0 && o.z < 16;
}

bool KDTChunk_inRangeRaw(KDTChunk* chunk, const Vector3 pos) {
	if (!chunk) return false;

	return KDTChunk_inRangeLocal(Vector3Subtract(pos, chunk->origin));
}

#undef X_AXIS
#undef Y_AXIS
#undef Z_AXIS
