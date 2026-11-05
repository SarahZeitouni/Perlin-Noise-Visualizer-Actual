#include "raylib.h"
#include<iostream>
#include<vector>

int TextBoxGenerator(int x, int y, int id) {

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

int CheckBoxGenerator(int x, int y, int id) {

    static int selected = 0;

    Rectangle box = {(float)x, (float)y, 25, 25};
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), box)) {
        if (selected == id) {
            selected = 0;
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
    return selected;
}

    std::vector<int> numbers;


int main(void)
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;


    InitWindow(screenWidth, screenHeight, "My UI");
    SetTargetFPS(60);


    while (!WindowShouldClose())
    {
        
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawLineEx({300, 50}, {300, 1080}, 5.0f, BLACK);
        DrawLineEx({0, 50}, {1920, 50}, 5.0f, BLACK);

        //DrawRectangleLinesEx({50, 120, 200, 35}, 2.0f, BLACK);
        //DrawRectangleLinesEx({50, 220, 200, 35}, 2.0f, BLACK);

        DrawText("Seed", 100, 85, 30, BLACK);
        DrawText("Scale", 100, 185, 30, BLACK);

        bool is2D = CheckBoxGenerator(80, 320,0);
        bool is3D = CheckBoxGenerator(180, 320,1);

        DrawText("2D", 85, 300, 15, BLACK);
        DrawText("3D", 185, 300, 15, BLACK);

    int seed = TextBoxGenerator(50, 120, 0);

    if (seed != -1) {
        numbers.push_back(seed);
    }

    int scale = TextBoxGenerator(50, 220, 1);

    if (scale != -1) {
        numbers.push_back(scale);
    }


        EndDrawing();
    }
    CloseWindow();

    return 0;
}