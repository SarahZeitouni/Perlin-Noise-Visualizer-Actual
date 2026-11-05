INCLUDE = -I /opt/homebrew/opt/raylib/include/
SOURCE_LIBS = -L /opt/homebrew/opt/raylib/lib/
OSX_OPT = -lraylib -L /opt/homebrew/opt/raylib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL /opt/homebrew/opt/raylib/lib/libraylib.a
src_files = main3.cpp
out_file = out.exe

.PHONY build:
	clang++ $(src_files) $(INCLUDE) $(SOURCE_LIBS) $(OSX_OPT) -o out.exe

.PHONY run:
	./$(out_file)