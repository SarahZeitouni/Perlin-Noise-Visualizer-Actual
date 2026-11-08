#ifndef KDTCHUNK_H
#define KDTCHUNK_H

#include "voxel.h"
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>

typedef struct KDTVoxelNode {
	Voxel* 	voxel;
	uint8_t	axis;
	struct 	KDTVoxelNode* children[2];
} KDTVoxelNode;

typedef struct KDTChunk {
	Vector3		origin;
	KDTVoxelNode*	root;
	struct KDTChunk* vis;
} KDTChunk;

KDTVoxelNode* KDTVoxelNode_init(KDTVoxelNode* node, const Vector3 origin, const uint8_t axis);

KDTChunk* KDTChunk_init(KDTChunk* chunk, const Vector3 origin);

void KDTChunk_delete(KDTChunk* chunk);

void KDTChunk_insertRaw(KDTChunk* chunk, const Vector3 pos);

void KDTChunk_insertLocal(KDTChunk* chunk, const Vector3 origin);

KDTVoxelNode* KDTChunk_searchRaw(KDTChunk* chunk, const Vector3 pos);

Voxel* KDTChunk_searchRawVoxel(KDTChunk* chunk, const Vector3 pos);

KDTVoxelNode* KDTChunk_searchLocal(KDTChunk* chunk, const Vector3 origin);

Voxel* KDTChunk_searchLocalVoxel(KDTChunk* chunk, const Vector3 origin);

void KDTChunk_removeNode(KDTChunk* chunk, KDTVoxelNode* node);

void KDTChunk_removeRaw(KDTChunk* chunk, const Vector3 pos);

void KDTChunk_removeLocal(KDTChunk* chunk, const Vector3 origin);

void KDTChunk_deleteVis(KDTChunk* chunk);

void KDTChunk_insertVis(KDTChunk* chunk, Voxel* voxel);

void KDTChunk_initVis(KDTChunk* chunk);

void KDTChunk_resetVis(KDTChunk* chunk);

void KDTChunk_rotateVis(KDTChunk* chunk, const Vector3 axis, const float rotation_speed);

void KDTChunk_translateVis(KDTChunk* chunk, const Vector3 axis, const float move_speed);

void KDTChunk_drawVis(KDTChunk* chunk, float x_offset, float y_offset, float scale);

Vector3 KDTChunk_getOrigin(const Vector3 pos);

bool KDTChunk_inRangeLocal(const Vector3 origin);

bool KDTChunk_inRangeRaw(KDTChunk* chunk, const Vector3 pos);

#endif
