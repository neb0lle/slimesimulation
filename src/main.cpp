#include <raylib.h>
#include <vector>
#include <cmath>
#include <iostream>
#include "render.h"
using namespace std;

const int RES=1000;

struct Agent{
    Vector2 position;
    Vector3 angle;
};

vector<Agent> agent_list;
// vector<Agent> agent_list = {
//     {{100,100},{1,0.8}},
//     {{200,500},{0.4,0.3}},
//     {{300,400},{0.5,0.4}},
//     {{400,300},{0.6,0.5}},
//     {{500,200},{0.7,0.6}},
//     {{400,100},{0.8,0.2}},
// };

extern Color *pixels;

// void AgentInit(int n){  //min 4
//     for(int i=(RES/2)-(n/4);i<(RES/2)+(n/4);++i){
//         for(int j=(RES/2)-(n/4);j<(RES/2)+(n/4);++j){
//             float randangle =hash_scale(hash_func(i*RES+j))*360*M_PI/180;
//             agent_list.push_back(Agent{{(float)i,(float)j},{cos(randangle),sin(randangle),static_cast<float>(randangle*180/M_PI)}});
//         }
//     }
// }

void AgentInit(float x, float y){
    float randangle =hash_scale(hash_func(y*RES+x))*360*M_PI/180;
    agent_list.push_back(Agent{{x,y},{cos(randangle),sin(randangle),static_cast<float>(randangle*180/M_PI)}});
}

int Signum(float x){
    if(x>0) return 1;
    if(x<0) return -1;
    return 0;
}

void AgentSense(Agent &agent){
    Vector2 r,l;
    Vector2 sensPos = {(float)Signum(agent.angle.x),(float)Signum(agent.angle.x)};
    if(sensPos.x == 1 && sensPos.y == 1){
        r = {-1,0};
        l = {0,-1};
    }
    else if(sensPos.x == 1 && sensPos.y == -1){
        r = {0,-1};
        l = {1,0};
    }
    else if(sensPos.x == -1 && sensPos.y == 1){
        r = {0,1};
        l = {-1,0};
    }
    else if(sensPos.x == -1 && sensPos.y == -1){
        r = {1,0};
        l = {0,1};
    }
    else if(sensPos.x == 1 && sensPos.y == 0){
        r = {0,1};
        l = {0,-1};
    }
    else if(sensPos.x == 0 && sensPos.y == 1){
        r = {-1,0};
        l = {1,0};
    }
    else if(sensPos.x == -1 && sensPos.y == 0){
        r = {0,-1};
        l = {0,1};
    }
    else if(sensPos.x == 0 && sensPos.y == -1){
        r = {1,0};
        l = {-1,0};
    }
    
    if(pixels[static_cast<int>((agent.position.y+r.y)*RES+agent.position.x+r.x)].r > 0){
        agent.angle.z += 1;
    }
    agent.angle.x = acos(agent.angle.z*M_PI/180);
    agent.angle.y = asin(agent.angle.z*M_PI/180);
}

void UpdateData(Agent &agent){
    AgentSense(agent);
    agent.position.x += agent.angle.x; 
    agent.position.y += agent.angle.y; 
    if(agent.position.x<0 || agent.position.x>=RES) agent.angle.x *= -1;
    if(agent.position.y<0 || agent.position.y>=RES) agent.angle.y *= -1;
    TexPixDraw(agent.position.x,agent.position.y,WHITE);
}


int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT); 
    SetTargetFPS(120);
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

    while (!WindowShouldClose()){
        if(GetGestureDetected()==GESTURE_DRAG){
            Vector2 mousePos = GetMousePosition();
            AgentInit(mousePos.x,mousePos.y);
        }
        BeginDrawing();
            DrawTexture(tux,0,0,WHITE);
            for(int k=0;k<agent_list.size();++k) UpdateData(agent_list[k]);
        EndDrawing();
        DiffuseTexture();
        UpdateTexture(tux,pixels);
    }
    UnloadTexture(tux);
}
