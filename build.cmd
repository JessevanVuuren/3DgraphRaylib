gcc -I./libs/raylib/ -I./libs/stb/ -L./libs/raylib/ -L./libs/stb/ main.c -o out/3dGraph -lraylib -lopengl32 -lgdi32 -lwinmm && .\out\3dGraph.exe