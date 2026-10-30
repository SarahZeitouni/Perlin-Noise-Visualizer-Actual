#include "raylib.h"
#include "raymath.h"

Vector2 V3_flat(const Vector3 fat, float x_offset, float y_offset, float scale);

// spans from origin = (a, b, c) to (a+1, b+1, c+1)
typedef struct Voxel {
	Vector3 origin;
	Vector3 disp_v[8];
	struct Voxel* adj[6];
} Voxel;

Voxel* Voxel_init(Voxel* voxel, const Vector3 origin);

void Voxel_drawDisplay(Voxel* const voxel, float x_offset, float y_offset, float scale);
void Voxel_forceDrawDisplay(Voxel* const voxel, float x_offset, float y_offset, float scale);

void Voxel_rotateDisplay(Voxel* voxel, const Vector3 axis, float rotation_speed);

void Voxel_translateDisplay(Voxel* voxel, const Vector3 axis, float move_speed);

void Voxel_resetDisplay(Voxel* voxel);

bool Voxel_isVisible(Voxel* voxel);

Vector3 Voxel_getOrigin(Vector3 pos);

typedef struct VoxelNode {
	Voxel* v;
	struct VoxelNode* left;
	struct VoxelNode* right;
} VoxelNode;

VoxelNode* VoxelNode_init(VoxelNode* node, const Vector3 origin);

typedef struct ChunkDisplay {
	VoxelNode* root;
} ChunkDisplay;

typedef struct Chunk {
	Vector3 origin;
	VoxelNode* root;
	ChunkDisplay* display;
} Chunk;

Chunk* Chunk_init(Chunk* chunk, const Vector3 origin);

void Chunk_insert(Chunk* chunk, const Vector3 origin);
void Chunk_insertArray(Chunk* chunk, const Vector3* array, int size);
VoxelNode* Chunk_insertHelper(Chunk* chunk, VoxelNode* node, const Vector3 origin);

void Chunk_drawAll(Chunk* chunk, float x_offset, float y_offset, float scale);
void Chunk_drawAllHelper(VoxelNode* node, float x_offset, float y_offset, float scale);

void Chunk_rotateAll(Chunk* chunk, const Vector3 axis, float rotation_speed);
void Chunk_rotateAllHelper(VoxelNode* node, const Vector3 axis, float rotation_speed);

void Chunk_translateAll(Chunk* chunk, const Vector3 axis, float move_speed);
void Chunk_translateAllHelper(VoxelNode* node, const Vector3 axis, float move_speed);

VoxelNode* Chunk_searchVoxelNode(Chunk* chunk, const Vector3 origin);
Voxel* Chunk_searchVoxel(Chunk* chunk, const Vector3 origin);

void Chunk_resetAll(Chunk* chunk);
void Chunk_resetAllHelper(VoxelNode* node);

ChunkDisplay* ChunkDisplay_init(ChunkDisplay* display, Chunk* chunk);
void ChunkDisplay_initHelper(ChunkDisplay* display, VoxelNode* node);

void ChunkDisplay_insert(ChunkDisplay* display, VoxelNode* insert_node);
VoxelNode* ChunkDisplay_insertHelper(VoxelNode* node, VoxelNode* insert_node);
