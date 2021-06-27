#include <raylib.h>
#include <iostream>
#include <cmath>
using namespace std;

#define PI 3.1415926535897932384626433832795

struct Agent{
    Vector2 position;
    Vector2 angle;
};

uint hash_func(uint state){
    state ^= 2747636419u;
    state *= 2654435769u;
    state ^= state >> 16;
    state *= 2654435769u;
    state ^= state >> 16;
    state *= 2654435769u;
    return state;
    // max: 4294967295
}

void Update(Agent &agent){ 
    agent.position.x += agent.angle.x;
    agent.position.y += agent.angle.y;
    DrawPixelV(agent.position,WHITE);
}
int main(){
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(250,250,"slime");
    SetTargetFPS(60);
    ClearBackground(BLACK);
    for(int i=0;i<250;++i){
       for(int j=0;j<250;++j){
            Color temp = {255,255,255,(float)hash_func(i*250+j)/4294967295.0*255};
            DrawPixel(i,j,temp);
         }
    }
    while(!WindowShouldClose()){
        BeginDrawing();
        EndDrawing();
    }
}
