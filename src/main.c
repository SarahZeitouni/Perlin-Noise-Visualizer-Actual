#include "voxel.h"
#include "raylib.h"
#include "raymath.h"
#include "math.h"
#include "stdio.h"

void ChunkDisplay_getNodeCountHelper(VoxelNode* node, int* count) {
	if (!node) return;

	(*count) ++;
	ChunkDisplay_getNodeCountHelper(node->left, count);
	ChunkDisplay_getNodeCountHelper(node->right, count);
}

int ChunkDisplay_getNodeCount(ChunkDisplay* display) {
	int count = 0;
	if (display) ChunkDisplay_getNodeCountHelper(display->root, &count);
	return count;
}

int main(void) {
	const Vector3 x_axis = { 1.0f, 0.0f, 0.0f };
	const Vector3 y_axis = { 0.0f, 1.0f, 0.0f };
	const Vector3 z_axis = { 0.0f, 0.0f, 1.0f };
	Vector3 x_axis_move = x_axis;
	Vector3 z_axis_move = z_axis;
	Vector3 y_axis_display = y_axis;
	Vector3 z_axis_display = z_axis;
	float yaw = 0.0f;

	const int screenWidth = 800;
	const int screenHeight = 450;

	const float x_offset = (float)screenWidth / 2;
	const float y_offset = (float)screenHeight / 2;
	const float scale = (float)screenHeight * 0.4f;

//	const float sens = 0.2f;

	Vector3 player_origin = { 4.0f, 20.0f, 4.0f };
	Vector3 player_pos = player_origin;
	Vector3 player_view_pos = Vector3Add(player_pos, (Vector3){0, 1.6, 0});
	Vector3 player_velocity = { 0.0f, 0.0f, 0.0f };
	bool isTouchingFloor = false;

	float rotation_speed = 0.05f;
	float nrotation_speed = -0.05f;
	float move_speed = 0.1f;
	float nmove_speed = -1*move_speed;
	float jump_speed = 0.25f;
	float gravity = -0.015f;

	Chunk chunk;
	Chunk_init(&chunk, (Vector3){0, 0, 0});

	for (int a = 0; a < 5; a ++) {
		for (int b = 0; b < 5; b ++) {
			for (int i = 0; i < 16; i ++) {
				for (int j = 0; j < 16; j ++) {
					for (int k = 0; k < 16; k ++) {
						Chunk_insert(&chunk, (Vector3){i + 16*a, -1*k, j + 16*b});
					}
				}
			}
		}
	}

	// blocks
	Chunk_insert(&chunk, (Vector3){-2, 0, -2});
	Chunk_insert(&chunk, (Vector3){-2, 0, -4});
	Chunk_insert(&chunk, (Vector3){-2, 0, -6});
	Chunk_insert(&chunk, (Vector3){-2, 0, -8});
	Chunk_insert(&chunk, (Vector3){-4, 1, -8});
	Chunk_insert(&chunk, (Vector3){-4, 2, -6});
	Chunk_insert(&chunk, (Vector3){-4, 3, -4});
	Chunk_insert(&chunk, (Vector3){-4, 4, -2});
	Chunk_insert(&chunk, (Vector3){-4, 4, 1});
	Chunk_insert(&chunk, (Vector3){-4, 4, 4});
	Chunk_insert(&chunk, (Vector3){-4, 4, 8});

	for (int i = 0; i < 40; i ++) {
		for (int j = i; j < 80 - i; j ++) {
			for (int k = i; k < 80 - i; k ++) {
				Chunk_insert(&chunk, (Vector3){j, i, k});
			}
		}
	}


	ChunkDisplay_init(chunk.display, &chunk);

	Chunk_translateAll(&chunk, x_axis, -1*player_pos.x);
	Chunk_translateAll(&chunk, y_axis, player_view_pos.y);
	Chunk_translateAll(&chunk, z_axis, -1*player_pos.z);

	InitWindow(screenWidth, screenHeight, "blah blah blah");
	SetTargetFPS(60);

	while(!WindowShouldClose()) {
		if (IsKeyDown(KEY_LEFT)) {
			Chunk_rotateAll(&chunk, y_axis_display, rotation_speed);
			x_axis_move = Vector3RotateByAxisAngle(x_axis_move, y_axis, nrotation_speed);
			z_axis_move = Vector3RotateByAxisAngle(z_axis_move, y_axis, nrotation_speed);
			yaw += nrotation_speed;
		}

		if (IsKeyDown(KEY_RIGHT)) {
			Chunk_rotateAll(&chunk, y_axis_display, nrotation_speed);
			x_axis_move = Vector3RotateByAxisAngle(x_axis_move, y_axis, rotation_speed);
			z_axis_move = Vector3RotateByAxisAngle(z_axis_move, y_axis, rotation_speed);
			yaw += rotation_speed;
		}

		if (IsKeyDown(KEY_UP) && y_axis_display.z > -0.98f) {
			Chunk_rotateAll(&chunk, x_axis, nrotation_speed);
			y_axis_display = Vector3RotateByAxisAngle(y_axis_display, x_axis, nrotation_speed);
			z_axis_display = Vector3RotateByAxisAngle(z_axis_display, x_axis, nrotation_speed);
		}

		if (IsKeyDown(KEY_DOWN) && y_axis_display.z < 0.98f) {
			Chunk_rotateAll(&chunk, x_axis, rotation_speed);
			y_axis_display = Vector3RotateByAxisAngle(y_axis_display, x_axis, rotation_speed);
			z_axis_display = Vector3RotateByAxisAngle(z_axis_display, x_axis, rotation_speed);
		}

		if (IsKeyDown(KEY_A)) {
			Chunk_translateAll(&chunk, x_axis, move_speed);
			player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, nmove_speed));
			player_view_pos = Vector3Add(player_view_pos, Vector3Scale(x_axis_move, nmove_speed));
		}


		if (IsKeyDown(KEY_D)) {
			Chunk_translateAll(&chunk, x_axis, nmove_speed);
			player_pos = Vector3Add(player_pos, Vector3Scale(x_axis_move, move_speed));
			player_view_pos = Vector3Add(player_view_pos, Vector3Scale(x_axis_move, move_speed));
		}


		if (IsKeyDown(KEY_S)) {
			Chunk_translateAll(&chunk, z_axis_display, move_speed);
			player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, nmove_speed));
			player_view_pos = Vector3Add(player_view_pos, Vector3Scale(z_axis_move, nmove_speed));
		}


		if (IsKeyDown(KEY_W)) {
			Chunk_translateAll(&chunk, z_axis_display, nmove_speed);
			player_pos = Vector3Add(player_pos, Vector3Scale(z_axis_move, move_speed));
			player_view_pos = Vector3Add(player_view_pos, Vector3Scale(z_axis_move, move_speed));
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
			Chunk_resetAll(&chunk);
			Chunk_translateAll(&chunk, x_axis, -1*player_pos.x);
			Chunk_translateAll(&chunk, y_axis, player_view_pos.y);
			Chunk_translateAll(&chunk, z_axis, -1*player_pos.z);
		}

		// jump!
		Voxel* beneath_player = Chunk_searchVoxel(&chunk, Vector3Add(Voxel_getOrigin(player_pos), (Vector3){0, -1, 0}));
		if (!beneath_player && isTouchingFloor) {
			isTouchingFloor = false;
		}

		if (!isTouchingFloor) {
			player_pos.y += player_velocity.y;
			player_view_pos.y += player_velocity.y;
			Chunk_translateAll(&chunk, y_axis_display, player_velocity.y);
			player_velocity.y += gravity;
		}

		if (IsKeyDown(KEY_SPACE) && isTouchingFloor) {
			player_velocity.y = jump_speed;
			isTouchingFloor = false;
		}

		Voxel* floor_collision = Chunk_searchVoxel(&chunk, Voxel_getOrigin(player_pos));
		if (floor_collision && !isTouchingFloor && player_velocity.y < 0) {
			Chunk_translateAll(&chunk, y_axis_display, (floor_collision->origin.y + 1 - player_pos.y));
			player_pos.y = floor_collision->origin.y + 1;
			player_view_pos.y = player_pos.y + 1.6;
			player_velocity.y = 0.0f;
			isTouchingFloor = true;
		}

		// highlight selected voxel
		Voxel* selected = nullptr;
		for (int i = 0; i < 30; i ++) {
			Vector3 search_pos = Vector3RotateByAxisAngle(z_axis_display, y_axis, yaw);
			search_pos = Vector3Add(player_view_pos, Vector3Scale(search_pos, i*0.2f));
			selected = Chunk_searchVoxel(&chunk, Voxel_getOrigin(search_pos));

			if (selected) {
				break;
			}
		}

		BeginDrawing();

			ClearBackground(RAYWHITE);
			Chunk_drawAll(&chunk, x_offset, y_offset, scale);
			if (selected) Voxel_forceDrawDisplay(selected, x_offset, y_offset, scale);

			DrawText(TextFormat("(%f, %f, %f)\n(%f, %f, %f)", player_pos.x, player_pos.y, player_pos.z, player_velocity.x, player_velocity.y, player_velocity.z), 0, 0, 20, BLACK);
			DrawText(TextFormat("isTouchingFloor: %d", isTouchingFloor), 0, 40, 20, BLACK);
			DrawText(TextFormat("z_axis_display.y: %f", z_axis_display.y), 0, 60, 20, BLACK);

			DrawLineV((Vector2){x_offset - 10, y_offset}, (Vector2){x_offset + 10, y_offset}, BLACK);
			DrawLineV((Vector2){x_offset, y_offset - 10}, (Vector2){x_offset, y_offset + 10}, BLACK);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
