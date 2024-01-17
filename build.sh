x86_64-w64-mingw32-g++ -I./raylib/ -L./raylib/ main.c -o 3dGraph -lraylib -lopengl32 -lgdi32 -lwinmm \
    && ./3dGraph.exe

# x86_64-w64-mingw32-gcc c
# x86_64-w64-mingw32-g++ c++