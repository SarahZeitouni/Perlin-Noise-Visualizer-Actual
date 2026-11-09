COMP_FLAGS = -Wall -Wextra
LIB_OPS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUT = ./build/main.o

.PHONY: main
.PHONY: kdt
.PHONY: pr
.PHONY: run
.PHONY: clean

main:
	g++ src/main.c src/kdt*.c src/kdt*.h src/pr*.c src/pr*.h src/voxel.c src/voxel.h src/player.h src/state.cpp src/state.h src/perlinNoiseGen.c src/perlinNoiseGen.h -o $(OUT) $(LIB_OPS) $(COMP_FLAGS)

kdt:
	gcc src/main_kdt.c src/kdt*.c src/kdt*.h src/voxel.* src/perlinNoiseGen.* -o $(OUT) $(LIB_OPS) $(COMP_FLAGS)

pr:
	gcc src/main_pr.c src/pr*.c src/pr*.h src/voxel.* src/perlinNoiseGen.* -o $(OUT) $(LIB_OPS) $(COMP_FLAGS)

run:
	$(OUT)

clean:
	rm -rf ./build/*
