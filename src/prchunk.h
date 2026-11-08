#ifndef PRCHUNK_H
#define PRCHUNK_H

#include "voxel.h"
#include <raymath.h>
#include <stdint.h>

typedef struct PRVoxelNode {
	Vector3 v1;
	Vector3 v2;
	bool 	leaf;
	Voxel* 	voxel;
	uint8_t count;
	struct 	PRVoxelNode** children;
} PRVoxelNode;

typedef struct PRChunk {
	Vector3 origin;
	PRVoxelNode* root;
	struct PRChunk* vis;
} PRChunk;

PRVoxelNode* PRVoxelNode_init(PRVoxelNode* node, const Vector3 v1, const Vector3 v2, const Vector3 origin);
void PRVoxelNode_clear(PRVoxelNode* node);
void PRVoxelNode_delete(PRVoxelNode* node);

PRChunk* PRChunk_init(PRChunk* chunk, const Vector3 origin);
void PRChunk_clear(PRChunk* chunk);
void PRChunk_delete(PRChunk* chunk);

PRVoxelNode* PRChunk_insertRaw(PRChunk* chunk, const Vector3 pos);
PRVoxelNode* PRChunk_insertLocal(PRChunk* chunk, const Vector3 origin);

PRVoxelNode* PRChunk_searchRaw(PRChunk* chunk, const Vector3 pos);
PRVoxelNode* PRChunk_searchLocal(PRChunk* chunk, const Vector3 origin);
Voxel* PRChunk_searchRaw_Voxel(PRChunk* chunk, const Vector3 pos);
Voxel* PRChunk_searchLocal_Voxel(PRChunk* chunk, const Vector3 origin);

void PRChunk_initVis(PRChunk* chunk);
void PRChunk_clearVis(PRChunk* chunk);
void PRChunk_deleteVis(PRChunk* chunk);

PRVoxelNode* PRChunk_insertVis(PRChunk* chunk, Voxel* voxel);

void PRChunk_rotateVis(PRChunk* chunk, const Vector3 axis, const float rotation_speed);
void PRChunk_translateVis(PRChunk* chunk, const Vector3 axis, const float move_speed);
void PRChunk_resetVis(PRChunk* chunk);
void PRChunk_drawVis(PRChunk* chunk, float x_offset, float y_offset, float scale);

int PRVoxelNode_getOctant(PRVoxelNode* node, const Vector3 pos);

bool PRVoxelNode_inRange(PRVoxelNode* node, const Vector3 pos);

bool PRChunk_inRangeRaw(PRChunk* chunk, const Vector3 pos);

#endif
