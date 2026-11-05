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
// +y (up)  4-----5  _. -z (back)
//     .   /|    /|  /|
//    /|\ 7-+---6 | /
//     |  | 0---|-1
//     |  |/    |/
//     |  3-----2  ---> +x (right)

static int edges[12][4] = { // left, right, face, next_face
		// around x
		{0, 1, 0, 4},
		{3, 2, 4, 2},
		{7, 6, 2, 5},
		{4, 5, 5, 0},
		// around y
		{0, 4, 3, 0},
		{1, 5, 0, 1},
		{2, 6, 1, 2},
		{3, 7, 2, 3},
		// around z
		{0, 3, 3, 4},
		{1, 2, 4, 1},
		{5, 6, 1, 5},
		{4, 7, 5, 3}
	};

Vector2 Voxel_flat(const Vector3 pos, float x_offset, float y_offset, float scale) {
	return (Vector2){ -1*scale*pos.x/pos.z + x_offset, scale*pos.y/pos.z + y_offset};
}

Voxel* Voxel_init(Voxel* voxel, const Vector3 origin) {
	if (!voxel) return nullptr;

	voxel->origin = origin;

	float x = origin.x;
	float y = origin.y;
	float z = origin.z;
	voxel->disp_v[0] = (Vector3){x, 	y, 		z};		// 0 origin
	voxel->disp_v[1] = (Vector3){x+1, 	y, 		z};		// 1 right
	voxel->disp_v[2] = (Vector3){x+1,	y, 		z+1};	// 2 front-right
	voxel->disp_v[3] = (Vector3){x, 	y, 		z+1};	// 3 front
	voxel->disp_v[4] = (Vector3){x, 	y+1, 	z};		// 4 up
	voxel->disp_v[5] = (Vector3){x+1, 	y+1,	z};		// 5 up-right
	voxel->disp_v[6] = (Vector3){x+1, 	y+1, 	z+1}; 	// 6 up-front-right
	voxel->disp_v[7] = (Vector3){x,		y+1, 	z+1};	// 7 up-front

	voxel->adj[0] = nullptr;	// 0 back
	voxel->adj[1] = nullptr;	// 1 right
	voxel->adj[2] = nullptr;	// 2 front
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
	voxel->disp_v[0] = (Vector3){x, 	y, 		z};		// 0 origin
	voxel->disp_v[1] = (Vector3){x+1, 	y, 		z};		// 1 right
	voxel->disp_v[2] = (Vector3){x+1,	y, 		z+1};	// 2 front-right
	voxel->disp_v[3] = (Vector3){x, 	y, 		z+1};	// 3 front
	voxel->disp_v[4] = (Vector3){x, 	y+1, 	z};		// 4 up
	voxel->disp_v[5] = (Vector3){x+1, 	y+1,	z};		// 5 up-right
	voxel->disp_v[6] = (Vector3){x+1, 	y+1, 	z+1}; 	// 6 up-front-right
	voxel->disp_v[7] = (Vector3){x,		y+1, 	z+1};	// 7 up-front
}

bool Voxel_isVisible(Voxel* voxel) {
	if (!voxel) return false;

	for (int i = 0; i < 6; i ++)
		if (!voxel->adj[i]) return true;
	return false;
}

Vector3 Voxel_getOrigin(Vector3 pos) {
	return (Vector3){
		pos.x < (int)pos.x ? (int)(pos.x - 1) : (int)pos.x,
		pos.y < (int)pos.y ? (int)(pos.y - 1) : (int)pos.y,
		pos.z < (int)pos.z ? (int)(pos.z - 1) : (int)pos.z
	};
}

// +y (up)  4-----5  _. -z (back)
//     .   /|    /|  /|
//    /|\ 7-+---6 | /
//     |  | 0---|-1
//     |  |/    |/
//     |  3-----2  ---> +x (right)
// 0 back
// 1 right
// 2 front
// 3 left
// 4 down
// 5 up

void Voxel_draw(Voxel* voxel, float x_offset, float y_offset, float scale) {
	if (!voxel) return;

	float x = x_offset;
	float y = y_offset;
	float s = scale;

	for (int i = 0; i < 12; i ++) {
		int left 	= edges[i][0];
		int right 	= edges[i][1];
		int face 	= edges[i][2];
		int next 	= edges[i][3];
		if (voxel->adj[face] || (voxel->adj[next] && !voxel->adj[next]->adj[face]))
			continue;
		if (voxel->disp_v[left].z > 0 || voxel->disp_v[right].z > 0)
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
		int left 	= edges[i][0];
		int right 	= edges[i][1];
		if (voxel->disp_v[left].z > 0 || voxel->disp_v[right].z > 0)
			continue;
		DrawLineV(Voxel_flat(voxel->disp_v[left],x,y,s), Voxel_flat(voxel->disp_v[right], x,y,s), RED);
	}
}

// +y (up)  4-----5  _. -z (back)
//     .   /|    /|  /|
//    /|\ 7-+---6 | /
//     |  | 0---|-1
//     |  |/    |/
//     |  3-----2  ---> +x (right)
// 0 back
// 1 right
// 2 front
// 3 left
// 4 down
// 5 up

float Voxel_getPDS(Vector3* points, int size, const Vector3 pos) {
	float sum = 0;
	for (int i = 0; i < size; i ++)
		sum += Vector3Distance(points[i], pos);
	return sum;
}

int Voxel_getNearestXZFace(Voxel* voxel, const Vector3 pos) {
	float x = voxel->origin.x;
	float y = voxel->origin.y;
	float z = voxel->origin.z;
	Vector3 v[8] = {
		(Vector3){x, 	y, 		z},
		(Vector3){x+1, 	y, 		z},
		(Vector3){x+1,	y, 		z+1},
		(Vector3){x, 	y, 		z+1},
		(Vector3){x, 	y+1, 	z},
		(Vector3){x+1, 	y+1,	z},
		(Vector3){x+1, 	y+1, 	z+1},
		(Vector3){x,	y+1, 	z+1}
	};
	float dist[6] = {
		Voxel_getPDS((Vector3[]){v[0], v[1], v[5], v[4]}, 4, pos),
		Voxel_getPDS((Vector3[]){v[1], v[2], v[5], v[6]}, 4, pos),
		Voxel_getPDS((Vector3[]){v[2], v[3], v[6], v[7]}, 4, pos),
		Voxel_getPDS((Vector3[]){v[3], v[0], v[7], v[4]}, 4, pos)
//		Voxel_getPDS((Vector3[]){v[0], v[1], v[2], v[3]}, 4, pos),
//		Voxel_getPDS((Vector3[]){v[4], v[5], v[6], v[7]}, 4, pos)
	};

	int min = 0;
	for (int i = 1; i < 4; i ++) {
		if (dist[i] < dist[min])
			min = i;
	}
	return min;
}
