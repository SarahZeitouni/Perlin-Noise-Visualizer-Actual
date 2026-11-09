#include "voxel.h"
#include "kdtchunk.h"
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>

//typedef struct KDTVoxelNode {
//	Voxel* 	voxel;
//	uint8_t	axis;
//	struct 	KDTVoxelNode* children[2];
//} KDTVoxelNode;
//
//typedef struct KDTChunk {
//	Vector3		origin;
//	KDTVoxelNode*	root;
//	struct KDTChunk* vis;
//} KDTChunk;

KDTVoxelNode* KDTVoxelNode_init(KDTVoxelNode* node, const Vector3 origin, const uint8_t axis) {
	if (!node) return nullptr;

	node->voxel = (Voxel*)malloc(sizeof(Voxel));
	Voxel_init(node->voxel, origin);
	node->axis = axis;
	node->children[0] = nullptr;
	node->children[1] = nullptr;

	return node;
}

KDTVoxelNode* KDTVoxelNode_initVoxel(KDTVoxelNode* node, Voxel* voxel, const uint8_t axis) {
	if (!node || !voxel) return nullptr;

	node->voxel = voxel;
	node->axis = axis;
	node->children[0] = nullptr;
	node->children[1] = nullptr;

	return node;
}

void KDTVoxelNode_clear(KDTVoxelNode* node) {
	if (!node) return;

	free(node->voxel);
	free(node->children);
}
void KDTVoxelNode_delete(KDTVoxelNode* node) {
	if (!node) return;

	KDTVoxelNode_clear(node);
	free(node);
}

KDTChunk* KDTChunk_init(KDTChunk* chunk, const Vector3 origin) {
	if (!chunk) return;

	chunk->origin 	= KDTChunk_getOrigin(origin);
	chunk->root 	= nullptr;
	chunk->vis->origin 	= chunk->origin;
	chunk->vis->root 	= nullptr;

	return chunk;
}

void KDTChunk_clear(KDTChunk* chunk) {
	if (!chunk) return;

	free(chunk->root);
	free(chunk->vis->root);
	free(chunk->vis);
}

void KDTChunk_delete(KDTChunk* chunk) {
	if (!chunk) return;

	KDTChunk_clear(chunk);
	free(chunk);
}

//KDTVoxelNode* KDTChunk_insertRawHelper(KDT)

KDTVoxelNode* KDTChunk_insertRaw(KDTChunk* chunk, const Vector3 pos) {
	if (!chunk) return nullptr;
}

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

Vector3 KDTChunk_getOrigin(const Vector3 pos) {
	Vector3 o = Voxel_getOrigin(pos);

	return (Vector3){
		(int)o.x < 0 && (int)o.x % 16 != 0 ? ((int)o.x/16)*16 - 16 : ((int)o.x/16)*16,
		(int)o.y < 0 && (int)o.y % 128 != 0 ? ((int)o.y/128)*128 - 128 : ((int)o.y/128)*128,
		(int)o.z < 0 && (int)o.z % 16 != 0 ? ((int)o.z/16)*16 - 16 : ((int)o.z/16)*16
	};
}

bool KDTChunk_inRangeLocal(const Vector3 origin);

bool KDTChunk_inRangeRaw(KDTChunk* chunk, const Vector3 pos);
