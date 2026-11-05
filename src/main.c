#include "voxel.h"
#include "arrchunk.h"
#include "chunk2dtree.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

int main(void) {
	const Vector3 x_axis = { 1.0f, 0.0f, 0.0f };
	const Vector3 y_axis = { 0.0f, 1.0f, 0.0f };
	const Vector3 z_axis = { 0.0f, 0.0f, 1.0f };
	Vector3 x_axis_move = x_axis;
	Vector3 z_axis_move = z_axis;
	Vector3 y_axis_display = y_axis;
	Vector3 z_axis_display = z_axis;
	float yaw = 0.0f;

	const int screenWidth = 1600;
	const int screenHeight = 900;

	const float x_offset = (float)screenWidth / 2;
	const float y_offset = (float)screenHeight / 2;
	const float scale = (float)screenHeight * 0.4f;

	Vector3 player_origin = { 4.0f, 11.0f, 4.0f };
	Vector3 player_pos = player_origin;
	Vector3 player_view_pos = Vector3Add(player_pos, (Vector3){0, 1.6f, 0});
	Vector3 player_velocity = { 0.0f, 0.0f, 0.0f };
	bool isTouchingFloor = false;

	float rotation_speed = 0.05f;
	float nrotation_speed = -0.05f;
	float move_speed = 0.1f;
	float nmove_speed = -1*move_speed;
	float adjust_speed = -0.01f;
	float jump_speed = 0.19f;
	float gravity = -0.015f;

	ChunkKDT chunkTree;
	ChunkKDT_init(&chunkTree);

	ArrChunk chunk;
	ArrChunk_init(&chunk, (Vector3){0, 0, 0});

	for (int x = 0; x < 10; x ++) {
		for (int y = 0; y < 10; y ++) {
			for (int z = 0; z < 10; z ++) {
				ArrChunk_emplace(&chunk, (Vector3){x, y, z});
			}
		}
	}

	for (int i = 0; i < 10; i ++) {
		ArrChunk_emplace(&chunk, (Vector3){6, 10+4*i, 6});
		ArrChunk_emplace(&chunk, (Vector3){6, 11+4*i, 8});
		ArrChunk_emplace(&chunk, (Vector3){8, 12+4*i, 8});
		ArrChunk_emplace(&chunk, (Vector3){8, 13+4*i, 6});
	}

	ArrChunk_emplace(&chunk, (Vector3){10, 9, 0});
	ArrChunk_emplace(&chunk, (Vector3){13, 9, 1});
	ArrChunk_emplace(&chunk, (Vector3){0, 9, 10});
	ArrChunk_emplace(&chunk, (Vector3){1, 9, 13});

	ArrChunk_setVis(&chunk);
	ArrChunk_translateVis(&chunk, player_view_pos, -1);

	InitWindow(screenWidth, screenHeight, "Main");
	SetTargetFPS(60);

	while(!WindowShouldClose()) {
		Vector3 dv = { 0.0f, 0.0f, 0.0f };

		if (IsKeyDown(KEY_LEFT)) {
			ArrChunk_rotateVis(&chunk, y_axis_display, nrotation_speed);
			x_axis_move = Vector3RotateByAxisAngle(x_axis_move, y_axis, rotation_speed);
			z_axis_move = Vector3RotateByAxisAngle(z_axis_move, y_axis, rotation_speed);
			yaw += rotation_speed;
		}
		if (IsKeyDown(KEY_RIGHT)) {
			ArrChunk_rotateVis(&chunk, y_axis_display, rotation_speed);
			x_axis_move = Vector3RotateByAxisAngle(x_axis_move, y_axis, nrotation_speed);
			z_axis_move = Vector3RotateByAxisAngle(z_axis_move, y_axis, nrotation_speed);
			yaw += nrotation_speed;
		}
		if (IsKeyDown(KEY_UP) && y_axis_display.z > -0.98f) {
			ArrChunk_rotateVis(&chunk, x_axis, nrotation_speed);
			y_axis_display = Vector3RotateByAxisAngle(y_axis_display, x_axis, nrotation_speed);
			z_axis_display = Vector3RotateByAxisAngle(z_axis_display, x_axis, nrotation_speed);
		}
		if (IsKeyDown(KEY_DOWN) && y_axis_display.z < 0.98f) {
			ArrChunk_rotateVis(&chunk, x_axis, rotation_speed);
			y_axis_display = Vector3RotateByAxisAngle(y_axis_display, x_axis, rotation_speed);
			z_axis_display = Vector3RotateByAxisAngle(z_axis_display, x_axis, rotation_speed);
		}

		if (IsKeyDown(KEY_A)) {
			ArrChunk_translateVis(&chunk, x_axis, move_speed);
			player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, nmove_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while (ArrChunk_find(&chunk, player_pos) || ArrChunk_find(&chunk, player_view_pos)) {
				ArrChunk_translateVis(&chunk, x_axis, adjust_speed*move_speed);
				player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, adjust_speed*nmove_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}

		if (IsKeyDown(KEY_D)) {
			ArrChunk_translateVis(&chunk, x_axis, nmove_speed);
			player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, move_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while(ArrChunk_find(&chunk, player_pos) || ArrChunk_find(&chunk, player_view_pos)) {
				ArrChunk_translateVis(&chunk, x_axis, adjust_speed*nmove_speed);
				player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, adjust_speed*move_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}


		if (IsKeyDown(KEY_S)) {
			ArrChunk_translateVis(&chunk, z_axis_display, nmove_speed);
			player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, move_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while(ArrChunk_find(&chunk, player_pos) || ArrChunk_find(&chunk, player_view_pos)) {
				ArrChunk_translateVis(&chunk, z_axis_display, adjust_speed*nmove_speed);
				player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, adjust_speed*move_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
		}


		if (IsKeyDown(KEY_W)) {
			ArrChunk_translateVis(&chunk, z_axis_display, move_speed);
			player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, nmove_speed));
			player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			while (ArrChunk_find(&chunk, player_pos) || ArrChunk_find(&chunk, player_view_pos)) {
				ArrChunk_translateVis(&chunk, z_axis_display, adjust_speed*move_speed);
				player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, adjust_speed*nmove_speed));
				player_view_pos = (Vector3){ player_pos.x, player_pos.y + 1.6, player_pos.z };
			}
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
			ArrChunk_resetVis(&chunk);
			ArrChunk_translateVis(&chunk, x_axis, -1*player_view_pos.x);
			ArrChunk_translateVis(&chunk, y_axis, -1*player_view_pos.y);
			ArrChunk_translateVis(&chunk, z_axis, -1*player_view_pos.z);
		}

		// jump
		if (IsKeyDown(KEY_SPACE) && isTouchingFloor) {
			player_velocity.y = jump_speed;
			isTouchingFloor = false;
		}

		// check if no floor beneath player
		Voxel* beneath_player = ArrChunk_find(&chunk, Vector3Add(player_pos, (Vector3){0, -0.5f, 0}));
		if (!beneath_player && isTouchingFloor) {
			isTouchingFloor = false;
		}

		if (!isTouchingFloor) {
			Voxel* floor = ArrChunk_find(&chunk, (Vector3){player_pos.x, player_pos.y + player_velocity.y, player_pos.z});
			if (!floor)
				floor = ArrChunk_find(&chunk, player_pos);
			if (floor && player_velocity.y < 0) {
				ArrChunk_translateVis(&chunk, y_axis_display, player_pos.y - (floor->origin.y + 1));
				player_pos.y = floor->origin.y + 1;
				player_view_pos.y = player_pos.y + 1.6;
				player_velocity.y = 0.0f;
				isTouchingFloor = true;
			}
			else {
				player_pos.y += player_velocity.y;
				player_view_pos.y += player_velocity.y;
				ArrChunk_translateVis(&chunk, y_axis_display, -1*player_velocity.y);
				player_velocity.y += gravity;
			}
		}

//		Voxel* floor_collision = ArrChunk_find(&chunk, player_pos);
//		if (floor_collision && !isTouchingFloor && player_velocity.y < 0) {
//			ArrChunk_translateVis(&chunk, y_axis_display, -1*(floor_collision->origin.y + 1 - player_pos.y));
//			player_pos.y = floor_collision->origin.y + 1;
//			player_view_pos.y = player_pos.y + 1.6;
//			player_velocity.y = 0.0f;
//			isTouchingFloor = true;
//		}

		// highlight selected voxel
		Voxel* selected = nullptr;
		for (int i = 0; i < 30; i ++) {
			Vector3 search_pos = Vector3RotateByAxisAngle(z_axis_display, y_axis, PI + yaw);
			search_pos = Vector3Add(player_view_pos, Vector3Scale(search_pos, i*0.2f));
			selected = ArrChunk_find(&chunk, search_pos);

			if (selected)
				break;
		}

		BeginDrawing();

			ClearBackground(RAYWHITE);
			ArrChunk_drawVis(&chunk, x_offset, y_offset, scale);
			if (selected) Voxel_forceDraw(selected, x_offset, y_offset, scale);

			DrawText(TextFormat("(%f, %f, %f)\n(%f, %f, %f)", player_pos.x, player_pos.y, player_pos.z, player_velocity.x, player_velocity.y, player_velocity.z), 0, 0, 20, BLACK);
			DrawText(TextFormat("isTouchingFloor: %d", isTouchingFloor), 0, 40, 20, BLACK);
			DrawText(TextFormat("y_axis_display.z: %f", y_axis_display.z), 0, 60, 20, BLACK);

			DrawLineV((Vector2){x_offset - 10, y_offset}, (Vector2){x_offset + 10, y_offset}, BLACK);
			DrawLineV((Vector2){x_offset, y_offset - 10}, (Vector2){x_offset, y_offset + 10}, BLACK);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
