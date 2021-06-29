#include <raylib.h>
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

const int RES = 500;
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

float hash_scale(uint state){
    return state / 4294967295.0;
}

vector<Agent> agent_list;
void agent_init(){
    for(int i=(RES/2)-5;i<RES/2+5;++i){
        for(int j=(RES/2)-5;j<RES/2+5;++j){
            float randangle =hash_scale(hash_func(i*RES+j))*360*M_PI/180;
            agent_list.push_back(Agent{{(float)i,(float)j},{cos(randangle),sin(randangle)}});
        }
    }
}
void agent_update(Agent &agent){ 
    agent.position.x += agent.angle.x;
    agent.position.y += agent.angle.y;
    if(agent.position.x<0 || agent.position.x>RES) agent.angle.x *= -1;
    if(agent.position.y<0 || agent.position.y>RES) agent.angle.y *= -1;
    DrawPixelV(agent.position,WHITE);
}
int main(){
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(RES,RES,"sed");
    SetTargetFPS(60);
    ClearBackground(BLACK);
    agent_init();
    while(!WindowShouldClose()){
        BeginDrawing();
        for(int k=0;k<agent_list.size();++k){
            agent_update(agent_list[k]);
        }
        EndDrawing();
    }
}
