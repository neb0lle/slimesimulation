#include <raylib.h>
#include <vector>
#include <cmath>
#include <iostream>
#include "render.h"

using namespace std;

const int RES=1000;

struct Agent {
    Vector2 position;
    float angle;
    Vector2 rangle;
};

struct SpeciesSettings {
    float moveSpeed;
    float turnSpeed;
    float sensorAngleOffset;
    float sensorOffsetDst;
    int sensorSize;
    Color color;
};

extern Color *pixels;
vector<Agent> agent_list;

Vector2 ResolveAngle(float angle) {
    angle = angle*DEG2RAD;
    return {cos(angle),sin(angle)};
}

double InvResolveAngle(Vector2 rangle) {
    double theta = std::atan2(rangle.y, rangle.x)*RAD2DEG;
    return theta;
}

float Sense(Agent *agent, SpeciesSettings settings, float angleOffset) {
    Vector2 sensorAngle = ResolveAngle(agent->angle + angleOffset);
    Vector2 sensorDir = {sensorAngle.x * settings.sensorOffsetDst, sensorAngle.y * settings.sensorOffsetDst};
    int sensorCentreX = static_cast<int>(agent->position.x + sensorDir.x);
    int sensorCentreY = static_cast<int>(agent->position.y + sensorDir.y);

    float sum = 0.0;
    for (int offsetX = -settings.sensorSize; offsetX <= settings.sensorSize; offsetX++) {
        for (int offsetY = -settings.sensorSize; offsetY <= settings.sensorSize; offsetY++) {
            int sampleX = min(RES, max(0, sensorCentreX + offsetX));
            int sampleY = min(RES, max(0, sensorCentreY + offsetY));
            Color sampledColor = pixels[sampleY * RES + sampleX];
            sum += (sampledColor.r + sampledColor.g + sampledColor.b) / 3.0;
        }
    }
    return sum;
}

void Clamp(Agent *a, float minValue, float maxValue) {
    if (a->position.x <= minValue) {
        a->position.x = minValue;
        a->rangle.x *= -1;
        a->angle = InvResolveAngle(a->rangle);
    } else if (a->position.x >= maxValue) {
        a->position.x = maxValue;
        a->rangle.x *=-1;
        a->angle = InvResolveAngle(a->rangle)+180;
    }
    if (a->position.y <= minValue) {
        a->position.y = minValue;
        a->rangle.y *= -1;
        a->angle = InvResolveAngle(a->rangle);
    } else if (a->position.y >= maxValue) {
        a->position.y = maxValue;
        a->rangle.y *= -1;
        a->angle = InvResolveAngle(a->rangle)+180;
    }
}

void UpdateData(Agent *agent, SpeciesSettings settings) {
    // Sense
    float weightForward = Sense(agent, settings,0);
    float weightRight = Sense(agent, settings, settings.sensorAngleOffset);
    float weightLeft = Sense(agent, settings, -settings.sensorAngleOffset);

    // Rotate
    if (weightForward > weightLeft && weightForward > weightRight) {
        // continue
    }
    else if (weightRight > weightLeft) {
        agent->angle += settings.turnSpeed;
    }
    else if (weightLeft > weightRight) {
        agent->angle -= settings.turnSpeed;
    }
    agent->rangle = ResolveAngle(agent->angle);


    // Move
    agent->position.x += agent->rangle.x * settings.moveSpeed;
    agent->position.y += agent->rangle.y * settings.moveSpeed;
    Clamp(agent, 0.0f, static_cast<float>(RES));

    // Deposit
    TexPixDraw(agent->position.x,agent->position.y,settings.color);
}

void AgentInit(float x, float y) {
    float randangle = GetRandomValue(0,360);
    agent_list.push_back(Agent{{x,y},randangle,ResolveAngle(randangle)});
}

void RandomAgentGenerator(int n, int rangl=0, int rangr=RES) {
    for(int b=0; b<n; ++b) {
        float randangle = GetRandomValue(0,360);
        Vector2 randompos = {static_cast<float>(GetRandomValue(rangl,rangr)),static_cast<float>(GetRandomValue(rangl,rangr))};
        agent_list.push_back(Agent{ randompos, randangle, ResolveAngle(randangle)});
    }
}
void RandomAgentGeneratorInCircle(int n, Vector2 center, float radius) {
    for (int b = 0; b < n; ++b) {
        float randAngle = static_cast<float>(GetRandomValue(0, 360)); // Random angle in degrees
        float randRadius = static_cast<float>(GetRandomValue(0, 100)) / 100.0f; // Random value between 0 and 1
        float angleInRadians = randAngle * DEG2RAD; // Convert degrees to radians
        float x = center.x + radius * randRadius * cosf(angleInRadians); // Calculate x-coordinate
        float y = center.y + radius * randRadius * sinf(angleInRadians); // Calculate y-coordinate
        agent_list.push_back(Agent{Vector2{x, y}, randAngle, ResolveAngle(randAngle)});
    }
}

int main() {
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
    CLS(BLACK);

    // RandomAgentGenerator(7500,1,RES-1);
    RandomAgentGeneratorInCircle(5000, {500,500},100);
    SpeciesSettings temp_setting = {1.0f,5.0f,45.0f,5,3,SKYBLUE};

    while (!WindowShouldClose()) {
        if(GetGestureDetected()==GESTURE_DRAG) {
            Vector2 mousePos = GetMousePosition();
            AgentInit(mousePos.x,mousePos.y);
        }

        for(int k=0; k<agent_list.size(); ++k) {
            UpdateData(&agent_list[k],temp_setting); // sense rotate move
        }
        UpdateTexture(tux,pixels); // deposit

        BeginDrawing();
        DrawTexture(tux,0,0,WHITE);
        EndDrawing();

        DDTexture(.1,1);
    }
    UnloadTexture(tux);
    // freeParray(pixels);
}
