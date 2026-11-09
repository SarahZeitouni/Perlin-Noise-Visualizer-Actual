#include "voxel.h"
#include "kdtchunk.h"
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdlib.h>

//typedef struct KDTVoxelNode {
//	Voxel* 	voxel;
//	uint8_t	axis;
//	struct 	KDTVoxelNode* children[2];
//} KDTVoxelNode;
//
//typedef struct KDTChunk {
//	Vector3		origin;
//	KDTVoxelNode*	root;
//	struct KDTChunk* vis;
//} KDTChunk;

KDTVoxelNode* KDTVoxelNode_init(KDTVoxelNode* node, const Vector3 origin, const uint8_t axis) {
	if (!node) return nullptr;

	node->voxel = (Voxel*)malloc(sizeof(Voxel));
	Voxel_init(node->voxel, origin);
	node->axis = axis;
	node->children[0] = nullptr;
	node->children[1] = nullptr;

	return node;
}

KDTVoxelNode* KDTVoxelNode_initVoxel(KDTVoxelNode* node, Voxel* voxel, const uint8_t axis) {
	if (!node || !voxel) return nullptr;

	node->voxel = voxel;
	node->axis = axis;
	node->children[0] = nullptr;
	node->children[1] = nullptr;

	return node;
}

void KDTVoxelNode_clear(KDTVoxelNode* node) {
	if (!node) return;

	free(node->voxel);
	free(node->children);
}
void KDTVoxelNode_delete(KDTVoxelNode* node) {
	if (!node) return;

	KDTVoxelNode_clear(node);
	free(node);
}

KDTChunk* KDTChunk_init(KDTChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	chunk->origin 	= KDTChunk_getOrigin(origin);
	chunk->root 	= nullptr;
	chunk->vis 			= (KDTChunk*)malloc(sizeof(KDTChunk));
	chunk->vis->origin 	= chunk->origin;
	chunk->vis->root 	= nullptr;

	return chunk;
}

void KDTChunk_clear(KDTChunk* chunk) {
	if (!chunk) return;

	free(chunk->root);
	free(chunk->vis->root);
	free(chunk->vis);
}

void KDTChunk_delete(KDTChunk* chunk) {
	if (!chunk) return;

	KDTChunk_clear(chunk);
	free(chunk);
}

static KDTVoxelNode* KDTChunk_insertHelper(KDTVoxelNode* node, const uint8_t axis, Voxel* voxel, KDTVoxelNode** writeback) {
	if (!voxel) {
		if (writeback)
			*writeback = nullptr;
		return node;
	}

	if (!node) {
		node = (KDTVoxelNode*)malloc(sizeof(KDTVoxelNode));
		KDTVoxelNode_initVoxel(node, voxel, axis);
		if (writeback)
			*writeback = node;
		return node;
	}

	float voxel_o[3] = { voxel->origin.x, voxel->origin.y, voxel->origin.z };
	float node_o[3] = { node->voxel->origin.x, node->voxel->origin.y, node->voxel->origin.z };

	int c = voxel_o[axis] >= node_o[axis];
	node->children[c] = KDTChunk_insertHelper(node->children[c], (axis+1)%3, voxel, writeback);
	return node;
}
KDTVoxelNode* KDTChunk_insertRaw(KDTChunk* chunk, const Vector3 pos) {
	if (!chunk || !KDTChunk_inRangeRaw(chunk, pos)) return nullptr;

	Voxel* voxel = (Voxel*)malloc(sizeof(Voxel));
	Voxel_init(voxel, pos);

	KDTVoxelNode* writeback = nullptr;

	chunk->root = KDTChunk_insertHelper(chunk->root, 0, voxel, &writeback);

	if (!writeback) {
		free(voxel);
		voxel = nullptr;
	}

	return writeback;
}

KDTVoxelNode* KDTChunk_insertLocal(KDTChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	return KDTChunk_insertRaw(chunk, Vector3Add(chunk->origin, origin));
}

static void KDTChunk_searchHelper(KDTVoxelNode* node, const Vector3 origin, KDTVoxelNode** writeback) {
	if (!node) {
		if (writeback)
			*writeback = nullptr;
		return;
	}

	float search_o[3] 	= { origin.x, origin.y, origin.z };
	float node_o[3] 	= { node->voxel->origin.x, node->voxel->origin.y, node->voxel->origin.z };

	if (search_o[0] == node_o[0] && search_o[1] == node_o[1] && search_o[2] == node_o[2]) {
		if (writeback)
			*writeback = node;
		return;
	}

	int c = search_o[node->axis] >= node_o[node->axis];
	KDTChunk_searchHelper(node->children[c], origin, writeback);
}
KDTVoxelNode* KDTChunk_searchRaw(KDTChunk* chunk, const Vector3 pos) {
	if (!chunk || !KDTChunk_inRangeRaw(chunk, pos)) return nullptr;

	KDTVoxelNode* writeback = nullptr;
	KDTChunk_searchHelper(chunk->root, Voxel_getOrigin(pos), &writeback);

	return writeback;
}

KDTVoxelNode* KDTChunk_searchLocal(KDTChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	return KDTChunk_searchRaw(chunk, Vector3Add(chunk->origin, origin));
}

Voxel* KDTChunk_searchRaw_Voxel(KDTChunk* chunk, const Vector3 pos) {
	if (!chunk) return nullptr;

	KDTVoxelNode* node = KDTChunk_searchRaw(chunk, pos);
	return node ? node->voxel : nullptr;
}

Voxel* KDTChunk_searchLocal_Voxel(KDTChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	return KDTChunk_searchRaw_Voxel(chunk, origin);
}

static void KDTChunk_initVisHelper(KDTChunk* chunk, KDTVoxelNode* node) {
	if (!chunk || !node) return;

	if (Voxel_isVisible(node->voxel))
		KDTChunk_insertVis(chunk, node->voxel);
	KDTChunk_initVisHelper(chunk, node->children[0]);
	KDTChunk_initVisHelper(chunk, node->children[1]);
}
void KDTChunk_initVis(KDTChunk* chunk) {
	if (!chunk) return;

	KDTChunk_initVisHelper(chunk, chunk->vis->root);
}

void KDTChunk_clearVis(KDTChunk* chunk) {}
void KDTChunk_deleteVis(KDTChunk* chunk) {}

KDTVoxelNode* KDTChunk_insertVis(KDTChunk* chunk, Voxel* voxel) {
	if (!chunk || !voxel || !Voxel_isVisible(voxel)) return nullptr;

	KDTVoxelNode* writeback = nullptr;

	chunk->vis->root = KDTChunk_insertHelper(chunk->vis->root, 0, voxel, &writeback);

	return writeback;
}

void KDTChunk_rotateVis(KDTChunk* chunk, const Vector3 axis, const float rotation_speed) {}
void KDTChunk_translateVis(KDTChunk* chunk, const Vector3 axis, const float move_speed) {}
void KDTChunk_resetVis(KDTChunk* chunk) {}
void KDTChunk_drawVis(KDTChunk* chunk, float x_offset, float y_offset, float scale) {}

Vector3 KDTChunk_getOrigin(const Vector3 pos) {
	Vector3 o = Voxel_getOrigin(pos);

	return (Vector3){
		(int)o.x < 0 && (int)o.x % 16 != 0 ? ((int)o.x/16)*16 - 16 : ((int)o.x/16)*16,
		(int)o.y < 0 && (int)o.y % 128 != 0 ? ((int)o.y/128)*128 - 128 : ((int)o.y/128)*128,
		(int)o.z < 0 && (int)o.z % 16 != 0 ? ((int)o.z/16)*16 - 16 : ((int)o.z/16)*16
	};
}

bool KDTChunk_inRangeLocal(const Vector3 origin) {
	return origin.x >= 0 && origin.x < 16 && origin.y >= 0 && origin.y < 128 && origin.z >= 0 && origin.z < 16;
}

bool KDTChunk_inRangeRaw(KDTChunk* chunk, const Vector3 pos) {
	if (!chunk) return false;

	Vector3 o = Vector3Subtract(pos, chunk->origin);
	return o.x >= 0 && o.x < 16 && o.y >= 0 && o.y < 128 && o.z >= 0 && o.z < 16;
}
