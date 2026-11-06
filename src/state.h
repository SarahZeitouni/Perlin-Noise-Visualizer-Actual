#pragma once
#include "player.h"
#include "voxel.h"
#include "arrchunk.h"
#include "chunk2dtree.h"
#include "perlinNoiseGen.h"
#include <vector>

enum class GameState{
    OPENED,
    THREE_D,
    TWO_D
};

typedef struct Sample {
    int name;
    uint8_t *pixels;
} sample;

class State{
public:
    GameState currentState = GameState::OPENED;

    //3d init
    Player player;
    ChunkKDT chunkTree;

    bool three_d_initialized = false;

     std::vector<int> numbers;

    virtual ~State(){}
    virtual void Opened(int* seed, float* scale2);
    virtual void three_d();
    // virtual void two_d() = 0;

    static void ChunkKDT_Perlin(ChunkKDT* tree, int x0, int z0, int x_size, 
        int z_size, float scale, int seed);
    int TextBoxGenerator(int x, int y, int id);
    int CheckBoxGenerator(int x, int y, int id);
    


    void Update();
};