#include <raylib.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "render.h"

using namespace std;

const int RES = 1000;

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
    int agentSize;
    Color color;
    int zIndex;  // Add zIndex member for Z index
};

extern Color *pixels;
vector<pair<vector<Agent>, SpeciesSettings>> agent_list_1;

Vector2 ResolveAngle(float angle) {
    angle = angle * DEG2RAD;
    return {cos(angle), sin(angle)};
}

double InvResolveAngle(Vector2 rangle) {
    double theta = std::atan2(rangle.y, rangle.x) * RAD2DEG;
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
            int sampleX = std::min(RES, std::max(0, sensorCentreX + offsetX));
            int sampleY = std::min(RES, std::max(0, sensorCentreY + offsetY));
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
        a->rangle.x *= -1;
        a->angle = InvResolveAngle(a->rangle) + 180;
    }
    if (a->position.y <= minValue) {
        a->position.y = minValue;
        a->rangle.y *= -1;
        a->angle = InvResolveAngle(a->rangle);
    } else if (a->position.y >= maxValue) {
        a->position.y = maxValue;
        a->rangle.y *= -1;
        a->angle = InvResolveAngle(a->rangle) + 180;
    }
}

void UpdateData(Agent *agent, SpeciesSettings settings) {
    float weightForward = Sense(agent, settings, 0);
    float weightRight = Sense(agent, settings, settings.sensorAngleOffset);
    float weightLeft = Sense(agent, settings, -settings.sensorAngleOffset);

    if (weightForward > weightLeft && weightForward > weightRight) {
        // continue
    } else if (weightRight > weightLeft) {
        agent->angle += settings.turnSpeed;
    } else if (weightLeft > weightRight) {
        agent->angle -= settings.turnSpeed;
    }
    agent->rangle = ResolveAngle(agent->angle);

    agent->position.x += agent->rangle.x * settings.moveSpeed;
    agent->position.y += agent->rangle.y * settings.moveSpeed;
    Clamp(agent, 0.0f, static_cast<float>(RES));

    int size = settings.agentSize / 2;
    for (int i = -size; i <= size; i++) {
        for (int j = -size; j <= size; j++) {
            TexPixDraw(agent->position.x + i, agent->position.y + j, settings.color);
        }
    }
}

void RandomAgentGeneratorInCircle(int n, Vector2 center, float minradius, float maxradius, SpeciesSettings settings) {
    vector<Agent> agent_list;
    while (n--) {
        float randAngle = static_cast<float>(GetRandomValue(0, 360));
        float randRadius = static_cast<float>(GetRandomValue(minradius, maxradius));
        float angleInRadians = randAngle * DEG2RAD;
        float x = center.x + randRadius * cos(angleInRadians);
        float y = center.y + randRadius * sin(angleInRadians);
        agent_list.push_back(Agent{{x, y}, randAngle, ResolveAngle(randAngle)});
    }
    agent_list_1.push_back({agent_list, settings});
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(1000);
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

    Color CY = GetColor(0xffef00ff);

    SpeciesSettings setting1 = {1.0f, 5.0f, 75.0f, 5, 3, 1, CY, 1};
    SpeciesSettings setting2 = {1.0f, 5.0f, 75.0f, 5, 3, 1, SKYBLUE, 2};

    while (!WindowShouldClose()) {
        std::sort(agent_list_1.begin(), agent_list_1.end(), [](const auto &a, const auto &b) {
            return a.second.zIndex < b.second.zIndex;
        });

        for (int k = 0; k < agent_list_1.size(); ++k) {
            for (int l = 0; l < agent_list_1[k].first.size(); l++) {
                UpdateData(&agent_list_1[k].first[l], agent_list_1[k].second);
            }
        }

        for (const auto &species : agent_list_1) {
            for (const Agent &agent : species.first) {
                DrawTexture(tux, agent.position.x, agent.position.y, WHITE);
            }
        }

        UpdateTexture(tux, pixels);

        BeginDrawing();
        DrawTexture(tux, 0, 0, WHITE);
        EndDrawing();

        DDTexture(.1, 1);
        printf("%d\n", GetFPS());
    }

    UnloadTexture(tux);
}
