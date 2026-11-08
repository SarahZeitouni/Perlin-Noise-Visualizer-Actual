#include "voxel.h"
#include "prchunk.h"
#include <raymath.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

//typedef struct PRVoxelNode {
//	Vector3 v1;
//	Vector3 v2;
//	bool 	leaf;
//	Voxel* 	voxel;
//	uint8_t count;
//	struct 	PRVoxelNode** children;
//} PRVoxelNode;
//
//typedef struct PRChunk {
//	Vector3 origin;
//	PRVoxelNode* root;
//	struct PRChunk* vis;
//} PRChunk;

PRVoxelNode* PRVoxelNode_init(PRVoxelNode* node, const Vector3 v1, const Vector3 v2, const Vector3 origin) {
	if (!node) return nullptr;

	node->v1 	= v1;
	node->v2	= v2;
	node->leaf 	= true;
	node->voxel = (Voxel*)malloc(sizeof(Voxel));
	Voxel_init(node->voxel, origin),
	node->count 	= 0;
	node->children 	= nullptr;

	return node;
}

PRVoxelNode* PRVoxelNode_initVoxel(PRVoxelNode* node, const Vector3 v1, const Vector3 v2, Voxel* voxel) {
	if (!node || !voxel) return nullptr;

	node->v1 	= v1;
	node->v2 	= v2;
	node->leaf 	= true;
	node->voxel = voxel;
	node->count = 0;
	node->children = nullptr;

	return nullptr;
}

void PRVoxelNode_delete(PRVoxelNode* node) {
	if (!node) return;

	for (int i = 0; i < 8; i ++) {
		PRVoxelNode_delete(node->children[i]);
	}
	free(node->children);
	free(node->voxel);
	free(node);
}

PRChunk* PRChunk_init(PRChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	chunk->origin 	= origin;
	chunk->root 	= nullptr;
	chunk->vis		= (PRChunk*)malloc(sizeof(PRChunk));
	chunk->vis->origin 	= origin;
	chunk->vis->root 	= nullptr;

	return chunk;
}

//void PRChunk_delete(PRChunk* chunk) {
//	if (!chunk) return;
//
//	PRVoxelNode_delete(chunk->root);
//}

// +y (up)  4-----5  _. -z (back)
//     .   /|    /|  /|
//    /|\ 7-+---6 | /
//     |  | 0---|-1
//     |  |/    |/
//     |  3-----2  ---> +x (right)

static PRVoxelNode* PRChunk_insertHelper(PRVoxelNode* node, const Vector3 v1, const Vector3 v2, Voxel* voxel, PRVoxelNode** writeback) {
	if (!voxel){
		if (writeback)
			*writeback = nullptr;
		printf("wb1\n");
		return node;
	}

	if (!node) {
		node = (PRVoxelNode*)malloc(sizeof(PRVoxelNode));
		PRVoxelNode_initVoxel(node, v1, v2, voxel);
		if (writeback)
			*writeback = node;
		return node;
	}

	if (!PRVoxelNode_inRange(node, voxel->origin)) {
		if (writeback)
			*writeback = nullptr;
		printf("wb2\n");
		return node;
	}

	if (node->leaf) {
		if (voxel->origin.x == node->voxel->origin.x
			&& voxel->origin.y == node->voxel->origin.y
			&& voxel->origin.z == node->voxel->origin.z)
		{
			if (writeback)
				*writeback = nullptr;
			printf("wb3\n");
			return node;
		}

		node->children = (PRVoxelNode**)malloc(8 * sizeof(PRVoxelNode*));
		for (int i = 0; i < 8; i ++)
			node->children[i] = nullptr;
		node->leaf 	= false;
		PRChunk_insertHelper(node, v1, v2, node->voxel, nullptr);
		node->voxel = nullptr;
	}

	Vector3 mid = (Vector3){
		(node->v1.x + node->v2.x)/2,
		(node->v1.y + node->v2.y)/2,
		(node->v1.z + node->v2.z)/2
	};

	int octant = PRVoxelNode_getOctant(node, voxel->origin);
	if (octant < 0) {
		if (writeback)
			*writeback = nullptr;
		printf("wb4\n");
		return node;
	}
	Vector3 u1 = v1;
	Vector3 u2 = mid;
	if (voxel->origin.x >= mid.x) {
		u1.x = mid.x;
		u2.x = v2.x;
	}
	if (voxel->origin.y >= mid.y) {
		u1.y = mid.y;
		u2.y = v2.y;
	}
	if (voxel->origin.z >= mid.z) {
		u1.z = mid.z;
		u2.z = v2.z;
	}

	node->children[octant] = PRChunk_insertHelper(node->children[octant], u1, u2, voxel, writeback);
	return node;
}
PRVoxelNode* PRChunk_insertRaw(PRChunk* chunk, const Vector3 pos) {
	if (!chunk || !PRChunk_inRangeRaw(chunk, pos)) {
		printf("0\n");
		return nullptr;
	}

	Voxel* voxel = (Voxel*)malloc(sizeof(Voxel));
	Voxel_init(voxel, Voxel_getOrigin(pos));

	PRVoxelNode* writeback = nullptr;

	chunk->root = PRChunk_insertHelper(chunk->root, chunk->origin, Vector3Add(chunk->origin, (Vector3){ 16, 128, 16}), voxel, &writeback);

	if (!writeback) {
		printf("wb\n");
		free(voxel);
	}
	return writeback;
}

PRVoxelNode* PRChunk_insertLocal(PRChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	return PRChunk_insertRaw(chunk, Vector3Add(chunk->origin, origin));
}

PRVoxelNode* PRChunk_searchRaw(PRChunk* chunk, const Vector3 pos) {
	if (!chunk || !PRChunk_inRangeRaw(chunk, pos)) {
		printf("bp2\n");
		return nullptr;
	}

	Vector3 o = Voxel_getOrigin(pos);

	PRVoxelNode* curr = chunk->root;
	while (curr && !curr->leaf) {
		int octant = PRVoxelNode_getOctant(curr, pos);
		if (octant < 0) {
			printf("bp3\n");
			return nullptr;
		}
		curr = curr->children[octant];
	}
//	if (!curr || !(curr->voxel->origin.x == o.x && curr->voxel->origin.y == o.y && curr->voxel->origin.z == o.y)) {
//			printf("bp4\n");
//			return nullptr;
//	}
	if (!curr) {
		printf("bp4.1\n");
		return nullptr;
	}
	if (!(curr->voxel->origin.x == o.x && curr->voxel->origin.y == o.y && curr->voxel->origin.z == o.z)) {
		printf("bp4.2\n");
		return nullptr;
	}
	return curr;
}

PRVoxelNode* PRChunk_searchLocal(PRChunk* chunk, const Vector3 origin) {
	if (!chunk) {
		printf("bp1\n");
		return nullptr;
	}

	return PRChunk_searchRaw(chunk, Vector3Add(chunk->origin, origin));
}

// +y (up)  4-----5  _. -z (back)
//     .   /|    /|  /|
//    /|\ 7-+---6 | /
//     |  | 0---|-1
//     |  |/    |/
//     |  3-----2  ---> +x (right)

int PRVoxelNode_getOctant(PRVoxelNode* node, const Vector3 pos) {
	if (!node || !PRVoxelNode_inRange(node, pos)) return -1;

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

bool PRVoxelNode_inRange(PRVoxelNode* node, const Vector3 pos) {
	if (!node) {
		printf("node\n");
		return false;
	}

	return (pos.x >= node->v1.x && pos.x < node->v2.x
		&& pos.y >= node->v1.y && pos.y < node->v2.y
		&& pos.z >= node->v1.z && pos.z < node->v2.z);
}

bool PRChunk_inRangeRaw(PRChunk* chunk, const Vector3 pos) {
	if (!chunk)	return false;

	Vector3 o = Vector3Subtract(pos, chunk->origin);
	return (o.x >= 0 && o.x < 16 && o.y >= 0 && o.y < 128 && o.z >= 0 && o.z < 16);
}
