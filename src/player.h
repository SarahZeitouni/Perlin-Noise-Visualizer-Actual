#pragma once
#include <raylib.h>
#include <raymath.h>

class Player{
    public:
    // Coordinate axes
    const Vector3 x_axis = { 1.0f, 0.0f, 0.0f };
    const Vector3 y_axis = { 0.0f, 1.0f, 0.0f };
    const Vector3 z_axis = { 0.0f, 0.0f, 1.0f };

    Vector3 x_axis_move = x_axis;
    Vector3 z_axis_move = z_axis;
    Vector3 y_axis_display = y_axis;
    Vector3 z_axis_display = z_axis;

    float yaw = 0.0f;
    float pitch = 0.0f;

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
    float nmove_speed = -1 * move_speed;

    float adjust_speed = 0.01f;
    float jump_speed = 0.19f;
    float gravity = -0.015f;

    int screenWidth = 1080;
    int screenHeight = 800;
    
	int seed;
    float scale2;
};