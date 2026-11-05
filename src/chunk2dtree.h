#ifndef CHUNK2DTREE_H
#define CHUNK2DTREE_H

#include "voxel.h"
#include "arrchunk.h"
#include "stdint.h"
#include "raymath.h"

typedef struct KDTNode {
	ArrChunk* chunk;
	int axis;
	struct KDTNode* children[2];
	uint32_t addr[2];
} KDTNode;

typedef struct ChunkKDT {
	KDTNode* root;
	int count;
} ChunkKDT;

KDTNode* KDTNode_init(KDTNode* node, ArrChunk* chunk, int axis);

ChunkKDT* ChunkKDT_init(ChunkKDT* tree);

void ChunkKDT_delete(ChunkKDT* tree);

KDTNode* ChunkKDT_insert(ChunkKDT* tree, ArrChunk* chunk);

KDTNode* ChunkKDT_insertNew(ChunkKDT* tree, const Vector3 origin);

KDTNode* ChunkKDT_search(ChunkKDT* tree, const Vector3 origin);

KDTNode* ChunkKDT_find(ChunkKDT* tree, const Vector3 pos);

void ChunkKDT_setVis(ChunkKDT* tree);

void ChunkKDT_resetVis(ChunkKDT* tree);

void ChunkKDT_rotateVis(ChunkKDT* tree, const Vector3 axis, const float rotation_speed);

void ChunkKDT_translateVis(ChunkKDT* tree, const Vector3 axis, const float move_speed);

void ChunkKDT_drawVis(ChunkKDT* tree, float x_offset, float y_offset, float scale);

#endif
