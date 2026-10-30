#ifndef VOXEL_H
#define VOXEL_H

#include <raylib.h>
#include <raymath.h>

// spans from origin = (a, b, c) to (a+1, b+1, c+1)
// z+ (up)  7-----6  _. y+ (back)
//     .   /|    /|  /|
//    /|\ 4-+---5 | /
//     |  | 3---|-2
//     |  |/    |/
//     |  0-----1  ---> x+ (right)
typedef struct Voxel {
	Vector3 origin;
	Vector3 disp_v[8];
	struct Voxel* adj[6];
} Voxel;

Voxel* Voxel_init(Voxel* voxel, const Vector3 origin);

void Voxel_rotateDisplay(Voxel* voxel, const Vector3 axis, float rotation_speed);

void Voxel_translateDisplay(Voxel* voxel, const Vector3 axis, float move_speed);

void Voxel_resetDisplay(Voxel* voxel);

bool Voxel_isVisible(Voxel* voxel);

Vector3 Voxel_getOrigin(Vector3 pos);

#endif
