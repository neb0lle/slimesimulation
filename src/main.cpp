#define _USE_MATH_DEFINES

#include <raylib.h>
#include <vector>
#include <cmath>
#include <iostream>
#include "render.h"

using namespace std;

const int RES=1000;

struct Agent {
    Vector2 position;
    Vector2 angle;
};

vector<Agent> agent_list;
extern Color *pixels; // import pixel array from render.cpp


void Sense(Agent &agent);

void DebugData(Agent &agent) {
    printf("(%f,%f):(%f,%f)\n",agent.position.x,agent.position.y,agent.angle.x,agent.angle.y);
}
void UpdateData(Agent &agent,float speed=1) {
    agent.position.x += agent.angle.x*speed;
    agent.position.y += agent.angle.y*speed;

    if(agent.position.x<=0) {
        agent.angle.x *= -1;
        agent.position.x = 0;
    }
    else if (agent.position.x>=RES) {
        agent.angle.x *= -1;
        agent.position.x = RES;
    }
    if(agent.position.y<=0) {
        agent.angle.y *= -1;
        agent.position.y = 0;
    }
    else if (agent.position.y>=RES) {
        agent.angle.y *= -1;
        agent.position.y = RES;
    }

    for(int k=-1; k<2; ++k) {
        TexPixDraw(agent.position.x+k,agent.position.y,WHITE);
        TexPixDraw(agent.position.x,agent.position.y+k,WHITE);
    }
    Sense(agent);
}

void Sense(Agent &agent) {
    DebugData(agent);
}

// Initialize induvigual agents with random angle at x,y
void AgentInit(float x, float y) {
    float randangle = GetRandomValue(0,360);
    Vector2 agent_angle = {cos(static_cast<float>(randangle*M_PI/180)),sin(static_cast<float>(randangle*M_PI/180))};
    agent_list.push_back(Agent{{x,y},agent_angle});
}

// Initial bulk generation of agents with random angle
void RandomAgentGenerator(int n, int rangl=0, int rangr=RES) {
    for(int b=0; b<n; ++b) {
        float randangle = GetRandomValue(0,360);
        printf("angle: %f\n",randangle);
        Vector2 agent_angle = {cos(static_cast<float>(randangle*M_PI/180)),sin(static_cast<float>(randangle*M_PI/180))};
        agent_list.push_back(Agent{
            { static_cast<float>(GetRandomValue(rangl,rangr)), static_cast<float>(GetRandomValue(rangl,rangr)) },
            agent_angle});
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
    // RandomAgentGenerator(1000,350,650);
    float temp = 90*M_PI/180;
    agent_list.push_back(Agent{{500,500},{cos(temp),sin(temp)}});

    while (!WindowShouldClose()) {
        // INPUT HANDLING
        if(GetGestureDetected()==GESTURE_DRAG) {
            Vector2 mousePos = GetMousePosition();
            AgentInit(mousePos.x,mousePos.y);
        }
        DiffuseTexture();
        BeginDrawing();
        DrawTexture(tux,0,0,WHITE);
        for(int k=0; k<agent_list.size(); ++k) {
            UpdateData(agent_list[k],.5);
        }
        EndDrawing();
        UpdateTexture(tux,pixels);
    }
    UnloadTexture(tux);
}
