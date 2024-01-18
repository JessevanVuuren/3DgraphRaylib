#include <stdio.h>
#include <stdlib.h>

#include "stb_ds.h"

#define STB_DS_IMPLEMENTATION


typedef struct {
    int key;
} Pixel;


int main() {
    Pixel *pixels = NULL;

    

    printf("%d\n", hmlen(pixels));

    return 0;
}