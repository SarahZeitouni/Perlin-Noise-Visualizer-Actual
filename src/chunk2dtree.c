#include "voxel.h"
#include "arrchunk.h"
#include "chunk2dtree.h"
#include "stdint.h"
#include "raymath.h"
#include "stdlib.h"

//typedef struct KDTNode {
//	ArrChunk* chunk;
//	int axis;
//	struct KDTNode* children[2];
//	uint32_t addr_left;
//	uint32_t addr_right;
//} KTNode;
//
//typedef struct ChunkKDT {
//	KDTNode* root;
//	int count;
//} ChunkKDT;

KDTNode* KDTNode_init(KDTNode* node, ArrChunk* chunk, int axis) {
	if (!node) return nullptr;

	node->chunk = chunk;
	node->axis 	= axis;
	node->children[0] = nullptr;
	node->children[1] = nullptr;
	node->addr[0] = 0;
	node->addr[1] = 0;
	return node;
}

ChunkKDT* ChunkKDT_init(ChunkKDT* tree) {
	if (!tree) return nullptr;

	tree->root = nullptr;
	tree->count = 0;
	return tree;
}

static void ChunkKDT_deleteHelper(KDTNode* node) {
	if (!node) return;

	ChunkKDT_deleteHelper(node->children[0]);
	ChunkKDT_deleteHelper(node->children[1]);
	free(node);
}
void ChunkKDT_delete(ChunkKDT* tree) {
	ChunkKDT_deleteHelper(tree->root);
	tree->root = nullptr;
}

KDTNode* ChunkKDT_insert(ChunkKDT* tree, ArrChunk* chunk) {
	if (!tree || !chunk) return nullptr;

	if (!tree->root) {
		tree->root = (KDTNode*)calloc(1, sizeof(KDTNode));
		return KDTNode_init(tree->root, chunk, 0);
	}

	float xz[2] = { chunk->origin.x, chunk->origin.z };
	for (KDTNode* curr = tree->root; curr->chunk->origin.x != xz[0] || curr->chunk->origin.z != xz[1];) {
		int b = xz[curr->axis] >= (float[]){ curr->chunk->origin.x, curr->chunk->origin.z }[curr->axis];
		if (!curr->children[b]) {
			curr->children[b] = (KDTNode*)calloc(1, sizeof(KDTNode));
			tree->count ++;
			return KDTNode_init(curr->children[b], chunk, !curr->axis);
		}
		curr = curr->children[b];
	}
	return nullptr;
}

KDTNode* ChunkKDT_insertNew(ChunkKDT* tree, const Vector3 origin) {
	if (!tree) return nullptr;

	ArrChunk* chunk = (ArrChunk*)calloc(1, sizeof(ArrChunk));
	ArrChunk_init(chunk, origin);
	KDTNode* node = ChunkKDT_insert(tree, chunk);
	if (!node)
		free(chunk);
	return node;
}

KDTNode* ChunkKDT_search(ChunkKDT* tree, const Vector3 origin) {
	if (!tree) return nullptr;

	float xz[2] = { origin.x, origin.z };
	for (KDTNode* curr = tree->root; curr;) {
		float xz_c[2] = { curr->chunk->origin.x, curr->chunk->origin.z };
		if (xz[0] == xz_c[0] && xz[1] == xz_c[1])
			return curr;
		curr = curr->children[xz[curr->axis] >= xz_c[curr->axis]];
	}
	return nullptr;
}

KDTNode* ChunkKDT_find(ChunkKDT* tree, const Vector3 pos) {
	if (!tree) return nullptr;
	return ChunkKDT_search(tree, ArrChunk_getOrigin(pos));
}

static void ChunkKDT_voidChunkHelper(KDTNode* node, void (*func)(ArrChunk*)) {
	if (!node) return;

	func(node->chunk);
	ChunkKDT_voidChunkHelper(node->children[0], func);
	ChunkKDT_voidChunkHelper(node->children[1], func);
}
void ChunkKDT_setVis(ChunkKDT* tree) {
	if (!tree) return;
	ChunkKDT_voidChunkHelper(tree->root, ArrChunk_setVis);
}
void ChunkKDT_resetVis(ChunkKDT* tree) {
	if (!tree) return;
	ChunkKDT_voidChunkHelper(tree->root, ArrChunk_resetVis);
}

static void ChunkKDT_voidChunkV3FHelper(KDTNode* node, const Vector3 axis, const float speed, void (*func)(ArrChunk*, const Vector3, const float)) {
	if (!node) return;

	func(node->chunk, axis, speed);
	ChunkKDT_voidChunkV3FHelper(node->children[0], axis, speed, func);
	ChunkKDT_voidChunkV3FHelper(node->children[1], axis, speed, func);
}
void ChunkKDT_rotateVis(ChunkKDT* tree, const Vector3 axis, const float rotation_speed) {
	if (!tree) return;
	ChunkKDT_voidChunkV3FHelper(tree->root, axis, rotation_speed, ArrChunk_rotateVis);
}
void ChunkKDT_translateVis(ChunkKDT* tree, const Vector3 axis, const float move_speed) {
	if (!tree) return;
	ChunkKDT_voidChunkV3FHelper(tree->root, axis, move_speed, ArrChunk_translateVis);
}

static void ChunkKDT_drawVisHelper(KDTNode* node, float x_offset, float y_offset, float scale) {
	if (!node) return;

	ArrChunk_drawVis(node->chunk, x_offset, y_offset, scale);
	ChunkKDT_drawVisHelper(node->children[0], x_offset, y_offset, scale);
	ChunkKDT_drawVisHelper(node->children[1], x_offset, y_offset, scale);
}
void ChunkKDT_drawVis(ChunkKDT* tree, float x_offset, float y_offset, float scale) {
	if (!tree) return;
	ChunkKDT_drawVisHelper(tree->root, x_offset, y_offset, scale);
}
