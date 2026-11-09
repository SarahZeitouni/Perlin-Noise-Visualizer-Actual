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

#define NUM_CHILDREN 8
#define X_WIDTH 	16
#define Y_HEIGHT 	128
#define Z_DEPTH 	16

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

	return node;
}

void PRVoxelNode_clear(PRVoxelNode* node) {
	if (!node) return;

	if (!node->leaf) {
		for (int i = 0; i < NUM_CHILDREN; i ++) {
			PRVoxelNode_delete(node->children[i]);
		}
		free(node->children);
	}
	free(node->voxel);
}

void PRVoxelNode_delete(PRVoxelNode* node) {
	if (!node) return;

	PRVoxelNode_clear(node);
	free(node);
}

PRChunk* PRChunk_init(PRChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	chunk->origin 	= PRChunk_getOrigin(origin);
	chunk->root 	= nullptr;
	chunk->vis		= (PRChunk*)malloc(sizeof(PRChunk));
	chunk->vis->origin 	= chunk->origin;
	chunk->vis->root 	= nullptr;

	return chunk;
}

void PRChunk_clear(PRChunk* chunk) {
	if (!chunk) return;

	PRVoxelNode_delete(chunk->root);
	PRChunk_deleteVis(chunk);
}

void PRChunk_delete(PRChunk* chunk) {
	if (!chunk) return;

	PRChunk_clear(chunk);
	free(chunk);
}

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
		return node;
	}

	if (node->leaf) {
		if (voxel->origin.x == node->voxel->origin.x
			&& voxel->origin.y == node->voxel->origin.y
			&& voxel->origin.z == node->voxel->origin.z)
		{
			if (writeback)
				*writeback = nullptr;
			return node;
		}

		node->children = (PRVoxelNode**)malloc(NUM_CHILDREN * sizeof(PRVoxelNode*));
		for (int i = 0; i < NUM_CHILDREN; i ++)
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
	if (!chunk || !PRChunk_inRangeRaw(chunk, pos))
		return nullptr;

	Voxel* voxel = (Voxel*)malloc(sizeof(Voxel));
	Voxel_init(voxel, Voxel_getOrigin(pos));

	PRVoxelNode* writeback = nullptr;

	chunk->root = PRChunk_insertHelper(chunk->root, chunk->origin, Vector3Add(chunk->origin,
		(Vector3){ X_WIDTH, Y_HEIGHT, Z_DEPTH}), voxel, &writeback);

	if (!writeback) {
		free(voxel);
		voxel = nullptr;
	}

	if (voxel) {
		Vector3 o = voxel->origin;

		int adj_pair_list[6] = { 2, 3, 0, 1, 5, 4 };

		Vector3 adj_v_list[6] = {
			(Vector3){ o.x, o.y, o.z-1 }, (Vector3){ o.x+1, o.y, o.z }, (Vector3){ o.x, o.y, o.z+1 },
			(Vector3){ o.x-1, o.y, o.z }, (Vector3){ o.x, o.y-1, o.z }, (Vector3){ o.x, o.y+1, o.z }
		};

		for (int i = 0; i < 6; i ++) {
			voxel->adj[i] = PRChunk_searchRaw_Voxel(chunk, adj_v_list[i]);
			if (!voxel->adj[i] && chunk->adj[i]) {
				voxel->adj[i] = PRChunk_searchRaw_Voxel(chunk->adj[i], adj_v_list[i]);
			}
			if(voxel->adj[i])
				voxel->adj[i]->adj[adj_pair_list[i]] = voxel;
		}
	}
	return writeback;
}

PRVoxelNode* PRChunk_insertLocal(PRChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	return PRChunk_insertRaw(chunk, Vector3Add(chunk->origin, origin));
}

PRVoxelNode* PRChunk_searchRaw(PRChunk* chunk, const Vector3 pos) {
	if (!chunk || !PRChunk_inRangeRaw(chunk, pos))
		return nullptr;

	Vector3 o = Voxel_getOrigin(pos);

	PRVoxelNode* curr = chunk->root;
	while (curr && !curr->leaf) {
		int octant = PRVoxelNode_getOctant(curr, o);
		if (octant < 0)
			return nullptr;
		curr = curr->children[octant];
	}
	if (!curr || !(curr->voxel->origin.x == o.x && curr->voxel->origin.y == o.y && curr->voxel->origin.z == o.z))
			return nullptr;
	return curr;
}

PRVoxelNode* PRChunk_searchLocal(PRChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	return PRChunk_searchRaw(chunk, Vector3Add(chunk->origin, origin));
}

Voxel* PRChunk_searchRaw_Voxel(PRChunk* chunk, const Vector3 pos) {
	if (!chunk) return nullptr;

	PRVoxelNode* node = PRChunk_searchRaw(chunk, pos);
	return node ? node->voxel : nullptr;
}

Voxel* PRChunk_searchLocal_Voxel(PRChunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	PRVoxelNode* node = PRChunk_searchLocal(chunk, origin);
	return node ? node->voxel : nullptr;
}

// +y (up)  4-----5  _. -z (back)
//     .   /|    /|  /|
//    /|\ 7-+---6 | /
//     |  | 0---|-1
//     |  |/    |/
//     |  3-----2  ---> +x (right)

static void PRChunk_initVisHelper(PRChunk* chunk, PRVoxelNode* node) {
	if (!chunk || !node) return;

	if (!node->leaf) {
		for (int i = 0; i < NUM_CHILDREN; i ++)
			PRChunk_initVisHelper(chunk, node->children[i]);
		return;
	}
	PRChunk_insertVis(chunk, node->voxel);
}
void PRChunk_initVis(PRChunk* chunk) {
	if (!chunk) return;

	PRChunk_initVisHelper(chunk, chunk->root);
}

static void PRChunk_clearVisHelper(PRVoxelNode* node) {
	if (!node) return;

	if (!node->leaf) {
		for (int i = 0; i < NUM_CHILDREN; i ++)
			PRChunk_clearVisHelper(node->children[i]);
		free(node->children);
	}
	free(node);
}
void PRChunk_clearVis(PRChunk* chunk) {
	if (!chunk) return;

	PRChunk_clearVisHelper(chunk->vis->root);
}

void PRChunk_deleteVis(PRChunk* chunk) {
	if (!chunk) return;

	PRChunk_clearVis(chunk);
	free(chunk->vis);
}

PRVoxelNode* PRChunk_insertVis(PRChunk* chunk, Voxel* voxel) {
	if (!chunk || !voxel || !Voxel_isVisible(voxel)) return nullptr;

	PRVoxelNode* writeback = nullptr;

	chunk->vis->root = PRChunk_insertHelper(chunk->vis->root, chunk->origin, Vector3Add(chunk->origin,
		(Vector3){ X_WIDTH, Y_HEIGHT, Z_DEPTH}), voxel, &writeback);

	return writeback;
}

static void PRChunk_rotateVisHelper(PRVoxelNode* node, const Vector3 axis, const float rotation_speed) {
	if (!node) return;

	if (!node->leaf) {
		for (int i = 0; i < NUM_CHILDREN; i ++)
			PRChunk_rotateVisHelper(node->children[i], axis, rotation_speed);
		return;
	}
	Voxel_rotateDisplay(node->voxel, axis, rotation_speed);
}
void PRChunk_rotateVis(PRChunk* chunk, const Vector3 axis, const float rotation_speed) {
	if (!chunk) return;

	PRChunk_rotateVisHelper(chunk->vis->root, axis, rotation_speed);
}

static void PRChunk_translateVisHelper(PRVoxelNode* node, const Vector3 axis, const float move_speed) {
	if (!node) return;

	if (!node->leaf) {
		for (int i = 0; i < NUM_CHILDREN; i ++)
			PRChunk_translateVisHelper(node->children[i], axis, move_speed);
		return;
	}
	Voxel_translateDisplay(node->voxel, axis, move_speed);
}
void PRChunk_translateVis(PRChunk* chunk, const Vector3 axis, const float move_speed) {
	if (!chunk) return;

	PRChunk_translateVisHelper(chunk->vis->root, axis, move_speed);
}

static void PRChunk_resetVisHelper(PRVoxelNode* node) {
	if (!node) return;

	if (!node->leaf) {
		for (int i = 0; i < NUM_CHILDREN; i ++)
			PRChunk_resetVisHelper(node->children[i]);
		return;
	}
	Voxel_resetDisplay(node->voxel);
}
void PRChunk_resetVis(PRChunk* chunk) {
	if (!chunk) return;

	PRChunk_resetVisHelper(chunk->vis->root);
}

static void PRChunk_drawVisHelper(PRVoxelNode* node, float x_offset, float y_offset, float scale) {
	if (!node) return;

	if (!node->leaf) {
		for (int i = 0; i < NUM_CHILDREN; i ++)
			if (node->children[i]) PRChunk_drawVisHelper(node->children[i], x_offset, y_offset, scale);
		return;
	}
	Voxel_draw(node->voxel, x_offset, y_offset, scale);
}
void PRChunk_drawVis(PRChunk* chunk, float x_offset, float y_offset, float scale) {
	if (!chunk) return;

	PRChunk_drawVisHelper(chunk->vis->root, x_offset, y_offset, scale);
}

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
	if (!node) return false;

	return (
		pos.x >= node->v1.x && pos.x < node->v2.x
		&& pos.y >= node->v1.y && pos.y < node->v2.y
		&& pos.z >= node->v1.z && pos.z < node->v2.z
	);
}

bool PRChunk_inRangeRaw(PRChunk* chunk, const Vector3 pos) {
	if (!chunk)
		return false;

	Vector3 o = Vector3Subtract(pos, chunk->origin);
	return (o.x >= 0 && o.x < X_WIDTH && o.y >= 0 && o.y < Y_HEIGHT && o.z >= 0 && o.z < Z_DEPTH);
}

Vector3 PRChunk_getOrigin(const Vector3 pos) {
    Vector3 o = Voxel_getOrigin(pos);
    return (Vector3){
        (int)o.x < 0 && (int)o.x % X_WIDTH   != 0 ? ((int)o.x/X_WIDTH)*X_WIDTH - X_WIDTH    : ((int)o.x/X_WIDTH)*X_WIDTH,
		(int)o.y < 0 && (int)o.y % Y_HEIGHT  != 0 ? ((int)o.y/Y_HEIGHT)*Y_HEIGHT - Y_HEIGHT : ((int)o.y/Y_HEIGHT)*Y_HEIGHT,
        (int)o.z < 0 && (int)o.z % Z_DEPTH   != 0 ? ((int)o.z/Z_DEPTH)*Z_DEPTH - Z_DEPTH   	: ((int)o.z/Z_DEPTH)*Z_DEPTH
    };
}

#undef Z_DEPTH
#undef Y_HEIGHT
#undef X_WIDTH
#undef NUM_CHILDREN
