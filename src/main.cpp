#include <raylib.h>
#include <vector>
#include <cmath>
#include <iostream>
#include "render.h"
using namespace std;

const int RES=500;

struct Agent{
    Vector2 position;
    Vector2 angle;
};

vector<Agent> agent_list;

extern Color *pixels; // import pixel array from render.cpp


void UpdateData(Agent &agent){
    agent.position.x += agent.angle.x; 
    agent.position.y += agent.angle.y; 
    if(agent.position.x<0 || agent.position.x>=RES) agent.angle.x *= -1;
    if(agent.position.y<0 || agent.position.y>=RES) agent.angle.y *= -1;
    TexPixDraw(agent.position.x,agent.position.y,WHITE);
}

// Initialize induvigual agents with random angle
void AgentInit(float x, float y){
    float randangle =hash_scale(hash_func(y*RES+x))*360*M_PI/180;
    agent_list.push_back(Agent{{x,y},{cos(randangle),sin(randangle)}});
    
}

// Initial bulk generation of agents with random angle
void RandomAgentGenerator(int x){
    for(int b=0;b<x;++b){
        float rango =hash_scale(hash_func(GetRandomValue(0,RES)))*360*M_PI/180;
        agent_list.push_back(Agent{{static_cast<float>(GetRandomValue(0,RES)),static_cast<float>(GetRandomValue(0,RES))},
                                   {cos(rango),sin(rango)}}); 
    }
}

int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT); 
    SetTargetFPS(60);
    InitWindow(RES, RES, "sed");
    Image tuxim = {
        .data = pixels,
        .width = RES,
        .height = RES,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };
    Texture2D tux = LoadTextureFromImage(tuxim);
    UnloadImage(tuxim);
    ClearBackground(BLACK);
    CLS();
    RandomAgentGenerator(100);

    while (!WindowShouldClose()){
        // INPUT HANDLING
        if(GetGestureDetected()==GESTURE_DRAG){
            Vector2 mousePos = GetMousePosition();
            AgentInit(mousePos.x,mousePos.y);
        }
        BeginDrawing();
            DrawTexture(tux,0,0,WHITE);
			for(int k=0;k<agent_list.size();++k){
				UpdateData(agent_list[k]);
			}
        EndDrawing();
        DiffuseTexture();
        UpdateTexture(tux,pixels);
    }
	UnloadTexture(tux);
}
