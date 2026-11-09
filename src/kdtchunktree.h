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
	struct KDTChunkTree* loaded;
} KDTChunkTree;

KDTChunkNode* KDTChunkNode_init(KDTChunkNode* node, const Vector3 origin, uint8_t axis);
KDTChunkNode* KDTChunkNode_initChunk(KDTChunkNode* node, KDTChunk* chunk, uint8_t axis);
void KDTChunkNode_clear(KDTChunkNode* node);
void KDTChunkNode_delete(KDTChunkNode* node);

KDTChunkTree* KDTChunkTree_init(KDTChunkTree* tree);
void KDTChunkTree_clear(KDTChunkTree* tree);
void KDTChunkTree_delete(KDTChunkTree* tree);

KDTChunkNode* KDTChunkTree_insertRaw(KDTChunkTree* tree, const Vector3 pos);

KDTChunkNode* KDTChunkTree_searchRaw(KDTChunkTree* tree, const Vector3 pos);
KDTChunk* KDTChunkTree_searchRaw_Chunk(KDTChunkTree* tree, const Vector3 pos);

void KDTChunkTree_initLoaded(KDTChunkTree* tree);
void KDTChunkTree_clearLoaded(KDTChunkTree* tree);
void KDTChunkTree_deleteLoaded(KDTChunkTree* tree);

KDTChunkNode* KDTChunkTree_insertLoaded(KDTChunkTree* tree, KDTChunk* chunk);

void KDTChunkTree_rotateLoaded(KDTChunkTree* tree, const Vector3 axis, const float rotation_speed);
void KDTChunkTree_translateLoaded(KDTChunkTree* tree, const Vector3 axis, const float move_speed);
void KDTChunkTree_resetLoaded(KDTChunkTree* tree);
void KDTChunkTree_drawLoaded(KDTChunkTree* tree, float x_offset, float y_offset, float scale);

#endif
