#include "voxel.h"
#include "prchunk.h"
#include "prchunktree.h"
#include <raymath.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

//typedef struct PRChunkNode {
//	Vector3 	v1;
//	Vector3 	v2;
//	bool 		leaf;
//	PRChunk* 	chunk;
//	uint8_t 	count;
//	struct 	PRChunkNode** children;
//} PRChunkNode;
//
//typedef struct PRChunkTree {
//	Vector3 v1;
// 	Vector3 v2;
//	PRChunkNode* root;
//	struct PRChunkTree* loaded;
//} PRChunkTree;

#define NUM_CHILDREN 8

PRChunkNode* PRChunkNode_init(PRChunkNode* node, const Vector3 v1, const Vector3 v2, const Vector3 origin) {
	if (!node) return nullptr;

	node->v1 	= v1;
	node->v2	= v2;
	node->leaf 	= true;
	node->chunk = (PRChunk*)malloc(sizeof(PRChunk));
	PRChunk_init(node->chunk, origin),
	node->count 	= 0;
	node->children 	= nullptr;

	return node;
}

PRChunkNode* PRChunkNode_initChunk(PRChunkNode* node, const Vector3 v1, const Vector3 v2, PRChunk* chunk) {
	if (!node || !chunk) return nullptr;

	node->v1 	= v1;
	node->v2 	= v2;
	node->leaf 	= true;
	node->chunk = chunk;
	node->count = 0;
	node->children = nullptr;

	return node;
}

void PRChunkNode_clear(PRChunkNode* node) {
	if (!node) return;

	if (!node->leaf) {
		for (int i = 0; i < NUM_CHILDREN; i ++) {
			PRChunkNode_delete(node->children[i]);
		}
		free(node->children);
	}
}

void PRChunkNode_delete(PRChunkNode* node) {
	if (!node) return;

	PRChunkNode_clear(node);
	free(node);
}

PRChunkTree* PRChunkTree_init(PRChunkTree* tree, const Vector3 v1, const Vector3 v2) {
	if (!tree) return nullptr;

	tree->v1 	= v1;
	tree->v2	= v2;
	tree->root 	= nullptr;
	tree->loaded		= (PRChunkTree*)malloc(sizeof(PRChunkTree));
	tree->loaded->v1 	= v1;
	tree->loaded->v2	= v2;
	tree->loaded->root 	= nullptr;

	return tree;
}

void PRChunkTree_clear(PRChunkTree* tree) {
	if (!tree) return;

	PRChunkNode_delete(tree->root);
	tree->root = nullptr;
	PRChunkTree_deleteLoaded(tree);
}

void PRChunkTree_delete(PRChunkTree* tree) {
	if (!tree) return;

	PRChunkTree_clear(tree);
	free(tree);
}

// +y (up)  4-----5  _. -z (back)
//     .   /|    /|  /|
//    /|\ 7-+---6 | /
//     |  | 0---|-1
//     |  |/    |/
//     |  3-----2  ---> +x (right)

static PRChunkNode* PRChunkTree_insertHelper(PRChunkNode* node, const Vector3 v1, const Vector3 v2, PRChunk* chunk, PRChunkNode** writeback) {
	if (!chunk){
		if (writeback)
			*writeback = nullptr;
		return node;
	}

	if (!node) {
		node = (PRChunkNode*)malloc(sizeof(PRChunkNode));
		PRChunkNode_initChunk(node, v1, v2, chunk);
		if (writeback)
			*writeback = node;
		return node;
	}

	if (!PRChunkNode_inRange(node, chunk->origin)) {
		if (writeback)
			*writeback = nullptr;
		return node;
	}

	if (node->leaf) {
		if (chunk->origin.x == node->chunk->origin.x
			&& chunk->origin.y == node->chunk->origin.y
			&& chunk->origin.z == node->chunk->origin.z)
		{
			if (writeback)
				*writeback = nullptr;
			return node;
		}

		node->children = (PRChunkNode**)malloc(NUM_CHILDREN * sizeof(PRChunkNode*));
		for (int i = 0; i < NUM_CHILDREN; i ++)
			node->children[i] = nullptr;
		node->leaf 	= false;
		PRChunkTree_insertHelper(node, v1, v2, node->chunk, nullptr);
		node->chunk = nullptr;
	}

	Vector3 mid = (Vector3){
		(node->v1.x + node->v2.x)/2,
		(node->v1.y + node->v2.y)/2,
		(node->v1.z + node->v2.z)/2
	};

	int octant = PRChunkNode_getOctant(node, chunk->origin);
	if (octant < 0) {
		if (writeback)
			*writeback = nullptr;
		return node;
	}
	Vector3 u1 = v1;
	Vector3 u2 = mid;
	if (chunk->origin.x >= mid.x) {
		u1.x = mid.x;
		u2.x = v2.x;
	}
	if (chunk->origin.y >= mid.y) {
		u1.y = mid.y;
		u2.y = v2.y;
	}
	if (chunk->origin.z >= mid.z) {
		u1.z = mid.z;
		u2.z = v2.z;
	}

	node->children[octant] = PRChunkTree_insertHelper(node->children[octant], u1, u2, chunk, writeback);
	return node;
}
PRChunkNode* PRChunkTree_insertRaw(PRChunkTree* tree, const Vector3 pos) {
	if (!tree || !PRChunkTree_inRange(tree, pos)) return nullptr;

	PRChunk* chunk = (PRChunk*)malloc(sizeof(PRChunk));
	PRChunk_init(chunk, pos);

	PRChunkNode* writeback = nullptr;

	tree->root = PRChunkTree_insertHelper(tree->root, tree->v1, tree->v2, chunk, &writeback);

	if (!writeback) {
		free(chunk);
		chunk = nullptr;
	}

	if (chunk) {
		Vector3 o = chunk->origin;

		chunk->adj[0] = PRChunkTree_searchRaw_Chunk(tree, (Vector3){ o.x, o.y, o.z-16 });
		chunk->adj[1] = PRChunkTree_searchRaw_Chunk(tree, (Vector3){ o.x+16, o.y, o.z });
		chunk->adj[2] = PRChunkTree_searchRaw_Chunk(tree, (Vector3){ o.x, o.y, o.z+16 });
		chunk->adj[3] = PRChunkTree_searchRaw_Chunk(tree, (Vector3){ o.x-16, o.y, o.z });
		chunk->adj[4] = PRChunkTree_searchRaw_Chunk(tree, (Vector3){ o.x, o.y-16, o.z });
		chunk->adj[5] = PRChunkTree_searchRaw_Chunk(tree, (Vector3){ o.x, o.y+16, o.z });
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

PRChunkNode* PRChunkTree_searchRaw(PRChunkTree* tree, const Vector3 pos) {
	if (!tree || !PRChunkTree_inRange(tree, pos)) return nullptr;

	Vector3 o = PRChunk_getOrigin(pos);

	PRChunkNode* curr = tree->root;
	while (curr && !curr->leaf) {
		int octant = PRChunkNode_getOctant(curr, o);
		if (octant < 0)
			return nullptr;
		curr = curr->children[octant];
	}
	if (!curr || !(curr->chunk->origin.x == o.x && curr->chunk->origin.y == o.y && curr->chunk->origin.z == o.z))
			return nullptr;
	return curr;
}

PRChunk* PRChunkTree_searchRaw_Chunk(PRChunkTree* tree, const Vector3 pos) {
	if (!tree) return nullptr;

	PRChunkNode* node = PRChunkTree_searchRaw(tree, pos);
	return node ? node->chunk : nullptr;
}

// +y (up)  4-----5  _. -z (back)
//     .   /|    /|  /|
//    /|\ 7-+---6 | /
//     |  | 0---|-1
//     |  |/    |/
//     |  3-----2  ---> +x (right)

static void PRChunkTree_initLoadedHelper(PRChunkTree* tree, PRChunkNode* node) {
	if (!tree || !node) return;

	if (!node->leaf) {
		for (int i = 0; i < 8; i ++)
			PRChunkTree_initLoadedHelper(tree, node->children[i]);
		return;
	}
	PRChunkTree_insertLoaded(tree, node->chunk);
	PRChunk_initVis(node->chunk);
}
void PRChunkTree_initLoaded(PRChunkTree* tree) {
	if (!tree) return;

	PRChunkTree_initLoadedHelper(tree, tree->root);
}

static void PRChunkTree_clearLoadedHelper(PRChunkNode* node) {
	if (!node) return;

	if (!node->leaf) {
		for (int i = 0; i < 8; i ++)
			PRChunkTree_clearLoadedHelper(node->children[i]);
		free(node->children);
	}
	free(node);
}
void PRChunkTree_clearLoaded(PRChunkTree* tree) {
	if (!tree || !tree->loaded) return;
	PRChunkTree_clearLoadedHelper(tree->loaded->root);
	tree->loaded->root = nullptr;
}

void PRChunkTree_deleteLoaded(PRChunkTree* tree) {
	if (!tree || !tree->loaded) return;
	PRChunkTree_clearLoaded(tree);
	free(tree->loaded);
	tree->loaded = nullptr;
}
PRChunkNode* PRChunkTree_insertLoaded(PRChunkTree* tree, PRChunk* chunk) {
	if (!tree || !chunk) return nullptr;

	PRChunkNode* writeback = nullptr;

	tree->loaded->root = PRChunkTree_insertHelper(tree->loaded->root, tree->v1, tree->v2, chunk, &writeback);

	return writeback;
}

static void PRChunkTree_rotateLoadedHelper(PRChunkNode* node, const Vector3 axis, const float rotation_speed) {
	if (!node) return;

	if (!node->leaf) {
		for (int i = 0; i < NUM_CHILDREN; i ++)
			PRChunkTree_rotateLoadedHelper(node->children[i], axis, rotation_speed);
		return;
	}
	PRChunk_rotateVis(node->chunk, axis, rotation_speed);
}
void PRChunkTree_rotateLoaded(PRChunkTree* tree, const Vector3 axis, const float rotation_speed) {
	if (!tree) return;

	PRChunkTree_rotateLoadedHelper(tree->loaded->root, axis, rotation_speed);
}

static void PRChunkTree_translateLoadedHelper(PRChunkNode* node, const Vector3 axis, const float move_speed) {
	if (!node) return;

	if (!node->leaf) {
		for (int i = 0; i < NUM_CHILDREN; i ++)
			PRChunkTree_translateLoadedHelper(node->children[i], axis, move_speed);
		return;
	}
	PRChunk_translateVis(node->chunk, axis, move_speed);
}
void PRChunkTree_translateLoaded(PRChunkTree* tree, const Vector3 axis, const float move_speed) {
	if (!tree) return;

	PRChunkTree_translateLoadedHelper(tree->loaded->root, axis, move_speed);
}

static void PRChunkTree_resetLoadedHelper(PRChunkNode* node) {
	if (!node) return;

	if (!node->leaf) {
		for (int i = 0; i < NUM_CHILDREN; i ++)
			PRChunkTree_resetLoadedHelper(node->children[i]);
		return;
	}
	PRChunk_resetVis(node->chunk);
}
void PRChunkTree_resetLoaded(PRChunkTree* tree) {
	if (!tree) return;

	PRChunkTree_resetLoadedHelper(tree->loaded->root);
}

static void PRChunkTree_drawLoadedHelper(PRChunkNode* node, float x_offset, float y_offset, float scale) {
	if (!node) return;

	if (!node->leaf) {
		for (int i = 0; i < NUM_CHILDREN; i ++)
			if (node->children[i]) PRChunkTree_drawLoadedHelper(node->children[i], x_offset, y_offset, scale);
		return;
	}
	PRChunk_drawVis(node->chunk, x_offset, y_offset, scale);
}
void PRChunkTree_drawLoaded(PRChunkTree* tree, float x_offset, float y_offset, float scale) {
	if (!tree) return;

	PRChunkTree_drawLoadedHelper(tree->loaded->root, x_offset, y_offset, scale);
}

int PRChunkNode_getOctant(PRChunkNode* node, const Vector3 pos) {
	if (!node || !PRChunkNode_inRange(node, pos)) return -1;

	Vector3 mid = (Vector3){
		(node->v1.x + node->v2.x)/2,
		(node->v1.y + node->v2.y)/2,
		(node->v1.z + node->v2.z)/2
	};

	if (pos.x >= mid.x && pos.y >= mid.y && pos.z >= mid.z)
		return 6;
	else if (pos.y >= mid.y && pos.z >= mid.z)
		return 7;
	else if (pos.x >= mid.x && pos.z >= mid.z)
		return 2;
	else if (pos.z >= mid.z)
		return 3;
	else if (pos.x >= mid.x && pos.y >= mid.y)
		return 5;
	else if (pos.y >= mid.y)
		return 4;
	else if (pos.x >= mid.x)
		return 1;
	else
		return 0;
}

bool PRChunkNode_inRange(PRChunkNode* node, const Vector3 pos) {
	if (!node) return false;

	return (
		pos.x >= node->v1.x && pos.x < node->v2.x
		&& ((pos.y >= node->v1.y && pos.y < node->v2.y) || (pos.y == node->v1.y && pos.y == node->v2.y))
		&& pos.z >= node->v1.z && pos.z < node->v2.z
	);
}

bool PRChunkTree_inRange(PRChunkTree* tree, const Vector3 pos) {
	if (!tree)	return false;

	return (
		pos.x >= tree->v1.x && pos.x < tree->v2.x
		&& ((pos.y >= tree->v1.y && pos.y < tree->v2.y) || (tree->v1.y == tree->v2.y))
		&& pos.z >= tree->v1.z && pos.z < tree->v2.z
	);
}

#undef NUM_CHILDREN
