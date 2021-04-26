#pragma once

#include "flecs.h"

typedef struct SystemSdl2 {
    ECS_DECLARE_ENTITY(Sdl2);
} SystemSdl2;

void SystemSdl2Import(ecs_world_t* world);

#define SystemSdl2ImportHandles(handles) ECS_IMPORT_ENTITY(handles, Sdl2);