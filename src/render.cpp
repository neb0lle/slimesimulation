#include <iostream>
#include <raylib.h>
#include "render.h"

const int RES = 1000;
Color *pixels = (Color *)malloc(RES*RES*sizeof(Color)); // 2 for doubling the linear matrix size to avoid segmentation faults

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

float hash_scale(uint state) {
    return state / 4294967295.0;
}

void TexPixDraw(int x,int y, Color col) {
    pixels[y*RES+x] = col;
}

void CLS() {
    for(int i=0; i<RES; ++i) {
        for(int j=0; j<RES; ++j) {
            pixels[i*RES+j] = BLACK;
        }
    }
}

void DiffuseTexture() {
    float diffstrength = 10;
    for(int i=0; i<RES; ++i) {
        for(int j=0; j<RES; ++j) {
            // diffuse each pixel value intensity each frame (all 3 components)
            pixels[i*RES+j].r -= diffstrength;
            pixels[i*RES+j].g -= diffstrength;
            pixels[i*RES+j].b -= diffstrength;

            // set pixel intensity as average value of neighbouring 3x3 pixel
            int av_sum = 0;
            for(int k=-1; k<2; ++k) {
                for(int l=-1; l<2; ++l) {
                    av_sum += pixels[(i+k)*RES+(j+l)].r;
                }
            }
            av_sum /= 9;
            pixels[i*RES+j].r = av_sum;
            pixels[i*RES+j].g = av_sum;
            pixels[i*RES+j].b = av_sum;

        }
    }
}
