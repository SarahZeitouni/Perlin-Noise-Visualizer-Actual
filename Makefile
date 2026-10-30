COMPILER = cc
COMPILER_FLAGS = -Wall -Wextra
LIB_OPS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUT = ./build/main.o
SRCFILES = src/main.c src/voxel.c src/voxel.h src/arrchunk.c src/arrchunk.h src/mergesort.h

.PHONY: build
.PHONY: run
.PHONY: clean

build:
	$(COMPILER) $(SRCFILES) -o $(OUT) $(LIB_OPS) $(COMPILER_FLAGS)

run:
	$(OUT)

clean:
	rm -rf ./build/*

