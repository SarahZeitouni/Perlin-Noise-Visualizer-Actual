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

static void ChunkKDT_deleteHelper(KDTNode* node)
	if (!node) return;

	ChunkKDT_deleteHelper(node->children[0]);
	ChunkKDT_deleteHelper(node->children[1]);
	free(node);

void ChunkKDT_delete(ChunkKDT* tree) {
	ChunkKDT_deletHelper(tree->root);
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
		int b = xz[curr->axis] >= (Vector3[]){ curr->chunk->origin.x, curr->chunk->origin.z }[curr->axis];
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
	return ChunkKDT_search(tree, ChunkArr_getOrigin(pos));
}

static void ChunkKDT_drawVisHelper(KDTNode* node, float x_offset, float y_offset, float scale) {
	if (!node) return;

	ArrChunk_drawVis(node->chunk, x_offset, y_offest, scale);
	ChunkKDT_drawVisHelper(node->children[0], x_offset, y_offset, scale);
	ChunkKDT_drawVisHelper(node->children[1], x_offset, y_offset, scale);
}

void ChunkKDT_drawVis(ChunkKDT* tree, float x_offset, float y_offset, float scale) {
	if (!tree) return;
	ChunkKDT_drawVisHelper(tree->root, x_offset, y_offset, scale);
}
