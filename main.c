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

#define HEIGHT 800
#define WIDTH 1200

Vector3 target = {0};
Vector3 origin = {-ORIGIN_OFFSET, -ORIGIN_OFFSET, -ORIGIN_OFFSET};
Vector3 camera_start_pos = {2, 2, 2};
Vector3 camera_start_up = {0, 1, 0};

Vector2 slider_nob_pos = {20, HEIGHT / 2};

// char const *image_path = "./imgs/4140047.png";
char const *image_path = "./imgs/200px-Cave_Entrance.png";
// char const *image_path = "./imgs/Lenna_(test_image).png";

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
    PixelPoint *pp = NULL;
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
        hmputs(pp, pixel);
    }

    return pp;
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


    PixelPoint *pixel_points = make_LOP_list(colors, pixels_length, pixels_length);
    int LOP = hmlen(pixel_points);
    int ww = hmlen(pixel_points);

    printf("pixels: %d, amount_avrg: %d, LOP: %d\n", hmlen(pixel_points), pixels_length / LOP, pixels_length);

    // return 0;
    SetTargetFPS(120);
    InitWindow(WIDTH, HEIGHT, "3D graph - Raylib");
    while (!WindowShouldClose()) {
        // printf("FPS: %d\n", GetFPS());

        float dist = GetMouseWheelMove();
        CameraMoveToTarget(&camera, -dist * ZOOM_SPEED);

        if (LOP <= 0) LOP = 1;
        if (LOP >= pixels_length) LOP = pixels_length;

        pixel_points = make_LOP_list(colors, pixels_length, LOP);
        printf("pixels: %d, amount_avrg: %d, LOP: %d\n", hmlen(pixel_points), pixels_length / LOP, LOP);


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
        for (int i = 0; i < hmlen(pixel_points); i++) {
            Vector3 place = {pixel_points[i].key.r / (float)255, pixel_points[i].key.g / (float)255, pixel_points[i].key.b / (float)255};
            Color color = {pixel_points[i].key.r, pixel_points[i].key.g, pixel_points[i].key.b, 255};
            place_point_on_graph(place, color);
        }
        draw_graph(10);
        EndMode3D();

        DrawRectangle(0, 0, 70, HEIGHT, GetColor(0x404040FF));
        add_slider(ww, &LOP);
        EndDrawing();
    }
    CloseWindow();
}
