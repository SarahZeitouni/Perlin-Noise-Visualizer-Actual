#include "voxel.h"
#include "stdlib.h"
#include "raylib.h"
#include "raymath.h"
#include "stdio.h"

Vector2 V3_flat(const Vector3 fat, float x_offset, float y_offset, float scale) {
	Vector2 vec = { scale*fat.x/fat.z + x_offset, scale*fat.y/fat.z + y_offset };
	return vec;
}

//spans from origin = (a, b, c) to (a+1, b+1, c+1)
//typedef struct {
//	Vector3 origin;
//	Vector3 disp_v[8];
//	struct Voxel* adj[6];
//} Voxel;
//
// y+ (up)  7-----6  _  z+ (back)
//         /|    /|  /|
//    /|\ 4-+---5 | /
//     |  | 3---|-2
//     |  |/    |/
//     |  0-----1  ---> x+ (right)

Voxel* Voxel_init(Voxel* voxel, const Vector3 origin) {
	if (!voxel) return nullptr;

	voxel->origin = origin;
	float x = origin.x;
	float y = -1 * origin.y; // y-axis is mirrored
	float z = origin.z;

	Vector3 disp_v[8] = {
		{x  , y  , z  }, // 0 origin
		{x+1, y  , z  }, // 1 right
		{x+1, y  , z+1}, // 2 right-back
		{x  , y  , z+1}, // 3 back
		{x  , y-1, z  }, // 4 up
		{x+1, y-1, z  }, // 5 up-right
		{x+1, y-1, z+1}, // 6 up-right-back
		{x  , y-1, z+1}  // 7 up-back
	};
	for (int i = 0; i < 8; i ++)
		voxel->disp_v[i] = disp_v[i];

	Voxel* adj[6] = {
		nullptr, // front
		nullptr, // right
		nullptr, // back
		nullptr, // left
		nullptr, // down
		nullptr  // up
	};
	for (int i = 0; i < 6; i ++)
		voxel->adj[i] = adj[i];

	return voxel;
}

// y+ (up)  7-----6  _  z+ (back)
//         /|    /|  /|
//    /|\ 4-+---5 | /
//     |  | 3---|-2
//     |  |/    |/
//     |  0-----1  ---> x+ (right)

// ~/python_stuff/floorper.py
void Voxel_drawDisplay(Voxel* const voxel, float x_offset, float y_offset, float scale) {
	float x = x_offset;
	float y = y_offset;
	float s = scale;
	for (int i = 0; i < 12; i ++) {
		int left = 3*i + (i/2) - 6*(i/3) - 8*(i/4) - 2*(i/5) + 3*(i/6) - 2*(i/7) + (i/8) + 4*(i/9) + 2*(i/10) - 4*(i/11);
		int right = 1 + i + 3*(i/2) - 2*(i/3) - 5*(i/4) - (i/6) - 3*(i/8);
		int face = 4*i - 6*(i/2) - (i/3) - 7*(i/5) + 4*(i/6) - 3*(i/7) + 3*(i/8) - 2*(i/9) + 6*(i/10);
		int next_face = 4 - 2*i + 5*(i/2) - 3*(i/3) - 3*(i/4) + 3*(i/5) + (i/6) + 3*(i/7) + (i/8) + 2*(i/9) - 2*(i/10);
		if (voxel->adj[face] || (voxel->adj[next_face] && !voxel->adj[next_face]->adj[face]))
			continue;
		if (voxel->disp_v[left].z < 0 || voxel->disp_v[right].z < 0)
			continue;
		DrawLineV(V3_flat(voxel->disp_v[left],x,y,s), V3_flat(voxel->disp_v[right], x,y,s), BLACK);
	}
}

void Voxel_forceDrawDisplay(Voxel* const voxel, float x_offset, float y_offset, float scale) {
	float x = x_offset;
	float y = y_offset;
	float s = scale;
	for (int i = 0; i < 12; i ++) {
		int left = 3*i + (i/2) - 6*(i/3) - 8*(i/4) - 2*(i/5) + 3*(i/6) - 2*(i/7) + (i/8) + 4*(i/9) + 2*(i/10) - 4*(i/11);
		int right = 1 + i + 3*(i/2) - 2*(i/3) - 5*(i/4) - (i/6) - 3*(i/8);
		DrawLineV(V3_flat(voxel->disp_v[left],x,y,s), V3_flat(voxel->disp_v[right], x,y,s), RED);
	}
}

void Voxel_rotateDisplay(Voxel* voxel, const Vector3 axis, float rotation_speed) {
	for (int i = 0; i < 8; i ++)
		voxel->disp_v[i] = Vector3RotateByAxisAngle(voxel->disp_v[i], axis, rotation_speed);
}

void Voxel_translateDisplay(Voxel* voxel, const Vector3 axis, float move_speed) {
	for (int i = 0; i < 8; i ++)
		voxel->disp_v[i] = Vector3Add(voxel->disp_v[i], (Vector3){axis.x*move_speed, axis.y*move_speed, axis.z*move_speed});
}

void Voxel_resetDisplay(Voxel* voxel) {
	float x = voxel->origin.x;
	float y = -1 * voxel->origin.y; // y-axis is mirrored
	float z = voxel->origin.z;
	Vector3 disp_v[] = {
		{x  , y  , z  }, // 0 origin
		{x+1, y  , z  }, // 1 right
		{x+1, y  , z+1}, // 2 right-back
		{x  , y  , z+1}, // 3 back
		{x  , y-1, z  }, // 4 up
		{x+1, y-1, z  }, // 5 up-right
		{x+1, y-1, z+1}, // 6 up-right-back
		{x  , y-1, z+1}  // 7 up-back
	};
	for (int i = 0; i < 8; i ++)
		voxel->disp_v[i] = disp_v[i];
}

bool Voxel_isVisible(Voxel* voxel) {
	for (int i = 0; i < 6; i ++)
		if (!voxel->adj[i]) return true;
	return false;
}

Vector3 Voxel_getOrigin(Vector3 pos) {
	return (Vector3){
		pos.x < 0 ? (int)(pos.x - 1) : (int)pos.x,
		pos.y < 0 ? (int)(pos.y - 1) : (int)pos.y,
		pos.z < 0 ? (int)(pos.z - 1) : (int)pos.z
	};
}

//typedef struct {
//	Voxel* v;
//	Voxel* left;
//	Voxel* right;
//} VoxelNode*;
//
//typedef struct {
//	Vector3 origin;
//	VoxelNode* root;
//	int count;
//} Chunk;

VoxelNode* VoxelNode_init(VoxelNode* node, const Vector3 origin) {
	if (!node) return nullptr;

	node->v = (Voxel*)malloc(sizeof(Voxel));
	Voxel_init(node->v, origin);
	node->left = nullptr;
	node->right = nullptr;
	return node;
}

Chunk* Chunk_init(Chunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	chunk->origin = origin;
	chunk->root = nullptr;
	chunk->display = (ChunkDisplay*)malloc(sizeof(ChunkDisplay));
	chunk->display->root = nullptr;
	return chunk;
}

void Chunk_insert(Chunk* chunk, const Vector3 origin) {
	if (!chunk) return;

	chunk->root = Chunk_insertHelper(chunk, chunk->root, origin);
}

void Chunk_insertArray(Chunk* chunk, const Vector3* array, int size) {
	if (!chunk || !array) return;

	for (int i = 0; i < size; i ++) Chunk_insert(chunk, array[i]);
}

VoxelNode* Chunk_insertHelper(Chunk* chunk, VoxelNode* node, const Vector3 origin) {
	if (!chunk) return node;

	if (!node) {
		node = (VoxelNode*)malloc(sizeof(VoxelNode));
		VoxelNode_init(node, origin);

		node->v->adj[0] = Chunk_searchVoxel(chunk, Vector3Add(origin, (Vector3){0, 0, -1}));
		if (node->v->adj[0]) node->v->adj[0]->adj[2] = node->v;

		node->v->adj[1] = Chunk_searchVoxel(chunk, Vector3Add(origin, (Vector3){1, 0, 0}));
		if (node->v->adj[1]) node->v->adj[1]->adj[3] = node->v;

		node->v->adj[2] = Chunk_searchVoxel(chunk, Vector3Add(origin, (Vector3){0, 0, 1}));
		if (node->v->adj[2]) node->v->adj[2]->adj[0] = node->v;

		node->v->adj[3] = Chunk_searchVoxel(chunk, Vector3Add(origin, (Vector3){-1, 0, 0}));
		if (node->v->adj[3]) node->v->adj[3]->adj[1] = node->v;

		node->v->adj[4] = Chunk_searchVoxel(chunk, Vector3Add(origin, (Vector3){0, -1, 0}));
		if (node->v->adj[4]) node->v->adj[4]->adj[5] = node->v;

		node->v->adj[5] = Chunk_searchVoxel(chunk, Vector3Add(origin, (Vector3){0, 1, 0}));
		if (node->v->adj[5]) node->v->adj[5]->adj[4] = node->v;

		return node;
	}

	if (node->v->origin.x < origin.x)
		node->left = Chunk_insertHelper(chunk, node->left, origin);
	else if (node->v->origin.x > origin.x)
		node->right = Chunk_insertHelper(chunk, node->right, origin);
	else if (node->v->origin.y < origin.y)
		node->left = Chunk_insertHelper(chunk, node->left, origin);
	else if (node->v->origin.y > origin.y)
		node->right = Chunk_insertHelper(chunk, node->right, origin);
	else if (node->v->origin.z < origin.z)
		node->left = Chunk_insertHelper(chunk, node->left, origin);
	else if (node->v->origin.z > origin.z)
		node->right = Chunk_insertHelper(chunk, node->right, origin);
	return node;
}

void Chunk_drawAll(Chunk* chunk, float x_offset, float y_offset, float scale) {
//	if (!chunk) return;
//
//	Chunk_drawAllHelper(chunk->root, x_offset, y_offset, scale);
	if (!chunk || !chunk->display) return;

	Chunk_drawAllHelper(chunk->display->root, x_offset, y_offset, scale);
}

void Chunk_drawAllHelper(VoxelNode* node, float x_offset, float y_offset, float scale) {
	if (!node) return;

	Voxel_drawDisplay(node->v, x_offset, y_offset, scale);
	Chunk_drawAllHelper(node->left, x_offset, y_offset, scale);
	Chunk_drawAllHelper(node->right, x_offset, y_offset, scale);
}

void Chunk_rotateAll(Chunk* chunk, const Vector3 axis, float rotation_speed) {
//	if (!chunk) return;
//
//	Chunk_rotateAllHelper(chunk->root, axis, rotation_speed);
	if (!chunk || !chunk->display) return;

	Chunk_rotateAllHelper(chunk->display->root, axis, rotation_speed);
}

void Chunk_rotateAllHelper(VoxelNode* node, const Vector3 axis, float rotation_speed) {
	if (!node) return;

	Voxel_rotateDisplay(node->v, axis, rotation_speed);
	Chunk_rotateAllHelper(node->left, axis, rotation_speed);
	Chunk_rotateAllHelper(node->right, axis, rotation_speed);
}

void Chunk_translateAll(Chunk* chunk, const Vector3 axis, float move_speed) {
//	if (!chunk) return;
//
//	Chunk_translateAllHelper(chunk->root, axis, move_speed);
	if (!chunk || !chunk->display) return;

	Chunk_translateAllHelper(chunk->display->root, axis, move_speed);
}

void Chunk_translateAllHelper(VoxelNode* node, const Vector3 axis, float move_speed) {
	if (!node) return;

	Voxel_translateDisplay(node->v, axis, move_speed);
	Chunk_translateAllHelper(node->left, axis, move_speed);
	Chunk_translateAllHelper(node->right, axis, move_speed);
}

VoxelNode* Chunk_searchVoxelNode(Chunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	VoxelNode* curr = chunk->root;

	while (curr) {
		if (curr->v->origin.x < origin.x)
			curr = curr->left;
		else if (curr->v->origin.x > origin.x)
			curr = curr->right;
		else if (curr->v->origin.y < origin.y)
			curr = curr->left;
		else if (curr->v->origin.y > origin.y)
			curr = curr->right;
		else if (curr->v->origin.z < origin.z)
			curr = curr->left;
		else if (curr->v->origin.z > origin.z)
			curr = curr->right;
		else
			return curr;
	}
	return nullptr;
}

Voxel* Chunk_searchVoxel(Chunk* chunk, const Vector3 origin) {
	if (!chunk) return nullptr;

	VoxelNode* node = Chunk_searchVoxelNode(chunk, origin);
	if (!node)
		return nullptr;
	else
		return node->v;
}

void Chunk_resetAll(Chunk* chunk) {
//	if (!chunk) return;
//
//	Chunk_resetAllHelper(chunk->root);
	if (!chunk || !chunk->display) return;

	Chunk_resetAllHelper(chunk->display->root);
}

void Chunk_resetAllHelper(VoxelNode* node) {
	if (!node) return;

	Voxel_resetDisplay(node->v);
	Chunk_resetAllHelper(node->left);
	Chunk_resetAllHelper(node->right);
}

ChunkDisplay* ChunkDisplay_init(ChunkDisplay* display, Chunk* chunk) {
	if (!display || !chunk) return nullptr;

	ChunkDisplay_initHelper(display, chunk->root);
	return display;
}
void ChunkDisplay_initHelper(ChunkDisplay* display, VoxelNode* node) {
	if (!node) return;

	if (Voxel_isVisible(node->v)) ChunkDisplay_insert(display, node);
	ChunkDisplay_initHelper(display, node->left);
	ChunkDisplay_initHelper(display, node->right);
}

void ChunkDisplay_insert(ChunkDisplay* display, VoxelNode* insert_node) {
	if (!display || !insert_node) return;

	display->root = ChunkDisplay_insertHelper(display->root, insert_node);
}

VoxelNode* ChunkDisplay_insertHelper(VoxelNode* node, VoxelNode* insert_node) {
	if (!insert_node) return node;

	if (!node) {
		node = (VoxelNode*)malloc(sizeof(VoxelNode));
		// alt init
		node->v = insert_node->v;
		node->left = nullptr;
		node->right = nullptr;

		return node;
	}

	Vector3 origin = insert_node->v->origin;

	if (node->v->origin.x < origin.x)
		node->left = ChunkDisplay_insertHelper(node->left, insert_node);
	else if (node->v->origin.x > origin.x)
		node->right = ChunkDisplay_insertHelper(node->right, insert_node);
	else if (node->v->origin.y < origin.y)
		node->left = ChunkDisplay_insertHelper(node->left, insert_node);
	else if (node->v->origin.y > origin.y)
		node->right = ChunkDisplay_insertHelper(node->right, insert_node);
	else if (node->v->origin.z < origin.z)
		node->left = ChunkDisplay_insertHelper(node->left, insert_node);
	else if (node->v->origin.z > origin.z)
		node->right = ChunkDisplay_insertHelper(node->right, insert_node);
	return node;
}
