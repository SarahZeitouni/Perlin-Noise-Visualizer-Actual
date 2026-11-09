COMP_FLAGS = -Wall -Wextra
LIB_OPS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUT = ./build/main.o

.PHONY: main
.PHONY: kdt
.PHONY: pr
.PHONY: run
.PHONY: clean

main:
	g++ src/main.c src/kdt*.c src/pr*.c src/voxel.c src/state.cpp src/perlinNoiseGen.c -o $(OUT) $(LIB_OPS) $(COMP_FLAGS)

kdt:
	gcc src/main_kdt.c src/kdt*.c src/voxel.c src/perlinNoiseGen.c -o $(OUT) $(LIB_OPS) $(COMP_FLAGS)

pr:
	gcc src/main_pr.c src/pr*.c src/voxel.c src/perlinNoiseGen.c -o $(OUT) $(LIB_OPS) $(COMP_FLAGS)

run:
	$(OUT)

clean:
	rm -rf ./build/*
