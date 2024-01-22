#include <math.h>
#include <stdbool.h>
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
#define AXIS_DETAIL 10

#define HEIGHT 800
#define WIDTH 1200

Vector3 target = {0};
Vector3 origin = {-ORIGIN_OFFSET, -ORIGIN_OFFSET, -ORIGIN_OFFSET};
Vector3 camera_start_pos = {2, 2, 2};
Vector3 camera_start_up = {0, 1, 0};

Vector2 slider_nob_pos = {20, HEIGHT / 2};

bool image_supplied = false;

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

PixelPoint *make_LOP_list(Color *colors, int size, int LOP) {
    PixelPoint *PP = NULL;
    int step = size / LOP;
    for (int i = 0; i < LOP; i++) {
        float r = 0, g = 0, b = 0;
        for (int j = i * step; j < (i + 1) * step; j++) {
            r += colors[j].r;
            g += colors[j].g;
            b += colors[j].b;
        }
        r = r / step;
        g = g / step;
        b = b / step;

        PixelPoint pixel = {.key = (Color){r, g, b}};
        hmputs(PP, pixel);
    }

    return PP;
}


void add_slider(int max, int *LOP) {
    int y = HEIGHT - slider_nob_pos.y - 50;
    float dist = y / (float)(HEIGHT - 65);
    *LOP = dist * max;

    DrawRectangle(30, 30, 10, HEIGHT - 60, WHITE);
    DrawRectangle(slider_nob_pos.x, slider_nob_pos.y, 30, 30, GetColor(0xffdd33FF));
    Vector2 mouse_pos = GetMousePosition();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (mouse_pos.x < slider_nob_pos.x + 70) {
            if (mouse_pos.y > 15 && mouse_pos.y < HEIGHT - 45) {

                slider_nob_pos.y = mouse_pos.y;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    PixelPoint *pixel_points = NULL;
    Color *colors;
    int pixels_length = 0;

    if (argc < 2) {
        printf("No Image supplied, display default graph");
    } else if (argc > 2) {
        printf("To many arguments");
        return 0;
    } else {
        const char *image_path = argv[1];
        Image img = LoadImage(image_path);
        colors = LoadImageColors(img);
        
        pixels_length = img.width * img.height;
        pixel_points = make_LOP_list(colors, pixels_length, pixels_length);
    }

    int LOP = hmlen(pixel_points);
    int max_pixels = hmlen(pixel_points);


    Camera3D camera = {
        .position = camera_start_pos,
        .target = target,
        .up = camera_start_up,
        .fovy = 30,
        .projection = CAMERA_PERSPECTIVE};

    SetTargetFPS(120);
    InitWindow(WIDTH, HEIGHT, "3D graph - Raylib");
    while (!WindowShouldClose()) {

        float dist = GetMouseWheelMove();
        CameraMoveToTarget(&camera, -dist * ZOOM_SPEED);

        if (LOP <= 0) LOP = 1;
        if (LOP >= pixels_length) LOP = pixels_length;

        if (argc == 2) pixel_points = make_LOP_list(colors, pixels_length, LOP);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouseDelta = GetMouseDelta();
            Vector2 mousePos = GetMousePosition();
            if (mousePos.x > 70) {
                CameraYaw(&camera, -mouseDelta.x * CAMERA_SPEED, true);
                CameraPitch(&camera, -mouseDelta.y * CAMERA_SPEED, true, true, false);
            }
        }

        if (IsKeyPressed(KEY_R)) {
            camera.position = camera_start_pos;
            camera.up = camera_start_up;
        }

        BeginDrawing();
        BeginMode3D(camera);
        ClearBackground(GetColor(0x181818AA));
        if (argc == 2) {
            for (int i = 0; i < hmlen(pixel_points); i++) {
                Vector3 place = {pixel_points[i].key.r / (float)255, pixel_points[i].key.g / (float)255, pixel_points[i].key.b / (float)255};
                Color color = {pixel_points[i].key.r, pixel_points[i].key.g, pixel_points[i].key.b, 255};
                place_point_on_graph(place, color);
            }
        }
        draw_graph(AXIS_DETAIL);
        EndMode3D();
        DrawRectangle(0, 0, 70, HEIGHT, GetColor(0x404040FF));
        add_slider(max_pixels, &LOP);
        EndDrawing();
    }
    CloseWindow();
}
