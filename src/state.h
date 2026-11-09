#pragma once
#include "player.h"
#include "voxel.h"
#include "prchunk.h"
#include "prchunktree.h"
#include "perlinNoiseGen.h"
#include <vector>

enum class GameState{
    SCRNBLANK,
    THREE_D,
    TWO_D
};

typedef struct Sample {
    int name;
    uint8_t *pixels;
} sample;

class State{
public:
    State();

    GameState currentState = GameState::SCRNBLANK;

    //3d init
    Player player;
    PRChunkTree chunkTree;
    Camera3D camera;


    bool three_d_initialized = false;
    Voxel* selectedVoxel = nullptr;
    int x_offset = 0;
    int y_offset = 0;
    float scale = 1.0f;


    std::vector<int> numbers;

    Texture2D worldTexture = { 0 };
    bool worldGenerated = false;

    virtual ~State(){}
    virtual void Opened(int* seed, float* scale2);
    virtual void three_d();
    virtual void two_d();

    static void PRChunkTree_perlin(PRChunkTree* tree, int x0, int z0, int x_size,
        int z_size, float scale, int seed);

    int TextBoxGenerator(int x, int y, int id);
    int CheckBoxGenerator(int x, int y, int id);
    Texture2D show_sample(sample *smp);

    void Update();
};
