#include "voxel.h"
#include "kdtchunk.h"
#include "kdtchunktree.h"
#include <raymath.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct KDTChunkNode {
	KDTChunk* 	chunk;
	uint8_t 	axis;
	struct 	KDTChunkNode* children[2];
} KDTChunkNode;

typedef struct KDTChunkTree {
	KDTChunkNode* root;
} KDTChunkTree;

KDTChunkNode* KDTChunkNode_init(KDTChunkNode* node, KDTChunk* chunk, uint8_t axis) {
	if (!node || !chunk) return nullptr;

	node->chunk = chunk;
	node->axis 	= axis;
	node->children[0] = nullptr;
	node->children[1] = nullptr;

	return node;
}

KDTChunkTree* KDTChunkTree_init(KDTChunkTree* tree) {
	if (!tree) return nullptr;

	tree->root = nullptr;
	return tree;
}

static void KDTChunkTree_deleteHelper(KDTChunkNode* node) {
	if (!node) return;

	KDTChunkTree_deleteHelper(node->children[0]);
	KDTChunkTree_deleteHelper(node->children[1]);
	KDTChunk_delete(node->chunk);
	free(node);
}
void KDTChunkTree_delete(KDTChunkTree* tree) {
	if (!tree) return;

	KDTChunkTree_deleteHelper(tree->root);
}

KDTChunkNode* KDTChunkTree_insert(KDTChunkTree* tree, KDTChunk* chunk) {
	if (!tree || !chunk) return nullptr;

	if (!tree->root) {
		tree->root = (KDTChunkNode*)malloc(sizeof(KDTChunkNode));
		return KDTChunkNode_init(tree->root, chunk, 0);
	}

	float xyz[3] = { chunk->origin.x, chunk->origin.y, chunk->origin.z };
	KDTChunkNode* curr = tree->root;
	while (curr) {
		float xyzc[3] = { curr->chunk->origin.x, curr->chunk->origin.y, curr->chunk->origin.z };
		if (xyz[0] == xyzc[0] && xyz[1] == xyzc[1] && xyz[2] == xyzc[2])
			break;
		int c = (int)(xyz[curr->axis] >= xyzc[curr->axis]);
		if (!curr->children[c]) {
			printf("nchunk\n");
			curr->children[c] = (KDTChunkNode*)malloc(sizeof(KDTChunkNode));
			return KDTChunkNode_init(curr->children[c], chunk, (curr->axis+1)%3);
		}
		curr = curr->children[c];
	}
	return nullptr;
}

KDTChunkNode* KDTChunkTree_insertNew(KDTChunkTree* tree, const Vector3 origin) {
	if (!tree) return nullptr;

	KDTChunk* chunk = (KDTChunk*)malloc(sizeof(KDTChunk));
	KDTChunk_init(chunk, origin);

	KDTChunkNode* node = KDTChunkTree_insert(tree, chunk);
	if (node) {
		free(chunk);
	}
	return node;
}

KDTChunkNode* KDTChunkTree_search(KDTChunkTree* tree, const Vector3 pos) {
	if (!tree) return nullptr;

	Vector3 o = KDTChunk_getOrigin(pos);

	float xyz[3] = { o.x, o.y, o.z };
	KDTChunkNode* curr = tree->root;
	while (curr) {
		float xyzc[3] = { curr->chunk->origin.x, curr->chunk->origin.y, curr->chunk->origin.z };
		if (xyz[0] == xyzc[0] && xyz[1] == xyzc[1] && xyz[2] == xyzc[2])
			return curr;
		int c = (int)(xyz[curr->axis] >= xyzc[curr->axis]);
		curr = curr->children[c];
	}
	return nullptr;
}

KDTChunk* KDTChunkTree_searchChunk(KDTChunkTree* tree, const Vector3 pos) {
	KDTChunkNode* node = KDTChunkTree_search(tree, pos);
	return node ? node->chunk : nullptr;
}

static void KDTChunkTree_deleteVisHelper(KDTChunkNode* node) {
	if (!node) return;

	KDTChunk_deleteVis(node->chunk);
	KDTChunkTree_deleteVisHelper(node->children[0]);
	KDTChunkTree_deleteVisHelper(node->children[1]);
}
void KDTChunkTree_deleteVis(KDTChunkTree* tree) {
	if (!tree) return;

	KDTChunkTree_deleteVisHelper(tree->root);
}

static void KDTChunkTree_initVisHelper(KDTChunkNode* node) {
	if (!node) return;

	KDTChunkTree_initVisHelper(node->children[0]);
	printf("c0\n");
	KDTChunkTree_initVisHelper(node->children[1]);
	printf("c1\n");
	KDTChunk_initVis(node->chunk);
	printf("vis\n");
}
void KDTChunkTree_initVis(KDTChunkTree* tree) {
	if (!tree) return;

	KDTChunkTree_initVisHelper(tree->root);
}

static void KDTChunkTree_resetVisHelper(KDTChunkNode* node) {
	if (!node) return;

	KDTChunkTree_resetVisHelper(node->children[0]);
	KDTChunkTree_resetVisHelper(node->children[1]);
	KDTChunk_resetVis(node->chunk);
}
void KDTChunkTree_resetVis(KDTChunkTree* tree) {
	if (!tree) return;

	KDTChunkTree_resetVisHelper(tree->root);
}

static void KDTChunkTree_rotateVisHelper(KDTChunkNode* node, const Vector3 axis, const float rotation_speed) {
	if (!node) return;

	KDTChunkTree_rotateVisHelper(node->children[0], axis, rotation_speed);
	KDTChunkTree_rotateVisHelper(node->children[1], axis, rotation_speed);
	KDTChunk_rotateVis(node->chunk, axis, rotation_speed);
}
void KDTChunkTree_rotateVis(KDTChunkTree* tree, const Vector3 axis, const float rotation_speed) {
	if (!tree) return;

	KDTChunkTree_rotateVisHelper(tree->root, axis, rotation_speed);
}

static void KDTChunkTree_translateVisHelper(KDTChunkNode* node, const Vector3 axis, const float move_speed) {
	if (!node) return;

	KDTChunkTree_translateVisHelper(node->children[0], axis, move_speed);
	KDTChunkTree_translateVisHelper(node->children[1], axis, move_speed);
	KDTChunk_translateVis(node->chunk, axis, move_speed);
}
void KDTChunkTree_translateVis(KDTChunkTree* tree, const Vector3 axis, const float move_speed) {
	if (!tree) return;

	KDTChunkTree_translateVisHelper(tree->root, axis, move_speed);
}

static void KDTChunkTree_drawVisHelper(KDTChunkNode* node, float x_offset, float y_offset, float scale) {
	if (!node) return;

	KDTChunkTree_drawVisHelper(node->children[0], x_offset, y_offset, scale);
	KDTChunkTree_drawVisHelper(node->children[1], x_offset, y_offset, scale);
	KDTChunk_drawVis(node->chunk, x_offset, y_offset, scale);
}
void KDTChunkTree_drawVis(KDTChunkTree* tree, float x_offset, float y_offset, float scale) {
	if (!tree) return;

	KDTChunkTree_drawVisHelper(tree->root, x_offset, y_offset, scale);
}
