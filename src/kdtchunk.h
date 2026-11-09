#ifndef KDTCHUNK_H
#define KDTCHUNK_H

#include "voxel.h"
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
KDTVoxelNode* KDTVoxelNode_initVoxel(KDTVoxelNode* node, Voxel* voxel, const uint8_t axis);

void KDTVoxelNode_clear(KDTVoxelNode* node);
void KDTVoxelNode_delete(KDTVoxelNode* node);

KDTChunk* KDTChunk_init(KDTChunk* chunk, const Vector3 origin);

void KDTChunk_clear(KDTChunk* chunk);
void KDTChunk_delete(KDTChunk* chunk);

KDTVoxelNode* KDTChunk_insertRaw(KDTChunk* chunk, const Vector3 pos);
KDTVoxelNode* KDTChunk_insertLocal(KDTChunk* chunk, const Vector3 origin);

KDTVoxelNode* KDTChunk_searchRaw(KDTChunk* chunk, const Vector3 pos);
KDTVoxelNode* KDTChunk_searchLocal(KDTChunk* chunk, const Vector3 origin);
Voxel* KDTChunk_searchRaw_Voxel(KDTChunk* chunk, const Vector3 pos);
Voxel* KDTChunk_searchLocal_Voxel(KDTChunk* chunk, const Vector3 origin);

void KDTChunk_initVis(KDTChunk* chunk);
void KDTChunk_clearVis(KDTChunk* chunk);
void KDTChunk_deleteVis(KDTChunk* chunk);

void KDTChunk_insertVis(KDTChunk* chunk, Voxel* voxel);

void KDTChunk_rotateVis(KDTChunk* chunk, const Vector3 axis, const float rotation_speed);
void KDTChunk_translateVis(KDTChunk* chunk, const Vector3 axis, const float move_speed);
void KDTChunk_resetVis(KDTChunk* chunk);
void KDTChunk_drawVis(KDTChunk* chunk, float x_offset, float y_offset, float scale);

Vector3 KDTChunk_getOrigin(const Vector3 pos);

bool KDTChunk_inRangeLocal(const Vector3 origin);

bool KDTChunk_inRangeRaw(KDTChunk* chunk, const Vector3 pos);

#endif
