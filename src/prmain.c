#include "voxel.h"
#include "prchunk.h"
#include "prchunk.h"
#include "perlinNoiseGen.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define PERLIN_WIDTH 	4
#define PERLIN_HEIGHT 	4
#define PERLIN_SCALE	0.25

//static void PRChunk_Perlin(PRchunk* tree, int x0, int z0, int x_size, int z_size, float scale) {
//	uint8_t* pixels = Perlin_generate(x_size*16, z_size*16);
//	for (int i = 0; i < x_size; i ++) {
//		for (int j = 0; j < z_size; j ++) {
//			PRChunkNode* node = PRchunk_insertNew(tree, (Vector3){(i + x0)*16, 0, (j + z0)*16});
//			PRChunk* chunk = node ? node->chunk : nullptr;
//			if (!chunk)
//				continue;
//			for (int x = 0; x < 16; x++) {
//				for (int z = 0; z < 16; z ++) {
//					int index = (i*16*64 + j*16 + x*64 + z)*4;
//					for (int y = 0; y < pixels[index]*scale; y ++)
//						PRChunk_insertLocal(&chunk, (Vector3){x, y, z});
//				}
//			}
//		}
//	}
//	free(pixels);
//}

int main(void) {
	PRChunk chunk;
	PRChunk* c1 = PRChunk_init(&chunk, (Vector3){ 0, 0, 0 });
	if (c1)
		printf("c1\n");
	else
		printf("n c1\n");

	PRVoxelNode* v1 = PRChunk_insertLocal(&chunk, (Vector3){ 0, 0, 0 });
	if (v1)
		printf("v1\n");
	else
		printf("n v1\n");

	PRVoxelNode* v2 = PRChunk_insertLocal(&chunk, (Vector3){ 1, 0, 0 });
	if (v2)
		printf("v2\n");
	else
		printf("n v2\n");

	PRChunk_clear(&chunk);
	c1 = PRChunk_init(&chunk, (Vector3){ 0, 0, 0 });

	int stop = 0;

	for (int x = 0; x < 16; x ++) {
		for (int z = 0; z < 16; z ++) {
			for (int y = 0; y < 8; y ++) {
				if (!PRChunk_insertLocal(&chunk, (Vector3){ x, y, z })) {
					printf("insrt1(%d, %d, %d)\n", x, y, z);
					stop = 1;
					break;
				}
			}
			if (stop) break;
		}
		if (stop) break;
	}

	stop = 0;

	for (int x = 0; x < 16; x ++) {
		for (int z = 0; z < 16; z ++) {
			for (int y = 0; y < 8; y ++) {
				if (!PRChunk_searchLocal(&chunk, (Vector3){ x, y, z })) {
					printf("srch1(%d, %d, %d)\n", x, y, z);
					stop = 1;
					break;
				}
			}
			if (stop) break;
		}
		if (stop) break;
	}

	PRChunk_clear(&chunk);
	c1 = PRChunk_init(&chunk, (Vector3){ 0, 0, 0 });

	stop = 0;

	for (int x = 0; x < 16; x ++) {
		for (int z = 0; z < 16; z ++) {
			if (!PRChunk_insertLocal(&chunk, (Vector3){ x, 0, z })) {
				printf("insrt2(%d, 0, %d)\n", x, z);
				stop = 1;
				break;
			}
		}
		if (stop) break;
	}

	PRChunk_initVis(&chunk);

	stop = 0;

	for (int x = 0; x < 16; x ++) {
		for (int z = 0; z < 16; z ++) {
			if (!PRChunk_searchLocal(chunk.vis, (Vector3){ x, 0, z })) {
				printf("srch2(%d, 0, %d)\n", x, z);
				stop = 1;
				break;
			}
		}
		if (stop) break;
	}

	Voxel* vox = PRChunk_searchLocal_Voxel(&chunk, (Vector3){ 0, 0, 0 });
	if (vox && Voxel_isVisible(vox))
		printf("visible\n");
	else
		printf("nvisible\n");

	const Vector3 x_axis = { 1.0f, 0.0f, 0.0f };
	const Vector3 y_axis = { 0.0f, 1.0f, 0.0f };
	const Vector3 z_axis = { 0.0f, 0.0f, 1.0f };
	Vector3 x_axis_move = x_axis;
	Vector3 z_axis_move = z_axis;
	Vector3 y_axis_display = y_axis;
	Vector3 z_axis_display = z_axis;
	float yaw 	= 0.0f;
	float pitch = 0.0f;

	const int screenWidth = 1600;
	const int screenHeight = 900;

	const float x_offset = (float)screenWidth / 2;
	const float y_offset = (float)screenHeight / 2;
	const float scale = (float)screenHeight * 0.4f;

	Vector3 player_origin = { 8.0f, 16.0f, 8.0f };
	Vector3 player_pos = player_origin;
	Vector3 player_view_pos = Vector3Add(player_pos, (Vector3){0, 1.6f, 0});
	Vector3 player_velocity = { 0.0f, 0.0f, 0.0f };
	bool isTouchingFloor = false;

	float rotation_speed = 0.05f;
	float nrotation_speed = -0.05f;
	float move_speed = 0.1f;
	float nmove_speed = -1*move_speed;
	float adjust_speed = 0.01f;
	float jump_speed = 0.19f;
	float gravity = -0.015f;

	PRChunk_clear(&chunk);
	PRChunk_init(&chunk, (Vector3){ 0, 0, 0 });

	stop = 0;

	printf("bpm0\n");

	for (int x = 0; x < 16; x ++) {
		for (int z = 0; z < 16; z ++) {
			for (int y = 0; y < 16; y ++) {
				if(!PRChunk_insertLocal(&chunk, (Vector3){ x, y, z })) {
					printf("insrt3(%d, %d, %d)\n", x, y, z);
					stop = 1;
					break;
				}
			}
			if (stop) break;
		}
		if (stop) break;
	}
	printf("\n");

	printf("bpm0.1\n");

	PRChunk_initVis(&chunk);
	PRChunk_translateVis(&chunk, player_view_pos, -1);

	printf("bpm1\n");

	stop = 0;

	for (int x = 0; x < 16; x ++) {
		for (int z = 0; z < 16; z ++) {
			for (int y = 0; y < 16; y ++) {
				if (!PRChunk_searchRaw(&chunk, (Vector3){ x, y, z })) {
					printf("srch3(%d, %d, %d)\n", x, y, z);
					stop = 1;
					break;
				}
			}
			if (stop) break;
		}
		if (stop) break;
	}

	printf("bpm2\n");

	if (!PRChunk_searchRaw(&chunk, (Vector3){ player_pos.x, player_pos.y - 0.5, player_pos.z })) {
		printf("srch3.1(%f, %f, %f)\n", player_pos.x, player_pos.y - 0.5, player_pos.z);
	}

	printf("bpm2.1\n");

	if (!PRChunk_searchRaw(&chunk, (Vector3){ player_pos.x - 0.5, player_pos.y - 0.5, player_pos.z - 0.5 })) {
		printf("srch3.2(%f, %f, %f)\n", player_pos.x - 0.5, player_pos.y - 0.5, player_pos.z - 0.5);
	}

	printf("bpm2.2\n");

	if (!PRChunk_searchRaw(&chunk, (Vector3){ 7, 15, 7 })) {
		printf("srch3.3(%d, %d, %d)\n", 7, 15, 7);
	}

	printf("bpm2.3\n");

	if (!PRChunk_searchRaw(&chunk, (Vector3){ 7.5, 15.5, 7.5 })) {
		printf("srch3.4(%f, %f, %f)\n", 7.5, 15.5, 7.5);
	}

	printf("bpm3\n");

//	return 0;

	InitWindow(screenWidth, screenHeight, "Main");
	SetTargetFPS(60);

	HideCursor();

	while(!WindowShouldClose()) {
		// load new chunks (got rid of block, fix)

//		for (int i = -1; i < 2; i ++) {
//			for (int j = -1; j < 2; j ++) {
//				Vector3 o = Vector3Add(&chunk->origin, (Vector3){16*i, 0, 16*j});
//				if (!PRChunk_search(&chunk, o)) {
//					PRChunk* tmp = PRChunk_insertNew(&chunk, o)->chunk;
//					PRChunk_block(tmp, (Vector3){0, 0, 0}, (Vector3){15, 9, 15});
//					PRChunk_setVis(tmp);
//					PRChunk_translateVis(tmp, player_view_pos, -1);
//					PRChunk_rotateVis(tmp, x_axis, -1*pitch);
//					PRChunk_rotateVis(tmp, y_axis_display, -1*yaw);
//				}
//			}
//		}

		if (IsKeyDown(KEY_LEFT)) {
			PRChunk_rotateVis(&chunk, y_axis_display, nrotation_speed);
			x_axis_move = Vector3RotateByAxisAngle(x_axis_move, y_axis, rotation_speed);
			z_axis_move = Vector3RotateByAxisAngle(z_axis_move, y_axis, rotation_speed);
			yaw += rotation_speed;
		}
		if (IsKeyDown(KEY_RIGHT)) {
			PRChunk_rotateVis(&chunk, y_axis_display, rotation_speed);
			x_axis_move = Vector3RotateByAxisAngle(x_axis_move, y_axis, nrotation_speed);
			z_axis_move = Vector3RotateByAxisAngle(z_axis_move, y_axis, nrotation_speed);
			yaw += nrotation_speed;
		}
		if (IsKeyDown(KEY_UP) && y_axis_display.z > -0.98f) {
			PRChunk_rotateVis(&chunk, x_axis, nrotation_speed);
			y_axis_display = Vector3RotateByAxisAngle(y_axis_display, x_axis, nrotation_speed);
			z_axis_display = Vector3RotateByAxisAngle(z_axis_display, x_axis, nrotation_speed);
			pitch += rotation_speed;
		}
		if (IsKeyDown(KEY_DOWN) && y_axis_display.z < 0.98f) {
			PRChunk_rotateVis(&chunk, x_axis, rotation_speed);
			y_axis_display = Vector3RotateByAxisAngle(y_axis_display, x_axis, rotation_speed);
			z_axis_display = Vector3RotateByAxisAngle(z_axis_display, x_axis, rotation_speed);
			pitch += nrotation_speed;
		}

		Vector3 dtranslate = { 0, 0, 0 };

		if (IsKeyDown(KEY_A)) {
			dtranslate = Vector3Add(dtranslate, Vector3Scale(x_axis, move_speed));
			player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, nmove_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while (PRChunk_searchRaw(&chunk, player_pos) || PRChunk_searchRaw(&chunk, player_view_pos)) {
				dtranslate = Vector3Add(dtranslate, Vector3Scale(x_axis, adjust_speed*nmove_speed));
				player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, adjust_speed*move_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}

		if (IsKeyDown(KEY_D)) {
			dtranslate = Vector3Add(dtranslate, Vector3Scale(x_axis, nmove_speed));
			player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, move_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while(PRChunk_searchRaw(&chunk, player_pos) || PRChunk_searchRaw(&chunk, player_view_pos)) {
				dtranslate = Vector3Add(dtranslate, Vector3Scale(x_axis, adjust_speed*move_speed));
				player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, adjust_speed*nmove_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}


		if (IsKeyDown(KEY_S)) {
			dtranslate = Vector3Add(dtranslate, Vector3Scale(z_axis_display, nmove_speed));
			player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, move_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while(PRChunk_searchRaw(&chunk, player_pos) || PRChunk_searchRaw(&chunk, player_view_pos)) {
				dtranslate = Vector3Add(dtranslate, Vector3Scale(z_axis_display, adjust_speed*move_speed));
				player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, adjust_speed*nmove_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}


		if (IsKeyDown(KEY_W)) {
			dtranslate = Vector3Add(dtranslate, Vector3Scale(z_axis_display, move_speed));
			player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, nmove_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while (PRChunk_searchRaw(&chunk, player_pos) || PRChunk_searchRaw(&chunk, player_view_pos)) {
				dtranslate = Vector3Add(dtranslate, Vector3Scale(z_axis_display, adjust_speed*nmove_speed));
				player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, adjust_speed*move_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}

		// better horizontal collision wip

//		for (Voxel* v0, * v1; (v0 = PRChunk_searchRaw(&chunk, player_pos)) || (v1 = PRChunk_searchRaw(&chunk, player_view_pos));) {
//			int face = Voxel_getNearestXZFace(v1 ? v1 : v0, v1 ? player_view_pos : player_pos);
//			Vector3 vadjust = (Vector3[]){
//				Vector3Scale(z_axis_display, adjust_speed*move_speed),
//				Vector3Scale(x_axis),
//				Vector3Add(Vector3Scale(), Vector3Scale()),
//				(Vector3){}
//			}[face];
//
//			Vector3 padjust = (Vector3[]){
//				(Vector3){ 0, 0, adjust_speed*nmove_speed },
//				(Vector3){ adjust_speed*move_speed, 0, 0 },
//				(Vector3){ 0, 0, adjust_speed*move_speed },
//				(Vector3){ adjust_speed*nmove_speed, 0, 0 }
//			}[face];
//
//			player_pos 		= Vector3Add(player_pos, padjust);
//			player_view_pos = Vector3Add(player_view_pos, padjust);
//			dtranslate 		= Vector3Add(dtranslate, vadjust);
//		}

		// jump
		if (IsKeyDown(KEY_SPACE) && isTouchingFloor) {
			player_velocity.y = jump_speed;
			isTouchingFloor = false;
		}

		// check if no floor beneath player
		Voxel* beneath_player = PRChunk_searchRaw_Voxel(&chunk, Vector3Add(player_pos, (Vector3){0, -0.5f, 0}));
		if (!beneath_player && isTouchingFloor) {
			isTouchingFloor = false;
		}

		if (!isTouchingFloor) {
			Voxel* floor = PRChunk_searchRaw_Voxel(&chunk, (Vector3){player_pos.x, player_pos.y + player_velocity.y, player_pos.z});
			if (!floor)
				floor = PRChunk_searchRaw_Voxel(&chunk, player_pos);
			if (floor && player_velocity.y < 0) {
				dtranslate = Vector3Add(dtranslate, Vector3Scale(y_axis_display, player_pos.y - (floor->origin.y + 1)));
				player_pos.y = floor->origin.y + 1;
				player_view_pos.y = player_pos.y + 1.6;
				player_velocity.y = 0.0f;
				isTouchingFloor = true;
			}
			else {
				player_pos.y += player_velocity.y;
				player_view_pos.y += player_velocity.y;
				dtranslate = Vector3Add(dtranslate, Vector3Scale(y_axis_display, -1*player_velocity.y));
				player_velocity.y += gravity;
			}
		}

		// highlight selected voxel
		Voxel* selected = nullptr;
		for (int i = 0; i < 30; i ++) {
			Vector3 search_pos = Vector3RotateByAxisAngle(z_axis_display, y_axis, PI + yaw);
			search_pos = Vector3Add(player_view_pos, Vector3Scale(search_pos, i*0.2f));
			selected = PRChunk_searchRaw_Voxel(&chunk, search_pos);

			if (selected)
				break;
		}

//		if (selected && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_F))) {
//			int p[6][2] = { {0, 2}, {1, 3}, {2, 0}, {3, 1}, {4, 5}, {5, 4} };
//			for (int i = 0; i < 6; i ++) {
//	     		if (selected->adj[p[i][0]]) {
//	     			selected->adj[p[i][0]]->adj[p[i][1]] = nullptr;
//	        	    if (selected->adj[p[i][0]]->alive && !selected->adj[p[i][0]]->visible) {
//	            		PRChunk_insertVis(&chunk, selected->adj[p[i][0]]);
//						Voxel_translateDisplay(selected->adj[p[i][0]], player_view_pos, -1);
//						Voxel_rotateDisplay(selected->adj[p[i][0]], x_axis, -1*pitch);
//						Voxel_rotateDisplay(selected->adj[p[i][0]], y_axis_display, -1*yaw);
//					}
//	        	}
//	    	}
//			PRChunk_removeVoxel(&chunk, selected);
//		}

		if (IsKeyPressed(KEY_Q) || player_pos.y < -20) {
			y_axis_display = y_axis;
			z_axis_display = z_axis;
			x_axis_move = x_axis;
			z_axis_move = z_axis;
			yaw = 0.0f;
			player_pos = player_origin;
			player_view_pos = Vector3Add(player_pos, (Vector3){0, 1.6f, 0});
			player_velocity = (Vector3){0, 0, 0};
			PRChunk_resetVis(&chunk);
			dtranslate = Vector3Scale(player_view_pos, -1);
		}

		PRChunk_translateVis(&chunk, dtranslate, 1);

		BeginDrawing();

			ClearBackground(RAYWHITE);
			PRChunk_drawVis(&chunk, x_offset, y_offset, scale);
			if (selected) Voxel_forceDraw(selected, x_offset, y_offset, scale);

			DrawText(TextFormat("(%f, %f, %f)\n(%f, %f, %f)", player_pos.x, player_pos.y, player_pos.z, player_velocity.x, player_velocity.y, player_velocity.z), 0, 0, 20, BLACK);
			DrawText(TextFormat("isTouchingFloor: %d", isTouchingFloor), 0, 40, 20, BLACK);
//			DrawText(TextFormat("chunk: (%f, %f, %f)", chunk ? chunk->origin.x : -1, chunk ? chunk->origin.y : -1, chunk ? chunk->origin.z : -1), 0, 60, 20, BLACK);
			DrawText(TextFormat("yaw: %f\n", yaw), 0, 80, 20, BLACK);

			DrawLineV((Vector2){x_offset - 10, y_offset}, (Vector2){x_offset + 10, y_offset}, BLACK);
			DrawLineV((Vector2){x_offset, y_offset - 10}, (Vector2){x_offset, y_offset + 10}, BLACK);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
