#include <iostream>
#include <raylib.h>
#include <algorithm>
#include "render.h"

const int RES = 1000;
Color *pixels = (Color *)calloc(RES*RES,sizeof(Color)); // 2 for doubling the linear matrix size to avoid segmentation faults

uint hash_func(uint state) {
    state ^= 2747636419u;
    state *= 2654435769u;
    state ^= state >> 16;
    state *= 2654435769u;
    state ^= state >> 16;
    state *= 2654435769u;
    return state;
    // max: 4294967295
}

Color pdata(int x,int y) {
    return pixels[x*RES+y];
}

float hash_scale(uint state) {
    return state / 4294967295.0;
}

void TexPixDraw(int x,int y, Color col) {
    pixels[y*RES+x] = col;
}

void CLS(Color c) {
    for(int i=0; i<RES; ++i) {
        for(int j=0; j<RES; ++j) {
            pixels[i*RES+j] = c;
        }
    }
}

void freeParray(Color* p) {
    free(p);
    p = NULL;
}

void DDTexture(float diffuseWeight, float decayRate) {
    Color newPixels[RES * RES];

    for (int i = 0; i < RES; ++i) {
        for (int j = 0; j < RES; ++j) {


            newPixels[i * RES + j].r = std::max(0.0, pixels[i * RES + j].r * (1.0-diffuseWeight));
            newPixels[i * RES + j].g = std::max(0.0, pixels[i * RES + j].g * (1.0-diffuseWeight));
            newPixels[i * RES + j].b = std::max(0.0, pixels[i * RES + j].b * (1.0-diffuseWeight));

            int av_sum[3] = {0,0,0};

            for (int k = -1; k <= 1; ++k) {
                for (int l = -1; l <= 1; ++l) {
                    int x = i + k;
                    int y = j + l;
                    if (x >= 0 && x < RES && y >= 0 && y < RES) {
                        av_sum[0] += pixels[x * RES + y].r;
                        av_sum[1] += pixels[x * RES + y].g;
                        av_sum[2] += pixels[x * RES + y].b;
                    }
                }
            }
            newPixels[i * RES + j].r = av_sum[0] / 9;
            newPixels[i * RES + j].g = av_sum[1] / 9;
            newPixels[i * RES + j].b = av_sum[2] / 9;
        }
    }

    // Decay the pixel values based on decayRate and Copy the updated pixel values back to the original array
    for (int i = 0; i < RES * RES; ++i) {
        pixels[i].r = std::max(0.0,static_cast<double>(newPixels[i].r-decayRate));
        pixels[i].g = std::max(0.0,static_cast<double>(newPixels[i].g-decayRate));
        pixels[i].b = std::max(0.0,static_cast<double>(newPixels[i].b-decayRate));
    }
}
