#include "voxel.h"
#include "kdtchunk.h"
#include "kdtchunktree.h"
#include <stdint.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>

//typedef struct KDTChunkNode {
//	KDTChunk* 	chunk;
//	uint8_t 	axis;
//	struct 	KDTChunkNode* children[2];
//} KDTChunkNode;
//
//typedef struct KDTChunkTree {
//	KDTChunkNode* root;
//	struct KDTChunkTree* loaded;
//} KDTChunkTree;

KDTChunkNode* KDTChunkNode_init(KDTChunkNode* node, const Vector3 origin, uint8_t axis) {
	if (!node) return nullptr;

	node->chunk = (KDTChunk*)malloc(sizeof(KDTChunk));
	KDTChunk_init(node->chunk, origin);
	node->axis = axis%3;
	node->children[0] = nullptr;
	node->children[1] = nullptr;

	return node;
}

KDTChunkNode* KDTChunkNode_initChunk(KDTChunkNode* node, KDTChunk* chunk, uint8_t axis) {
	if (!node || !chunk) return nullptr;

	node->chunk = chunk;
	node->axis = axis%3;
	node->children[0] = nullptr;
	node->children[1] = nullptr;

	return node;
}

void KDTChunkNode_clear(KDTChunkNode* node) {
	if (!node) return;

	KDTChunkNode_delete(node->children[0]);
	KDTChunkNode_delete(node->children[1]);
	free(node->children);
	KDTChunk_delete(node->chunk);
}

void KDTChunkNode_delete(KDTChunkNode* node) {
	if (!node) return;

	KDTChunkNode_clear(node);
	free(node);
}

KDTChunkTree* KDTChunkTree_init(KDTChunkTree* tree) {
	if (!tree) return nullptr;

	tree->root = nullptr;
	tree->loaded = (KDTChunkTree*)malloc(sizeof(KDTChunkTree));
	tree->loaded->root = nullptr;

	return tree;
}

void KDTChunkTree_clear(KDTChunkTree* tree) {
	if (!tree) return;

	KDTChunkNode_delete(tree->root);
	KDTChunkTree_deleteLoaded(tree);
}

void KDTChunkTree_delete(KDTChunkTree* tree) {
	if (!tree) return;

	KDTChunkTree_clear(tree);
	free(tree);
}

static KDTChunkNode* KDTChunkTree_insertHelper(KDTChunkNode* node, const uint8_t axis, KDTChunk* chunk, KDTChunkNode** writeback) {
	if (!chunk) {
		if (writeback)
			*writeback = nullptr;
		return node;
	}

	if (!node) {
		node = (KDTChunkNode*)malloc(sizeof(KDTChunkNode));
		KDTChunkNode_initChunk(node, chunk, axis);
		if (writeback)
			*writeback = node;
		return node;
	}

	float chunk_o[3] = { chunk->origin.x, chunk->origin.y, chunk->origin.z };
	float node_o[3]	= { node->chunk->origin.x, node->chunk->origin.y, node->chunk->origin.z };

	if (chunk_o[0] == node_o[0] && chunk_o[1] == node_o[1] && chunk_o[2] == node_o[2]) {
		if (writeback)
			*writeback = nullptr;
		return node;
	}

	int c = chunk_o[node->axis] >= node_o[node->axis];

	node->children[c] = KDTChunkTree_insertHelper(node->children[c], (node->axis+1)%3, chunk, writeback);

	return node;
}
KDTChunkNode* KDTChunkTree_insertRaw(KDTChunkTree* tree, const Vector3 pos) {
	if (!tree) return nullptr;

	KDTChunk* chunk = (KDTChunk*)malloc(sizeof(KDTChunk));
	KDTChunk_init(chunk, pos);

	KDTChunkNode* writeback = nullptr;

	tree->root = KDTChunkTree_insertHelper(tree->root, 0, chunk, &writeback);

	if (!writeback) {
		free(chunk);
		chunk = nullptr;
	}

	if (chunk) {
		Vector3 o = chunk->origin;

		chunk->adj[0] = KDTChunkTree_searchRaw_Chunk(tree, (Vector3){ o.x, o.y, o.z-16 });
		chunk->adj[1] = KDTChunkTree_searchRaw_Chunk(tree, (Vector3){ o.x+16, o.y, o.z });
		chunk->adj[2] = KDTChunkTree_searchRaw_Chunk(tree, (Vector3){ o.x, o.y, o.z+16 });
		chunk->adj[3] = KDTChunkTree_searchRaw_Chunk(tree, (Vector3){ o.x-16, o.y, o.z });
		chunk->adj[4] = KDTChunkTree_searchRaw_Chunk(tree, (Vector3){ o.x, o.y-16, o.z });
		chunk->adj[5] = KDTChunkTree_searchRaw_Chunk(tree, (Vector3){ o.x, o.y+16, o.z });
		if (chunk->adj[0])
			chunk->adj[0]->adj[2] = chunk;
		if (chunk->adj[1])
			chunk->adj[1]->adj[3] = chunk;
		if (chunk->adj[2])
			chunk->adj[2]->adj[0] = chunk;
		if (chunk->adj[3])
			chunk->adj[3]->adj[1] = chunk;
		if (chunk->adj[4])
			chunk->adj[4]->adj[5] = chunk;
		if (chunk->adj[5])
			chunk->adj[5]->adj[4] = chunk;
	}

	return writeback;
}

static void KDTChunkTree_searchHelper(KDTChunkNode* node, const Vector3 origin, KDTChunkNode** writeback) {
	if (!node) {
		if (writeback)
			*writeback = nullptr;
		return;
	}

	float search_o[3] = { origin.x, origin.y, origin.z };
	float node_o[3] = { node->chunk->origin.x, node->chunk->origin.y, node->chunk->origin.z };

	if (search_o[0] == node_o[0] && search_o[1] == node_o[1] && search_o[2] == node_o[2]) {
		if (writeback)
			*writeback = node;
		return;
	}

	int c = search_o[node->axis] >= node_o[node->axis];
	KDTChunkTree_searchHelper(node->children[c], origin, writeback);
}

KDTChunkNode* KDTChunkTree_searchRaw(KDTChunkTree* tree, const Vector3 pos) {
	if (!tree) return nullptr;

	KDTChunkNode* writeback = nullptr;

	KDTChunkTree_searchHelper(tree->root, KDTChunk_getOrigin(pos), &writeback);

	return writeback;
}

KDTChunk* KDTChunkTree_searchRaw_Chunk(KDTChunkTree* tree, const Vector3 pos) {
	if (!tree) return nullptr;

	KDTChunkNode* node = KDTChunkTree_searchRaw(tree, pos);
	return node ? node->chunk : nullptr;
}

static void KDTChunkTree_initLoadedHelper(KDTChunkTree* tree, KDTChunkNode* node) {
	if (!tree || !node) return;

	KDTChunkTree_initLoadedHelper(tree, node->children[0]);
	KDTChunkTree_initLoadedHelper(tree, node->children[1]);
	KDTChunkTree_insertLoaded(tree, node->chunk);
}
void KDTChunkTree_initLoaded(KDTChunkTree* tree) {
	if (!tree) return;

	KDTChunkTree_initLoadedHelper(tree, tree->root);
}

static void KDTChunkTree_clearLoadedHelper(KDTChunkNode* node) {
	if (!node) return;

	KDTChunkTree_clearLoadedHelper(node->children[0]);
	KDTChunkTree_clearLoadedHelper(node->children[1]);
	free(node->children);
	free(node);
}
void KDTChunkTree_clearLoaded(KDTChunkTree* tree) {
	if (!tree) return;

	KDTChunkTree_clearLoadedHelper(tree->loaded->root);
}

void KDTChunkTree_deleteLoaded(KDTChunkTree* tree) {
	if (!tree) return;

	KDTChunkTree_clearLoaded(tree);
	free(tree->loaded);
}

KDTChunkNode* KDTChunkTree_insertLoaded(KDTChunkTree* tree, KDTChunk* chunk) {
	if (!tree || !chunk) return nullptr;

	KDTChunkNode* writeback = nullptr;

	tree->loaded->root = KDTChunkTree_insertHelper(tree->loaded->root, 0, chunk, &writeback);

	if (writeback)
		KDTChunk_initVis(chunk);
	return writeback;
}

static void KDTChunkTree_rotateLoadedHelper(KDTChunkNode* node, const Vector3 axis, const float rotation_speed) {
	if (!node) return;

	KDTChunkTree_rotateLoadedHelper(node->children[0], axis, rotation_speed);
	KDTChunkTree_rotateLoadedHelper(node->children[1], axis, rotation_speed);
	KDTChunk_rotateVis(node->chunk, axis, rotation_speed);
}
void KDTChunkTree_rotateLoaded(KDTChunkTree* tree, const Vector3 axis, const float rotation_speed) {
	if (!tree) return;

	KDTChunkTree_rotateLoadedHelper(tree->loaded->root, axis, rotation_speed);
}

static void KDTChunkTree_translateLoadedHelper(KDTChunkNode* node, const Vector3 axis, const float move_speed) {
	if (!node) return;

	KDTChunkTree_translateLoadedHelper(node->children[0], axis, move_speed);
	KDTChunkTree_translateLoadedHelper(node->children[1], axis, move_speed);
	KDTChunk_translateVis(node->chunk, axis, move_speed);
}
void KDTChunkTree_translateLoaded(KDTChunkTree* tree, const Vector3 axis, const float move_speed) {
	if (!tree) return;

	KDTChunkTree_translateLoadedHelper(tree->loaded->root, axis, move_speed);
}

static void KDTChunkTree_resetLoadedHelper(KDTChunkNode* node) {
	if (!node) return;

	KDTChunkTree_resetLoadedHelper(node->children[0]);
	KDTChunkTree_resetLoadedHelper(node->children[1]);
	KDTChunk_resetVis(node->chunk);
}
void KDTChunkTree_resetLoaded(KDTChunkTree* tree) {
	if (!tree) return;

	KDTChunkTree_resetLoadedHelper(tree->loaded->root);
}

static void KDTChunkTree_drawLoadedHelper(KDTChunkNode* node, float x_offset, float y_offset, float scale) {
	if (!node) return;

	KDTChunkTree_drawLoadedHelper(node->children[0], x_offset, y_offset, scale);
	KDTChunkTree_drawLoadedHelper(node->children[1], x_offset, y_offset, scale);
	KDTChunk_drawVis(node->chunk, x_offset, y_offset, scale);
}
void KDTChunkTree_drawLoaded(KDTChunkTree* tree, float x_offset, float y_offset, float scale) {
	if (!tree) return;

	KDTChunkTree_drawLoadedHelper(tree->loaded->root, x_offset, y_offset, scale);
}
