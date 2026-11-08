#ifndef PRCHUNKTREE_H
#define PRCHUNKTREE_H

#include "voxel.h"
#include "prchunk.h"
#include <raymath.h>
#include <stdint.h>

typedef struct PRChunkNode {
	Vector3 	v1;
	Vector3 	v2;
	bool 		leaf;
	PRChunk* 	chunk;
	uint8_t 	count;
	struct 	PRChunkNode** children;
} PRChunkNode;

typedef struct PRChunkTree {
	Vector3 v1;
	Vector3 v2;
	PRChunkNode* root;
	struct PRChunkTree* loaded;
} PRChunkTree;

PRChunkNode* PRChunkNode_init(PRChunkNode* node, const Vector3 v1, const Vector3 v2, const Vector3 origin);
PRChunkNode* PRChunkNode_initChunk(PRChunkNode* node, const Vector3 v1, const Vector3 v2, PRChunk* chunk);
void PRChunkNode_clear(PRChunkNode* node);
void PRChunkNode_delete(PRChunkNode* node);

PRChunkTree* PRChunkTree_init(PRChunkTree* tree, const Vector3 v1, const Vector3 v2);
void PRChunkTree_clear(PRChunkTree* tree);
void PRChunkTree_delete(PRChunkTree* tree);

PRChunkNode* PRChunkTree_insertRaw(PRChunkTree* tree, const Vector3 pos);
//PRChunkNode* PRChunkTree_insertLocal(PRChunkTree* tree, const Vector3 origin);

PRChunkNode* PRChunkTree_searchRaw(PRChunkTree* tree, const Vector3 pos);
//PRChunkNode* PRChunkTree_searchLocal(PRChunkTree* tree, const Vector3 origin);
PRChunk* PRChunkTree_searchRaw_Chunk(PRChunkTree* tree, const Vector3 pos);
//PRChunk* PRChunkTree_searchLocal_Chunk(PRChunkTree* tree, const Vector3 origin);

void PRChunkTree_initLoaded(PRChunkTree* tree);
void PRChunkTree_clearLoaded(PRChunkTree* tree);
void PRChunkTree_deleteLoaded(PRChunkTree* tree);

PRChunkNode* PRChunkTree_insertLoaded(PRChunkTree* tree, PRChunk* chunk);

void PRChunkTree_rotateLoaded(PRChunkTree* tree, const Vector3 axis, const float rotation_speed);
void PRChunkTe_translateLoaded(PRChunkTree* tree, const Vector3 axis, const float move_speed);
void PRChunkTree_resetLoaded(PRChunkTree* tree);
void PRChunkTree_drawLoaded(PRChunkTree* tree, float x_offset, float y_offset, float scale);

int PRChunkNode_getOctant(PRChunkNode* node, const Vector3 pos);

bool PRChunkNode_inRange(PRChunkNode* node, const Vector3 pos);

bool PRChunkTree_inRange(PRChunkTree* tree, const Vector3 pos);

#endif
