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

SpeciesSettings temp_setting = {0.5f,0.2f,45.0f,10.0f,5,WHITE};

vector<Agent> agent_list;
extern Color *pixels; // import pixel array from render.cpp

float Sense(Agent agent, SpeciesSettings settings, float angleOffset) {
    float sensorAngle = agent.angle + settings.sensorAngleOffset;
    Vector2 sensorDir = {cos(sensorAngle) * settings.sensorOffsetDst, sin(sensorAngle) * settings.sensorOffsetDst};
    int sensorCentreX = static_cast<int>(agent.position.x + sensorDir.x);
    int sensorCentreY = static_cast<int>(agent.position.y + sensorDir.y);

    float sum = 0.0;
    for (int offsetX = -settings.sensorSize; offsetX <= settings.sensorSize; offsetX++) {
        for (int offsetY = -settings.sensorSize; offsetY <= settings.sensorSize; offsetY++) {
            int sampleX = min(RES - 1, max(0, sensorCentreX + offsetX));
            int sampleY = min(RES - 1, max(0, sensorCentreY + offsetY));
            Color sampledColor = pixels[sampleY * RES + sampleX];
            sum += (sampledColor.r + sampledColor.g + sampledColor.b) / 3.0;
        }
    }

    return sum;
}

float UnresolveAngle(Vector2 rangle) {
    return 0;
}

void Clamp(Agent *a, float minValue, float maxValue) {
    // clamp along with recalculation of angle
    // need offset for certain bounds for angle
    if (a->position.x < minValue) {
        a->position.x = minValue;
        a->rangle.x *= -1;
        a->angle = UnresolveAngle(a->rangle);
    } else if (a->position.x > maxValue) {
        a->position.x = maxValue;
        a->rangle.x *=-1;
        a->angle = UnresolveAngle(a->rangle);
    }
    if (a->position.y < minValue) {
        a->position.y = minValue;
        a->rangle.y *= -1;
        a->angle = UnresolveAngle(a->rangle);
    } else if (a->position.y > maxValue) {
        a->position.y = maxValue;
        a->rangle.y *= -1;
        a->angle = UnresolveAngle(a->rangle);
    }
}

void UpdateData(Agent &agent, SpeciesSettings settings) {
    // Calculate sensor readings
    // to make it single function return instead of 3 times;
    float weightForward = Sense(agent, settings,0);
    float weightLeft = Sense(agent, settings, -settings.sensorAngleOffset);
    float weightRight = Sense(agent, settings, settings.sensorAngleOffset);

    float randomSteerStrength = static_cast<float>(GetRandomValue(0, 100)) / 100.0f - 0.5f;
    float turnSpeed = settings.turnSpeed * 2.0f * 3.1415f;
    if (weightForward > weightLeft && weightForward > weightRight) {
        // Continue in the same direction
    }
    else if (weightForward < weightLeft && weightForward < weightRight) {
        // Turn based on random steering strength
        agent.angle += randomSteerStrength * turnSpeed;
    }
    else if (weightRight > weightLeft) {
        // Turn right
        agent.angle -= randomSteerStrength * turnSpeed;
    }
    else if (weightLeft > weightRight) {
        // Turn left
        agent.angle += randomSteerStrength * turnSpeed;
    }

    // Update agent's position based on angle and speed
    agent.position.x += cos(agent.angle) * settings.moveSpeed;
    agent.position.y += sin(agent.angle) * settings.moveSpeed;

    Clamp(&agent, 0.0f, static_cast<float>(RES));
}

// Initialize individual agents with random angle at x,y
void AgentInit(float x, float y) {
    float randangle = GetRandomValue(0,360);
    agent_list.push_back(Agent{{x,y},randangle});
}

// Initial bulk generation of agents with random angle
void RandomAgentGenerator(int n, int rangl=0, int rangr=RES) {
    for(int b=0; b<n; ++b) {
        float randangle = GetRandomValue(0,360);
        agent_list.push_back(Agent{ { static_cast<float>(GetRandomValue(rangl,rangr)), static_cast<float>(GetRandomValue(rangl,rangr)) }, randangle});
    }
}

int main() {
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
    RandomAgentGenerator(10,0,RES);


    while (!WindowShouldClose()) {
        if(GetGestureDetected()==GESTURE_DRAG) {
            Vector2 mousePos = GetMousePosition();
            AgentInit(mousePos.x,mousePos.y);
        }

        // sense rotate move
        UpdateTexture(tux,pixels);

        BeginDrawing(); // deposit
        DrawTexture(tux,0,0,WHITE);
        for(int k=0; k<agent_list.size(); ++k) {
            UpdateData(agent_list[k],temp_setting);
        }
        EndDrawing();

        // DDTexture(.1,1); // diffuse and decay
    }
    UnloadTexture(tux);
}
