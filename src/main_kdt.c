#include "voxel.h"
#include "kdtchunk.h"
#include "kdtchunktree.h"
#include "perlinNoiseGen.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define PERLIN_WIDTH 	4
#define PERLIN_HEIGHT 	4
#define PERLIN_SCALE	0.25
#define PERLIN_SEED		1

static void KDTChunkTree_Perlin(KDTChunkTree* tree, uint32_t x0, uint32_t z0, uint32_t x_size, uint32_t z_size, float scale, int seed) {
	uint8_t* pixels = Perlin_generate(x_size*16, z_size*16, seed);
	for (uint32_t i = 0; i < x_size; i ++) {
		for (uint32_t j = 0; j < z_size; j ++) {
			KDTChunkNode* node = KDTChunkTree_insertRaw(tree, (Vector3){(i + x0)*16, 0, (j + z0)*16});
			KDTChunk* chunk = node ? node->chunk : nullptr;
			if (!chunk) {
				printf("poop\n");
				continue;
			}
			for (int x = 0; x < 16; x++) {
				for (int z = 0; z < 16; z ++) {
					int index = (i*16*64 + j*16 + x*64 + z)*4;
					for (int y = 0; y < pixels[index]*scale; y ++)
						KDTChunk_insertLocal(chunk, (Vector3){x, y, z});
				}
			}
			printf("%d\n", i*x_size + j);
		}
	}
	free(pixels);
}

int main(void) {
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

	Vector3 player_origin = { 15.0f, 63.0f, 15.0f };
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

	KDTChunkTree chunkTree;
	KDTChunkTree_init(&chunkTree);

	printf("init\n");

	KDTChunkTree_Perlin(&chunkTree, 0, 0, PERLIN_WIDTH, PERLIN_HEIGHT, PERLIN_SCALE, PERLIN_SEED);

	printf("perlin\n");

	KDTChunkTree_initLoaded(&chunkTree);
	KDTChunkTree_translateLoaded(&chunkTree, player_view_pos, -1);

	printf("vis\n");

//	printf("0\n");
//
//	KDTChunk* chunk = (KDTChunk*)malloc(sizeof(KDTChunk));
//
//	printf("1\n");
//
//	KDTChunk_init(chunk, (Vector3){ 0, 0, 0 });
//
//	printf("begin\n");
//
//	for (int x = 0; x < 16; x ++) {
//		for (int z = 0; z < 16; z ++) {
//			for (int y = 0; y < 16; y ++) {
//				if (!KDTChunk_insertLocal(chunk, (Vector3){ x, y, z })) {
//					printf("(%d, %d, %d)\n", x, y, z);
//				}
//				if (!KDTChunk_searchLocal(chunk, (Vector3){ x, y, z })) {
//					printf("(%d, %d, %d)\n", x, y, z);
//				}
//			}
//		}
//	}
//
//	printf("inserted\n");
//
//	KDTChunk_initVis(chunk);
//	KDTChunk_translateVis(chunk, player_view_pos, -1);

	InitWindow(screenWidth, screenHeight, "Main");
	SetTargetFPS(60);

	HideCursor();

	while(!WindowShouldClose()) {
		KDTChunk* chunk = KDTChunkTree_searchRaw_Chunk(&chunkTree, player_pos);

		if (IsKeyDown(KEY_LEFT)) {
			KDTChunkTree_rotateLoaded(&chunkTree, y_axis_display, nrotation_speed);
//			KDTChunk_rotateVis(chunk, y_axis_display, nrotation_speed);
			x_axis_move = Vector3RotateByAxisAngle(x_axis_move, y_axis, rotation_speed);
			z_axis_move = Vector3RotateByAxisAngle(z_axis_move, y_axis, rotation_speed);
			yaw += rotation_speed;
		}
		if (IsKeyDown(KEY_RIGHT)) {
			KDTChunkTree_rotateLoaded(&chunkTree, y_axis_display, rotation_speed);
//			KDTChunk_rotateVis(chunk, y_axis_display, rotation_speed);
			x_axis_move = Vector3RotateByAxisAngle(x_axis_move, y_axis, nrotation_speed);
			z_axis_move = Vector3RotateByAxisAngle(z_axis_move, y_axis, nrotation_speed);
			yaw += nrotation_speed;
		}
		if (IsKeyDown(KEY_UP) && y_axis_display.z > -0.98f) {
			KDTChunkTree_rotateLoaded(&chunkTree, x_axis, nrotation_speed);
//			KDTChunk_rotateVis(chunk, x_axis, nrotation_speed);
			y_axis_display = Vector3RotateByAxisAngle(y_axis_display, x_axis, nrotation_speed);
			z_axis_display = Vector3RotateByAxisAngle(z_axis_display, x_axis, nrotation_speed);
			pitch += rotation_speed;
		}
		if (IsKeyDown(KEY_DOWN) && y_axis_display.z < 0.98f) {
			KDTChunkTree_rotateLoaded(&chunkTree, x_axis, rotation_speed);
//			KDTChunk_rotateVis(chunk, x_axis, rotation_speed);
			y_axis_display = Vector3RotateByAxisAngle(y_axis_display, x_axis, rotation_speed);
			z_axis_display = Vector3RotateByAxisAngle(z_axis_display, x_axis, rotation_speed);
			pitch += nrotation_speed;
		}

		Vector3 dtranslate = { 0, 0, 0 };

		if (IsKeyDown(KEY_A)) {
			dtranslate = Vector3Add(dtranslate, Vector3Scale(x_axis, move_speed));
			player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, nmove_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while (KDTChunk_searchRaw(chunk, player_pos) || KDTChunk_searchRaw(chunk, player_view_pos)) {
				dtranslate = Vector3Add(dtranslate, Vector3Scale(x_axis, adjust_speed*nmove_speed));
				player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, adjust_speed*move_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}

		if (IsKeyDown(KEY_D)) {
			dtranslate = Vector3Add(dtranslate, Vector3Scale(x_axis, nmove_speed));
			player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, move_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while(KDTChunk_searchRaw(chunk, player_pos) || KDTChunk_searchRaw(chunk, player_view_pos)) {
				dtranslate = Vector3Add(dtranslate, Vector3Scale(x_axis, adjust_speed*move_speed));
				player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, adjust_speed*nmove_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}


		if (IsKeyDown(KEY_S)) {
			dtranslate = Vector3Add(dtranslate, Vector3Scale(z_axis_display, nmove_speed));
			player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, move_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while(KDTChunk_searchRaw(chunk, player_pos) || KDTChunk_searchRaw(chunk, player_view_pos)) {
				dtranslate = Vector3Add(dtranslate, Vector3Scale(z_axis_display, adjust_speed*move_speed));
				player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, adjust_speed*nmove_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}


		if (IsKeyDown(KEY_W)) {
			dtranslate = Vector3Add(dtranslate, Vector3Scale(z_axis_display, move_speed));
			player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, nmove_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while (KDTChunk_searchRaw(chunk, player_pos) || KDTChunk_searchRaw(chunk, player_view_pos)) {
				dtranslate = Vector3Add(dtranslate, Vector3Scale(z_axis_display, adjust_speed*nmove_speed));
				player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, adjust_speed*move_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}

		// jump
		if (IsKeyDown(KEY_SPACE) && isTouchingFloor) {
			player_velocity.y = jump_speed;
			isTouchingFloor = false;
		}

		// check if no floor beneath player
		Voxel* beneath_player = KDTChunk_searchRaw_Voxel(chunk, Vector3Add(player_pos, (Vector3){0, -0.5f, 0}));
		if (!beneath_player && isTouchingFloor) {
			isTouchingFloor = false;
		}

		if (!isTouchingFloor) {
			Voxel* floor = KDTChunk_searchRaw_Voxel(chunk, (Vector3){player_pos.x, player_pos.y + player_velocity.y, player_pos.z});
			if (!floor)
				floor = KDTChunk_searchRaw_Voxel(chunk, player_pos);
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
			selected = KDTChunk_searchRaw_Voxel(chunk, search_pos);
			if (selected)
				break;
		}

		if (IsKeyPressed(KEY_Q) || player_pos.y < -20) {
			y_axis_display = y_axis;
			z_axis_display = z_axis;
			x_axis_move = x_axis;
			z_axis_move = z_axis;
			yaw = 0.0f;
			player_pos = player_origin;
			player_view_pos = Vector3Add(player_pos, (Vector3){0, 1.6f, 0});
			player_velocity = (Vector3){0, 0, 0};
			KDTChunkTree_resetLoaded(&chunkTree);
//			KDTChunk_resetVis(chunk);
			dtranslate = Vector3Scale(player_view_pos, -1);
		}

		KDTChunkTree_translateLoaded(&chunkTree, dtranslate, 1);
//		KDTChunk_translateVis(chunk, dtranslate, 1);

		BeginDrawing();

			ClearBackground(RAYWHITE);
			KDTChunkTree_drawLoaded(&chunkTree, x_offset, y_offset, scale);
//			KDTChunk_drawVis(chunk, x_offset, y_offset, scale);
			if (selected) Voxel_forceDraw(selected, x_offset, y_offset, scale);

			DrawText(TextFormat("(%f, %f, %f)\n(%f, %f, %f)", player_pos.x, player_pos.y, player_pos.z, player_velocity.x, player_velocity.y, player_velocity.z), 0, 0, 20, BLACK);
			DrawText(TextFormat("isTouchingFloor: %d", isTouchingFloor), 0, 40, 20, BLACK);
			DrawText(TextFormat("chunk: (%f, %f, %f)", chunk ? chunk->origin.x : -1, chunk ? chunk->origin.y : -1, chunk ? chunk->origin.z : -1), 0, 60, 20, BLACK);
			DrawText(TextFormat("yaw: %f\n", yaw), 0, 80, 20, BLACK);

			DrawLineV((Vector2){x_offset - 10, y_offset}, (Vector2){x_offset + 10, y_offset}, BLACK);
			DrawLineV((Vector2){x_offset, y_offset - 10}, (Vector2){x_offset, y_offset + 10}, BLACK);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
