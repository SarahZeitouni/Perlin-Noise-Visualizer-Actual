#include "player.h"
#include "state.h"
#include "voxel.h"
#include "arrchunk.h"
#include "chunk2dtree.h"
#include "perlinNoiseGen.h"

#include <string>
#include <vector>

#define PERLIN_WIDTH 	4
#define PERLIN_HEIGHT 	4
#define PERLIN_SCALE	0.25

State::State(){
    x_offset = player.screenWidth  / 2;
    y_offset = player.screenHeight / 2;
    scale = (float)player.screenHeight * 0.4f;

    camera.position   = (Vector3){0.0f, 5.0f, 10.0f};
	camera.target     = (Vector3){0.0f, 5.0f, 0.0f};
	camera.up         = (Vector3){0.0f, 1.0f, 0.0f};
	camera.fovy       = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;
}

void State::Opened(int* seed, float* scale2){

    DrawLineEx({300, 50}, {300, 1080}, 5.0f, BLACK);
    DrawLineEx({0, 50}, {1920, 50}, 5.0f, BLACK);

    DrawText("Seed", 100, 85, 30, BLACK);
    DrawText("Scale", 100, 185, 30, BLACK);

    bool is2D = CheckBoxGenerator(80, 320,0);
    bool is3D = CheckBoxGenerator(180, 320,1);

    DrawText("2D", 85, 300, 15, BLACK);
    DrawText("3D", 185, 300, 15, BLACK);

    int seedIn = TextBoxGenerator(50, 120, 0);

    if (seedIn != -1 && seedIn != player.seed){
        if (worldGenerated) {
            UnloadTexture(worldTexture);
            worldGenerated = false;
        }
        if (three_d_initialized) {
            ChunkKDT_delete(&chunkTree);
            three_d_initialized = false;
        }
        player.seed = seedIn;
    }

    int scaleIn = TextBoxGenerator(50, 220, 1);

    if (scaleIn != -1 && scaleIn != player.scale2) {
        numbers.push_back(scaleIn);
        *scale2 = scaleIn;
    }

    if (is3D && (*seed != 0)){
        currentState = GameState::THREE_D;
        is2D = false;
    }

    else if (is2D && (*seed != 0)){
        currentState = GameState::TWO_D;
        is3D = false;
    }
}

void State::three_d(){

    camera.position = player.player_view_pos;
    camera.target   = (Vector3){
        player.player_view_pos.x,
        player.player_view_pos.y,
        player.player_view_pos.z
    } + player.y_axis_display;

    BeginMode3D(camera);

    if (!three_d_initialized){
        ChunkKDT_init(&chunkTree);

        ChunkKDT_Perlin(&chunkTree, 0, 0, PERLIN_WIDTH, PERLIN_HEIGHT, PERLIN_SCALE, player.seed); //err

        ChunkKDT_setVis(&chunkTree);
        ChunkKDT_translateVis(&chunkTree, player.player_view_pos, -1);

        three_d_initialized = true;
    }

    KDTNode* node = ChunkKDT_find(&chunkTree, player.player_pos);
    ArrChunk* chunk = node ? node->chunk : nullptr;

	SetTargetFPS(60);

	//HideCursor();

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
        ChunkKDT_rotateVis(&chunkTree, player.y_axis_display, player.nrotation_speed);
        player.x_axis_move = Vector3RotateByAxisAngle(player.x_axis_move, player.y_axis, player.rotation_speed);
        player.z_axis_move = Vector3RotateByAxisAngle(player.z_axis_move, player.y_axis, player.rotation_speed);
        player.yaw += player.rotation_speed;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        ChunkKDT_rotateVis(&chunkTree, player.y_axis_display, player.rotation_speed);
        player.x_axis_move = Vector3RotateByAxisAngle(player.x_axis_move, player.y_axis, player.nrotation_speed);
        player.z_axis_move = Vector3RotateByAxisAngle(player.z_axis_move, player.y_axis, player.nrotation_speed);
        player.yaw += player.nrotation_speed;
    }
    if (IsKeyDown(KEY_UP) && player.y_axis_display.z > -0.98f) {
        ChunkKDT_rotateVis(&chunkTree, player.x_axis, player.nrotation_speed);
        player.y_axis_display = Vector3RotateByAxisAngle(player.y_axis_display, player.x_axis, player.nrotation_speed);
        player.z_axis_display = Vector3RotateByAxisAngle(player.z_axis_display, player.x_axis, player.nrotation_speed);
        player.pitch += player.rotation_speed;
    }
    if (IsKeyDown(KEY_DOWN) && player.y_axis_display.z < 0.98f) {
        ChunkKDT_rotateVis(&chunkTree, player.x_axis, player.rotation_speed);
        player.y_axis_display = Vector3RotateByAxisAngle(player.y_axis_display, player.x_axis, player.rotation_speed);
        player.z_axis_display = Vector3RotateByAxisAngle(player.z_axis_display, player.x_axis, player.rotation_speed);
        player.pitch += player.nrotation_speed;
    }

    Vector3 dtranslate = { 0, 0, 0 };

    if (IsKeyDown(KEY_A)) {
//			ChunkKDT_translateVis(&chunkTree, x_axis, move_speed);
        dtranslate = Vector3Add(dtranslate, Vector3Scale(player.x_axis, player.move_speed));
        player.player_pos = Vector3Add(player.player_pos, Vector3Scale(player.x_axis_move, player.nmove_speed));
        player.player_view_pos = (Vector3){ player.player_pos.x, player.player_pos.y + 1.6f, player.player_pos.z };
        while (ArrChunk_find(chunk, player.player_pos) || ArrChunk_find(chunk, player.player_view_pos)) {
//				ChunkKDT_translateVis(&chunkTree, x_axis, adjust_speed*nmove_speed);
            dtranslate = Vector3Add(dtranslate, Vector3Scale(player.x_axis, player.adjust_speed*player.nmove_speed));
            player.player_pos = Vector3Add(player.player_pos, Vector3Scale(player.x_axis_move, player.adjust_speed*player.move_speed));
            player.player_view_pos = (Vector3){ player.player_pos.x, player.player_pos.y + 1.6f, player.player_pos.z };
        }
    }

    if (IsKeyDown(KEY_D)) {
//			ChunkKDT_translateVis(&chunkTree, x_axis, nmove_speed);
        dtranslate = Vector3Add(dtranslate, Vector3Scale(player.x_axis, player.nmove_speed));
        player.player_pos = Vector3Add(player.player_pos, Vector3Scale(player.x_axis_move, player.move_speed));
        player.player_view_pos = (Vector3){ player.player_pos.x, player.player_pos.y + 1.6f, player.player_pos.z };
        while(ArrChunk_find(chunk, player.player_pos) || ArrChunk_find(chunk, player.player_view_pos)) {
//				ChunkKDT_translateVis(&chunkTree, x_axis, adjust_speed*move_speed);
            dtranslate = Vector3Add(dtranslate, Vector3Scale(player.x_axis, player.adjust_speed*player.move_speed));
            player.player_pos = Vector3Add(player.player_pos, Vector3Scale(player.x_axis_move, player.adjust_speed*player.nmove_speed));
            player.player_view_pos = (Vector3){ player.player_pos.x, player.player_pos.y + 1.6f, player.player_pos.z };
        }
    }


    if (IsKeyDown(KEY_S)) {
//			ChunkKDT_translateVis(&chunkTree, z_axis_display, nmove_speed);
        dtranslate = Vector3Add(dtranslate, Vector3Scale(player.z_axis_display, player.nmove_speed));
        player.player_pos = Vector3Add(player.player_pos, Vector3Scale(player.z_axis_move, player.move_speed));
        player.player_view_pos = (Vector3){ player.player_pos.x, player.player_pos.y + 1.6f, player.player_pos.z };
        while(ArrChunk_find(chunk, player.player_pos) || ArrChunk_find(chunk, player.player_view_pos)) {
//				ChunkKDT_translateVis(&chunkTree, z_axis_display, adjust_speed*move_speed);
            dtranslate = Vector3Add(dtranslate, Vector3Scale(player.z_axis_display, player.adjust_speed*player.move_speed));
            player.player_pos = Vector3Add(player.player_pos, Vector3Scale(player.z_axis_move, player.adjust_speed*player.nmove_speed));
            player.player_view_pos = (Vector3){ player.player_pos.x, player.player_pos.y + 1.6f, player.player_pos.z };
        }
    }

    ChunkKDT_drawVis(&chunkTree, x_offset, y_offset, scale);
    if (selectedVoxel) Voxel_forceDraw(selectedVoxel, x_offset, y_offset, scale);
    EndMode3D();
}

void State::two_d(){
    if (!worldGenerated){
	    uint8_t* pixels = Perlin_generate(player.screenWidth, player.screenHeight, player.seed);
        sample smp = { .name = 5, .pixels = pixels };

        worldTexture = show_sample(&smp);

        free(pixels);
        worldGenerated = true;
    }
    DrawTexture(worldTexture, 0, 0, WHITE);
}

Texture2D State::show_sample(sample *smp){
    Image windowImg = {
        .data = smp->pixels,
        .width = player.screenWidth,
        .height = player.screenHeight,
        .mipmaps = 1,
		.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };


    //return this
	Texture2D windowTexture = LoadTextureFromImage(windowImg);
    return windowTexture;
}

void State::Update(){
    BeginScissorMode(300, 50, 1620, 1030);

    switch (currentState){
        case GameState::SCRNBLANK:
            break;
        case GameState::THREE_D:
            three_d();
            break;

        case GameState::TWO_D:
            two_d();
            break;
    }
    EndScissorMode();

    Opened(&player.seed, &player.scale2);
}

void State::ChunkKDT_Perlin(ChunkKDT* tree, int x0, int z0, int x_size, int z_size, float scale, int seed) {
	uint8_t* pixels = Perlin_generate(x_size*16, z_size*16, seed);
	for (int i = 0; i < x_size; i ++) {
		for (int j = 0; j < z_size; j ++) {
			KDTNode* node = ChunkKDT_insertNew(tree, (Vector3){(float)((i + x0)*16), 0.0f, (float)((j + z0)*16)});
			ArrChunk* chunk = node ? node->chunk : nullptr;
			if (!chunk)
				continue;
			for (int x = 0; x < 16; x++) {
				for (int z = 0; z < 16; z ++) {
					int index = (i*16*64 + j*16 + x*64 + z)*4;
					for (int y = 0; y < pixels[index]*scale; y ++)
						ArrChunk_emplaceLocal(chunk, (Vector3){(float)x, (float)y, (float)z});
				}
			}
		}
	}
	free(pixels);
}

int State::TextBoxGenerator(int x, int y, int id) {

    static std::string input[2] = {"", ""};
    static int activeBox = -1;

    Rectangle box = {(float)x, (float)y, 200, 35};
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), box)) {
        activeBox = id;
    }
    if (activeBox == id) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= '0' && key <= '9' && input[id].size() < 10) {
                input[id] += (char)key;
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !input[id].empty()) {
            input[id].pop_back();
        }
        if (IsKeyPressed(KEY_ENTER) && !input[id].empty()) {
            int number = std::stoi(input[id]);
            input[id].clear();
            activeBox = -1;
            return number;
        }
    }
    DrawRectangleRec(box, WHITE);

    DrawRectangleLinesEx(
        box,
        2,
        activeBox == id ? BLUE : BLACK
    );

    DrawText(input[id].c_str(), x + 10, y + 5, 25, BLACK);

    return -1;
}

int State::CheckBoxGenerator(int x, int y, int id) {

    static int selected = -1;

    Rectangle box = {(float)x, (float)y, 25, 25};
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), box)) {
        if (selected == id) {
            selected = -1;
        }
        else {
            selected = id;
        }
    }
    DrawRectangleLinesEx(box, 2, BLACK);
    if (selected == id) {
        DrawLine(x + 5, y + 12, x + 10, y + 18, BLACK);
        DrawLine(x + 10, y + 18, x + 21, y + 5, BLACK);
    }
    return selected == id;
}
