#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

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
// char const *image_path = "./imgs/Lenna_(test_image).png";
char const *image_path = "./imgs/Flag_of_the_Netherlands.png";

typedef struct{
    float r;
    float g;
    float b;
}PixelPoint;


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


PixelPoint *normalize_img_colors(Color *colors, int size) {
    PixelPoint *pixel_points = malloc(size * sizeof(PixelPoint));
    int max_color_size = 255;

    for (int i = 0; i < size; i++) {
        pixel_points[i].r = colors[i].r / (float)max_color_size;
        pixel_points[i].g = colors[i].g / (float)max_color_size;
        pixel_points[i].b = colors[i].b / (float)max_color_size;
    }
    
    return pixel_points;
}

void remove_double(PixelPoint *pixels, int *size) {
    int n = *size;

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (pixels[i].r == pixels[j].r && pixels[i].g == pixels[j].g && pixels[i].b == pixels[j].b) {
                for (int l = j; l < n; l++) {
                    pixels[l] = pixels[l + 1];
                }

                n--;
                j--;
            }
        }
    }

    *size = n;
}

int main() {
    InitWindow(1200, 800, "Hello world");
    SetTargetFPS(60);

    Camera3D camera = {
        .position = camera_start_pos,
        .target = target,
        .up = camera_start_up,
        .fovy = 30,
        .projection = CAMERA_PERSPECTIVE};

    Image img = LoadImage(image_path);
    Color *colors = LoadImageColors(img);
    int pixels_length = img.width * img.height;
    PixelPoint *pixel_points = normalize_img_colors(colors, pixels_length);

    printf("Original: %d\n", pixels_length);
    remove_double(pixel_points, &pixels_length);
    printf("Compressed: %d\n", pixels_length);

    while (!WindowShouldClose()) {
        printf("FPS: %d\n", GetFPS());

        float dist = GetMouseWheelMove();
        CameraMoveToTarget(&camera, -dist * ZOOM_SPEED);


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
                for (int i = 0; i < pixels_length; i++) {
                    Vector3 place = {pixel_points[i].r, pixel_points[i].g, pixel_points[i].b};
                    Color color = {
                        (unsigned char)(pixel_points[i].r * 255), 
                        (unsigned char)(pixel_points[i].g * 255), 
                        (unsigned char)(pixel_points[i].b * 255), 255};
                    place_point_on_graph(place, color);
                }
                draw_graph(10);
            EndMode3D();
        EndDrawing();
    }
    free(pixel_points);
    CloseWindow();
}
