#pragma once

#include "flecs.h"
#include "tx_math.h"

typedef vec2 Position;
typedef vec2 Velocity;

typedef struct CommonGameComponents {
    ECS_DECLARE_COMPONENT(Position);
    ECS_DECLARE_COMPONENT(Velocity);
} CommonGameComponents;

void CommonGameComponentsImport(ecs_world_t* world);

#define CommonGameComponentsImportHandles(handles)                                                 \
    ECS_IMPORT_COMPONENT(handles, Position);                                                       \
    ECS_IMPORT_COMPONENT(handles, Velocity);