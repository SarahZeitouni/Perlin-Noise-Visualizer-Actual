#ifndef KDTCHUNKTREE_H
#define KDTCHUNKTREE_H

#include "voxel.h"
#include "kdtchunk.h"
#include <stdint.h>
#include <raymath.h>

typedef struct KDTChunkNode {
	KDTChunk* 	chunk;
	uint8_t 	axis;
	struct 	KDTChunkNode* children[2];
} KDTChunkNode;

typedef struct KDTChunkTree {
	KDTChunkNode* root;
} KDTChunkTree;

KDTChunkNode* KDTChunkNode_init(KDTChunkNode* node, KDTChunk* chunk, uint8_t axis);

KDTChunkTree* KDTChunkTree_init(KDTChunkTree* tree);

void KDTChunkTree_delete(KDTChunkTree* tree);

KDTChunkNode* KDTChunkTree_insert(KDTChunkTree* tree, KDTChunk* chunk);

KDTChunkNode* KDTChunkTree_insertNew(KDTChunkTree* tree, const Vector3 origin);

KDTChunkNode* KDTChunkTree_search(KDTChunkTree* tree, const Vector3 pos);

KDTChunk* KDTChunkTree_searchChunk(KDTChunkTree* tree, const Vector3 pos);

void KDTChunkTree_deleteVis(KDTChunkTree* tree);

void KDTChunkTree_initVis(KDTChunkTree* tree);

void KDTChunkTree_resetVis(KDTChunkTree* tree);

void KDTChunkTree_rotateVis(KDTChunkTree* tree, const Vector3 axis, const float rotation_speed);

void KDTChunkTree_translateVis(KDTChunkTree* tree, const Vector3 axis, const float move_speed);

void KDTChunkTree_drawVis(KDTChunkTree* tree, float x_offset, float y_offset, float scale);

#endif
