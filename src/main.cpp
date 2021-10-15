#include <raylib.h>
#include <vector>
#include <cmath>
#include <iostream>
#include "render.h"
using namespace std;

const int RES=1000;

struct Agent{
    Vector2 position;
    Vector2 angle;
	pair<bool,bool> is_oob;
};

vector<Agent> agent_list;

extern Color *pixels; // import pixel array from render.cpp


void Sense(Agent &agent);

void UpdateData(Agent &agent){
    agent.position.x += agent.angle.x; 
    agent.position.y += agent.angle.y; 

	if(agent.position.x<0 || agent.position.x>=RES){
		if(agent.is_oob.first == false){
			agent.angle.x *= -1;
			agent.is_oob.first = true;
		}
	}
	else agent.is_oob.first = false;
	if(agent.position.y<0 || agent.position.y>=RES){
		if(agent.is_oob.second == false){
			agent.angle.y *= -1;
			agent.is_oob.second = true;
		}
	}
	else agent.is_oob.second = false;
	for(int k=-1; k<2; ++k){
		TexPixDraw(agent.position.x+k,agent.position.y,WHITE);
		TexPixDraw(agent.position.x,agent.position.y+k,WHITE);
	}
	Sense(agent);
}

void Sense(Agent &agent){
	int max_intensity = 0;
	if(agent.angle.x>0 && agent.angle.y>0){
		for(int i=0; i<3; ++i){
			for(int j=0; j<3; ++j){
			}
		}
	}
}

// Initialize induvigual agents with random angle
void AgentInit(float x, float y){
    float randangle =hash_scale(hash_func(y*RES+x))*360*M_PI/180;
    agent_list.push_back(Agent{{x,y},{cos(randangle),sin(randangle)},{false,false}});
    
}

// Initial bulk generation of agents with random angle
void RandomAgentGenerator(int x, int rangl=0, int rangr=RES){
    for(int b=0;b<x;++b){
        float rango =hash_scale(hash_func(GetRandomValue(rangl,rangr)))*360*M_PI/180;
		agent_list.push_back(Agent{
				{
					static_cast<float>(GetRandomValue(rangl,rangr)),
					static_cast<float>(GetRandomValue(rangl,rangr))
				},
				{cos(rango)*2,sin(rango)*2},
				{false,false}}
				); 
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
    RandomAgentGenerator(1000,450,550);

    while (!WindowShouldClose()){
        // INPUT HANDLING
        if(GetGestureDetected()==GESTURE_DRAG){
            Vector2 mousePos = GetMousePosition();
            AgentInit(mousePos.x,mousePos.y);
        }

        DiffuseTexture();
        BeginDrawing();
            DrawTexture(tux,0,0,WHITE);
			for(int k=0;k<agent_list.size();++k){
				UpdateData(agent_list[k]);
			}
        EndDrawing();
        UpdateTexture(tux,pixels);
    }
	UnloadTexture(tux);
}
