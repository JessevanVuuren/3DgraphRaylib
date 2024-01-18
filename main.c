#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"


#define CUBE_SIZE .5
#define CAMERA_SPEED 0.01
#define ZOOM_SPEED 1
#define SCALE 1
#define ORIGIN_OFFSET ((float)SCALE / 2)
#define POINT_SIZE .005

Vector3 target = {0};
Vector3 origin = {-ORIGIN_OFFSET, -ORIGIN_OFFSET, -ORIGIN_OFFSET};
Vector3 camera_start_pos = {2, 2, 2};
Vector3 camera_start_up = {0, 1, 0};

// char const *image_path = "./imgs/4140047.png";
char const *image_path = "./imgs/Lenna_(test_image).png";
// char const *image_path = "./imgs/Flag_of_the_Netherlands.png";

typedef struct {
    Color key;
} PixelPoint;


void draw_graph(int steps) {
    DrawLine3D(origin, (Vector3){ORIGIN_OFFSET, -ORIGIN_OFFSET, -ORIGIN_OFFSET}, RED);
    DrawLine3D(origin, (Vector3){-ORIGIN_OFFSET, ORIGIN_OFFSET, -ORIGIN_OFFSET}, GREEN);
    DrawLine3D(origin, (Vector3){-ORIGIN_OFFSET, -ORIGIN_OFFSET, ORIGIN_OFFSET}, BLUE);

    float step_size = 1 / (float)(steps + 1);
    for (int i = 0; i < steps + 1; i++) {
        float new_step = -ORIGIN_OFFSET + step_size * (i + 1);
        Color line_color = GRAY;
        if (i == steps) line_color = WHITE;
        DrawLine3D((Vector3){new_step, -ORIGIN_OFFSET, -ORIGIN_OFFSET}, (Vector3){new_step, ORIGIN_OFFSET, -ORIGIN_OFFSET}, line_color);
        DrawLine3D((Vector3){-ORIGIN_OFFSET, new_step, -ORIGIN_OFFSET}, (Vector3){ORIGIN_OFFSET, new_step, -ORIGIN_OFFSET}, line_color);

        DrawLine3D((Vector3){-ORIGIN_OFFSET, -ORIGIN_OFFSET, new_step}, (Vector3){-ORIGIN_OFFSET, ORIGIN_OFFSET, new_step}, line_color);
        DrawLine3D((Vector3){-ORIGIN_OFFSET, new_step, -ORIGIN_OFFSET}, (Vector3){-ORIGIN_OFFSET, new_step, ORIGIN_OFFSET}, line_color);

        DrawLine3D((Vector3){new_step, -ORIGIN_OFFSET, -ORIGIN_OFFSET}, (Vector3){new_step, -ORIGIN_OFFSET, ORIGIN_OFFSET}, line_color);
        DrawLine3D((Vector3){-ORIGIN_OFFSET, -ORIGIN_OFFSET, new_step}, (Vector3){ORIGIN_OFFSET, -ORIGIN_OFFSET, new_step}, line_color);
    }
}

void place_point_on_graph(Vector3 pos, Color color) {
    Vector3 local_pos = Vector3SubtractValue(pos, ORIGIN_OFFSET);
    DrawCube(local_pos, POINT_SIZE, POINT_SIZE, POINT_SIZE, color);
}

PixelPoint *fill_hashmap(Color *colors, int *size) {
    PixelPoint *pp = NULL;
    int n = *size;

    for (int i = 0; i < n; i++) {
        int pixel = hmgeti(pp, colors[i]);
        if (pixel < 0) {
            PixelPoint point = {.key = colors[i]};
            hmputs(pp, point);
        }
    }

    *size = hmlen(pp);
    return pp;
}

PixelPoint *make_LOP_list(Color *colors, int size, int LOP) {
    PixelPoint *LOP_pixels = malloc(LOP * sizeof(PixelPoint));
    int pixels = size / LOP;
    for (int i = 0; i < LOP; i++) {
        float r, g, b = 0;
        for (int j = (i * pixels); j < ((i + 1) * pixels); j++) {
            r += colors[j].r;
            g += colors[j].g;
            b += colors[j].b;
        }

        r = r / (float)pixels;
        g = g / (float)pixels;
        b = b / (float)pixels;

        PixelPoint pixel = {.key = (Color){r, g, b}};
        LOP_pixels[i] = pixel;
    }

    return LOP_pixels;
}

int main() {

    Camera3D camera = {
        .position = camera_start_pos,
        .target = target,
        .up = camera_start_up,
        .fovy = 30,
        .projection = CAMERA_PERSPECTIVE};

    Image img = LoadImage(image_path);
    Color *colors = LoadImageColors(img);
    int pixels_length = img.width * img.height;

    int LOP = 1;

    PixelPoint *pixel_points = make_LOP_list(colors, pixels_length, LOP);
    // for (int i = 0; i < LOP; i++) {
    //     printf("R: %u, G: %u, B: %u\n", pixel_points[i].key.r, pixel_points[i].key.g, pixel_points[i].key.b);
    // }
    // PixelPoint *pixel_points = fill_hashmap(colors, &pixels_length);


    SetTargetFPS(60);
    InitWindow(1200, 800, "3D graph - Raylib");
    while (!WindowShouldClose()) {
        // printf("FPS: %d\n", GetFPS());

        float dist = GetMouseWheelMove();
        CameraMoveToTarget(&camera, -dist * ZOOM_SPEED);
        if (dist != 0.0) {
            LOP += dist;
            if (LOP <= 0) {
                LOP = 0;
            }
            PixelPoint *pixel_points = make_LOP_list(colors, pixels_length, LOP);
        }
        printf("LOP: %d\n", LOP);
        if (IsKeyDown(KEY_W)) {
            LOP += 5;
        }
        if (IsKeyDown(KEY_S)) {
            LOP -= 5;
            if (LOP <= 0) LOP = 0;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouseDelta = GetMouseDelta();
            CameraYaw(&camera, -mouseDelta.x * CAMERA_SPEED, true);
            CameraPitch(&camera, -mouseDelta.y * CAMERA_SPEED, true, true, false);
        }

        if (IsKeyPressed(KEY_R)) {
            camera.position = camera_start_pos;
            camera.up = camera_start_up;
        }

        BeginDrawing();
        BeginMode3D(camera);
        ClearBackground(GetColor(0x181818AA));
        for (int i = 0; i < LOP; i++) {

            Vector3 place = {pixel_points[i].key.r / (float)255, pixel_points[i].key.g / (float)255, pixel_points[i].key.b / (float)255};
            Color color = {pixel_points[i].key.r, pixel_points[i].key.g, pixel_points[i].key.b, 255};
            place_point_on_graph(place, color);
        }
        draw_graph(10);
        EndMode3D();
        EndDrawing();
    }
    free(pixel_points);
    CloseWindow();
}
