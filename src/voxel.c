#include "voxel.h"
#include "stdlib.h"
#include "raylib.h"
#include "raymath.h"
#include "stdio.h"

//spans from origin = (a, b, c) to (a+1, b+1, c+1)
//typedef struct {
//	Vector3 origin;
//	Vector3 disp_v[8];
//	struct Voxel* adj[6];
//} Voxel;
//
// z+ (up)  7-----6  _. y+ (back)
//     .   /|    /|  /|
//    /|\ 4-+---5 | /
//     |  | 3---|-2
//     |  |/    |/
//     |  0-----1  ---> x+ (right)

Voxel* Voxel_init(Voxel* voxel, const Vector3 origin) {
	if (!voxel) return nullptr;

	voxel->origin = origin;

	float x = origin.x;
	float y = origin.y;
	float z = origin.z;
	voxel->disp_v[0] = (Vector3){x, 	y, 		z};		// 0 origin
	voxel->disp_v[1] = (Vector3){x+1, 	y, 		z};		// 1 right
	voxel->disp_v[2] = (Vector3){x+1,	y+1, 	z};		// 2 right-back
	voxel->disp_v[3] = (Vector3){x, 	y+1, 	z};		// 3 back
	voxel->disp_v[4] = (Vector3){x, 	y, 		z+1};		// 4 up
	voxel->disp_v[5] = (Vector3){x+1, 	y,	 	z+1};		// 5 up-right
	voxel->disp_v[6] = (Vector3){x+1, 	y+1, 	z+1}; 	// 6 up-right-back
	voxel->disp_v[7] = (Vector3){x,		y+1, 	z+1};	// 7 up-back

	voxel->adj[0] = nullptr;	// 0 front
	voxel->adj[1] = nullptr;	// 1 right
	voxel->adj[2] = nullptr;	// 2 back
	voxel->adj[3] = nullptr;	// 3 left
	voxel->adj[4] = nullptr;	// 4 down
	voxel->adj[5] = nullptr;	// 5 up

	return voxel;
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
    float y = voxel->origin.y;
    float z = voxel->origin.z;
    voxel->disp_v[0] = (Vector3){x,     y,      z};     // 0 origin
    voxel->disp_v[1] = (Vector3){x+1,   y,      z};     // 1 right
    voxel->disp_v[2] = (Vector3){x+1,   y+1,    z};     // 2 right-back
    voxel->disp_v[3] = (Vector3){x,     y+1,    z};     // 3 back
    voxel->disp_v[4] = (Vector3){x,     y,      z+1};   // 4 up
    voxel->disp_v[5] = (Vector3){x+1,   y,      z+1};   // 5 up-right
    voxel->disp_v[6] = (Vector3){x+1,   y+1,    z+1};   // 6 up-right-back
    voxel->disp_v[7] = (Vector3){x,     y+1,    z+1};   // 7 up-back
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
