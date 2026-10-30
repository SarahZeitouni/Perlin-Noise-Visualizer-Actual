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

Vector2 Voxel_flat(const Vector3 pos, float x_offset, float y_offset, float scale) {
	return (Vector2){ scale*pos.x/pos.y + x_offset, -1*scale*pos.z/pos.y + y_offset};
}

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
	if (!voxel) return;

	for (int i = 0; i < 8; i ++)
		voxel->disp_v[i] = Vector3RotateByAxisAngle(voxel->disp_v[i], axis, rotation_speed);
}

void Voxel_translateDisplay(Voxel* voxel, const Vector3 axis, float move_speed) {
	if (!voxel) return;

	for (int i = 0; i < 8; i ++)
		voxel->disp_v[i] = Vector3Add(voxel->disp_v[i], (Vector3){axis.x*move_speed, axis.y*move_speed, axis.z*move_speed});
}

void Voxel_resetDisplay(Voxel* voxel) {
	if (!voxel) return;

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
	if (!voxel) return false;

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

void Voxel_draw(Voxel* voxel, float x_offset, float y_offset, float scale) {
	if (!voxel) return;

	float x = x_offset;
	float y = y_offset;
	float s = scale;
	for (int i = 0; i < 12; i ++) {
		int left = 3*i + (i/2) - 6*(i/3) - 8*(i/4) - 2*(i/5) + 3*(i/6) - 2*(i/7) + (i/8) + 4*(i/9) + 2*(i/10) - 4*(i/11);
		int right = 1 + i + 3*(i/2) - 2*(i/3) - 5*(i/4) - (i/6) - 3*(i/8);
		int face = 4*i - 6*(i/2) - (i/3) - 7*(i/5) + 4*(i/6) - 3*(i/7) + 3*(i/8) - 2*(i/9) + 6*(i/10);
		int next_face = 4 - 2*i + 5*(i/2) - 3*(i/3) - 3*(i/4) + 3*(i/5) + (i/6) + 3*(i/7) + (i/8) + 2*(i/9) - 2*(i/10);
		if (voxel->adj[face] || (voxel->adj[next_face] && !voxel->adj[next_face]->adj[face]))
			continue;
		if (voxel->disp_v[left].y < 0 || voxel->disp_v[right].y < 0)
			continue;
		DrawLineV(Voxel_flat(voxel->disp_v[left],x,y,s), Voxel_flat(voxel->disp_v[right], x,y,s), BLACK);
	}
}

void Voxel_forceDraw(Voxel* voxel, float x_offset, float y_offset, float scale) {
	if (!voxel) return;

	float x = x_offset;
	float y = y_offset;
	float s = scale;
	for (int i = 0; i < 12; i ++) {
		int left = 3*i + (i/2) - 6*(i/3) - 8*(i/4) - 2*(i/5) + 3*(i/6) - 2*(i/7) + (i/8) + 4*(i/9) + 2*(i/10) - 4*(i/11);
		int right = 1 + i + 3*(i/2) - 2*(i/3) - 5*(i/4) - (i/6) - 3*(i/8);
		if (voxel->disp_v[left].y < 0 || voxel->disp_v[right].y < 0)
			continue;
		DrawLineV(Voxel_flat(voxel->disp_v[left],x,y,s), Voxel_flat(voxel->disp_v[right], x,y,s), RED);
	}
}
