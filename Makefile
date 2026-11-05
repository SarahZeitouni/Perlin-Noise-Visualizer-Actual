COMPILER = gcc
COMP_FLAGS = -Wall -Wextra
LIB_OPS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUT = ./build/main.o
SRCFILES = src/*

.PHONY: build
.PHONY: run
.PHONY: clean

build:
	$(COMPILER) $(SRCFILES) -o $(OUT) $(LIB_OPS) $(COMP_FLAGS)

run:
	$(OUT)

clean:
	rm -rf ./build/*

