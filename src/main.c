#include "voxel.h"
#include "arrchunk.h"
#include "chunk2dtree.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include "perlinNoiseGen.h"
#include <stdint.h>

static void ChunkKDT_Perlin(ChunkKDT* tree, int x0, int z0, int x_size, int z_size, float scale) {
	uint8_t* pixels = Perlin_generate(x_size*16, z_size*16);
	for (int i = 0; i < x_size; i ++) {
		for (int j = 0; j < z_size; j ++) {
			KDTNode* node = ChunkKDT_insertNew(tree, (Vector3){(i + x0)*16, 0, (j + z0)*16});
			ArrChunk* chunk = node ? node->chunk : nullptr;
			if (!chunk)
				continue;
			for (int x = 0; x < 16; x++) {
				for (int z = 0; z < 16; z ++) {
					int index = (i*16*64 + j*16 + x*64 + z)*4;
					for (int y = 0; y < pixels[index]*scale; y ++)
						ArrChunk_emplaceLocal(chunk, (Vector3){x, y, z});
				}
			}
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

	ChunkKDT chunkTree;
	ChunkKDT_init(&chunkTree);
	ChunkKDT_Perlin(&chunkTree, -1, -1, 4, 4, 0.25);

//	for (int cx = -1; cx < 2; cx ++) {
//		for (int cz = -1; cz < 2; cz ++) {
//			ArrChunk* chunk = ChunkKDT_insertNew(&chunkTree, (Vector3){ cx*16, 0, cz*16 })->chunk;
//
//			ArrChunk_block(chunk, (Vector3){0, 0, 0}, (Vector3){9, 9, 9});
//
//			for (int i = 0; i < 4; i ++) {
//				if (!ArrChunk_emplaceLocal(chunk, (Vector3){6, 10+4*i, 6}))
//					printf("oopsies\n");
//				ArrChunk_emplaceLocal(chunk, (Vector3){6, 11+4*i, 8});
//				ArrChunk_emplaceLocal(chunk, (Vector3){8, 12+4*i, 8});
//				ArrChunk_emplaceLocal(chunk, (Vector3){8, 13+4*i, 6});
//			}
//
//			ArrChunk_emplaceLocal(chunk, (Vector3){10, 9, 0});
//			ArrChunk_emplaceLocal(chunk, (Vector3){13, 9, 1});
//			ArrChunk_emplaceLocal(chunk, (Vector3){15, 9, 2});
//			ArrChunk_emplaceLocal(chunk, (Vector3){0, 9, 10});
//			ArrChunk_emplaceLocal(chunk, (Vector3){1, 9, 13});
//			ArrChunk_emplaceLocal(chunk, (Vector3){2, 9, 15});
//
//			for (int i = 0; i < 4; i ++) {
//				for (int j = 0; j < 4; j ++) {
//					for (int k = 0; k < 4; k ++)
//						ArrChunk_removeLocal(chunk, (Vector3){2+i, 9-k, 2+j});
//				}
//			}
//
//			ArrChunk_emplaceLocal(chunk, (Vector3){5, 6, 5});
//			ArrChunk_emplaceLocal(chunk, (Vector3){5, 7, 4});
//			ArrChunk_emplaceLocal(chunk, (Vector3){5, 8, 3});
//			ArrChunk_emplaceLocal(chunk, (Vector3){5, 9, 2});
//		}
//	}

	ChunkKDT_setVis(&chunkTree);
	ChunkKDT_translateVis(&chunkTree, player_view_pos, -1);

	InitWindow(screenWidth, screenHeight, "Main");
	SetTargetFPS(60);

	HideCursor();

	while(!WindowShouldClose()) {
		KDTNode* node = ChunkKDT_find(&chunkTree, player_pos);
		if (!node)
			printf("poop\n");
		ArrChunk* chunk = node ? node->chunk : nullptr;

		// load new chunks (got rid of block, fix)

//		for (int i = -1; i < 2; i ++) {
//			for (int j = -1; j < 2; j ++) {
//				Vector3 o = Vector3Add(chunk->origin, (Vector3){16*i, 0, 16*j});
//				if (!ChunkKDT_search(&chunkTree, o)) {
//					ArrChunk* tmp = ChunkKDT_insertNew(&chunkTree, o)->chunk;
//					ArrChunk_block(tmp, (Vector3){0, 0, 0}, (Vector3){15, 9, 15});
//					ArrChunk_setVis(tmp);
//					ArrChunk_translateVis(tmp, player_view_pos, -1);
//					ArrChunk_rotateVis(tmp, x_axis, -1*pitch);
//					ArrChunk_rotateVis(tmp, y_axis_display, -1*yaw);
//				}
//			}
//		}

		if (IsKeyDown(KEY_LEFT)) {
			ChunkKDT_rotateVis(&chunkTree, y_axis_display, nrotation_speed);
			x_axis_move = Vector3RotateByAxisAngle(x_axis_move, y_axis, rotation_speed);
			z_axis_move = Vector3RotateByAxisAngle(z_axis_move, y_axis, rotation_speed);
			yaw += rotation_speed;
		}
		if (IsKeyDown(KEY_RIGHT)) {
			ChunkKDT_rotateVis(&chunkTree, y_axis_display, rotation_speed);
			x_axis_move = Vector3RotateByAxisAngle(x_axis_move, y_axis, nrotation_speed);
			z_axis_move = Vector3RotateByAxisAngle(z_axis_move, y_axis, nrotation_speed);
			yaw += nrotation_speed;
		}
		if (IsKeyDown(KEY_UP) && y_axis_display.z > -0.98f) {
			ChunkKDT_rotateVis(&chunkTree, x_axis, nrotation_speed);
			y_axis_display = Vector3RotateByAxisAngle(y_axis_display, x_axis, nrotation_speed);
			z_axis_display = Vector3RotateByAxisAngle(z_axis_display, x_axis, nrotation_speed);
			pitch += rotation_speed;
		}
		if (IsKeyDown(KEY_DOWN) && y_axis_display.z < 0.98f) {
			ChunkKDT_rotateVis(&chunkTree, x_axis, rotation_speed);
			y_axis_display = Vector3RotateByAxisAngle(y_axis_display, x_axis, rotation_speed);
			z_axis_display = Vector3RotateByAxisAngle(z_axis_display, x_axis, rotation_speed);
			pitch += nrotation_speed;
		}

		Vector3 dtranslate = { 0, 0, 0 };

		if (IsKeyDown(KEY_A)) {
//			ChunkKDT_translateVis(&chunkTree, x_axis, move_speed);
			dtranslate = Vector3Add(dtranslate, Vector3Scale(x_axis, move_speed));
			player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, nmove_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while (ArrChunk_find(chunk, player_pos) || ArrChunk_find(chunk, player_view_pos)) {
//				ChunkKDT_translateVis(&chunkTree, x_axis, adjust_speed*nmove_speed);
				dtranslate = Vector3Add(dtranslate, Vector3Scale(x_axis, adjust_speed*nmove_speed));
				player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, adjust_speed*move_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}

		if (IsKeyDown(KEY_D)) {
//			ChunkKDT_translateVis(&chunkTree, x_axis, nmove_speed);
			dtranslate = Vector3Add(dtranslate, Vector3Scale(x_axis, nmove_speed));
			player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, move_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while(ArrChunk_find(chunk, player_pos) || ArrChunk_find(chunk, player_view_pos)) {
//				ChunkKDT_translateVis(&chunkTree, x_axis, adjust_speed*move_speed);
				dtranslate = Vector3Add(dtranslate, Vector3Scale(x_axis, adjust_speed*move_speed));
				player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, adjust_speed*nmove_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}


		if (IsKeyDown(KEY_S)) {
//			ChunkKDT_translateVis(&chunkTree, z_axis_display, nmove_speed);
			dtranslate = Vector3Add(dtranslate, Vector3Scale(z_axis_display, nmove_speed));
			player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, move_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while(ArrChunk_find(chunk, player_pos) || ArrChunk_find(chunk, player_view_pos)) {
//				ChunkKDT_translateVis(&chunkTree, z_axis_display, adjust_speed*move_speed);
				dtranslate = Vector3Add(dtranslate, Vector3Scale(z_axis_display, adjust_speed*move_speed));
				player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, adjust_speed*nmove_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}


		if (IsKeyDown(KEY_W)) {
//			ChunkKDT_translateVis(&chunkTree, z_axis_display, move_speed);
			dtranslate = Vector3Add(dtranslate, Vector3Scale(z_axis_display, move_speed));
			player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, nmove_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while (ArrChunk_find(chunk, player_pos) || ArrChunk_find(chunk, player_view_pos)) {
//				ChunkKDT_translateVis(&chunkTree, z_axis_display, adjust_speed*nmove_speed);
				dtranslate = Vector3Add(dtranslate, Vector3Scale(z_axis_display, adjust_speed*nmove_speed));
				player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, adjust_speed*move_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}

		// better horizontal collision wip

//		for (Voxel* v0, * v1; (v0 = ArrChunk_find(chunk, player_pos)) || (v1 = ArrChunk_find(chunk, player_view_pos));) {
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
		Voxel* beneath_player = ArrChunk_find(chunk, Vector3Add(player_pos, (Vector3){0, -0.5f, 0}));
		if (!beneath_player && isTouchingFloor) {
			isTouchingFloor = false;
		}

		if (!isTouchingFloor) {
			Voxel* floor = ArrChunk_find(chunk, (Vector3){player_pos.x, player_pos.y + player_velocity.y, player_pos.z});
			if (!floor)
				floor = ArrChunk_find(chunk, player_pos);
			if (floor && player_velocity.y < 0) {
//				ChunkKDT_translateVis(&chunkTree, y_axis_display, player_pos.y - (floor->origin.y + 1));
				dtranslate = Vector3Add(dtranslate, Vector3Scale(y_axis_display, player_pos.y - (floor->origin.y + 1)));
				player_pos.y = floor->origin.y + 1;
				player_view_pos.y = player_pos.y + 1.6;
				player_velocity.y = 0.0f;
				isTouchingFloor = true;
			}
			else {
				player_pos.y += player_velocity.y;
				player_view_pos.y += player_velocity.y;
//				ChunkKDT_translateVis(&chunkTree, y_axis_display, -1*player_velocity.y);
				dtranslate = Vector3Add(dtranslate, Vector3Scale(y_axis_display, -1*player_velocity.y));
				player_velocity.y += gravity;
			}
		}

		// highlight selected voxel
		Voxel* selected = nullptr;
		for (int i = 0; i < 30; i ++) {
			Vector3 search_pos = Vector3RotateByAxisAngle(z_axis_display, y_axis, PI + yaw);
			search_pos = Vector3Add(player_view_pos, Vector3Scale(search_pos, i*0.2f));
			selected = ArrChunk_find(chunk, search_pos);

			if (selected)
				break;
		}

		if (selected && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			ArrChunk_removeVoxel(chunk, selected);

		if (IsKeyPressed(KEY_Q) || player_pos.y < -20) {
			y_axis_display = y_axis;
			z_axis_display = z_axis;
			x_axis_move = x_axis;
			z_axis_move = z_axis;
			yaw = 0.0f;
			player_pos = player_origin;
			player_view_pos = Vector3Add(player_pos, (Vector3){0, 1.6f, 0});
			player_velocity = (Vector3){0, 0, 0};
			ChunkKDT_resetVis(&chunkTree);
			dtranslate = Vector3Scale(player_view_pos, -1);
//			ChunkKDT_translateVis(&chunkTree, player_view_pos, -1);
		}

		ChunkKDT_translateVis(&chunkTree, dtranslate, 1);

		BeginDrawing();

			ClearBackground(RAYWHITE);
			ChunkKDT_drawVis(&chunkTree, x_offset, y_offset, scale);
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
