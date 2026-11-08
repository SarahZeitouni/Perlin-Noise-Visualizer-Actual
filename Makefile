COMP_FLAGS = -Wall -Wextra
LIB_OPS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUT = ./build/main.o

.PHONY: arr
.PHONY: kdt
.PHONY: pr
.PHONY: run
.PHONY: clean

arr:
	gcc src/main.c src/voxel.* src/arrchunk.* src/chunk2dtree.*	src/perlinNoiseGen.h -o $(OUT) $(LIB_OPS) $(COMP_FLAGS)

kdt:
	gcc src/kdt* src/voxel.* src/perlinNoiseGen.* -o $(OUT) $(LIB_OPS) $(COMP_FLAGS)

pr:
	gcc src/pr* src/voxel.* src/perlinNoiseGen.* -o $(OUT) $(LIB_OPS) $(COMP_FLAGS)

run:
	$(OUT)

clean:
	rm -rf ./build/*
