COMPILER = g++
SRC = src/*
OUT = ./build/main.o
LIB_PATHS = -L/c/raylib/src
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
INC_PATHS = -I/c/raylib/src


.PHONY: build

build:
	$(COMPILER) $(SRC) -o $(OUT) $(LIB_PATHS) $(LIBS) $(INC_PATHS)

run:
	$(OUT)