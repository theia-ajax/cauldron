#include "common_game_components.h"

void CommonGameComponentsImport(ecs_world_t* world)
{
    ECS_MODULE(world, CommonGameComponents);

    ecs_set_name_prefix(world, "common");

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_EXPORT_COMPONENT(Position);
    ECS_EXPORT_COMPONENT(Velocity);
}