#include "bot_system.h"

#include "entity_system.h"
#include "event_messages.h"
#include "event_system.h"
#include "stb_ds.h"

POOL_IMPL(bot);

static void on_entity_spawned(event_message* message)
{
    on_entity_spawned_event* on_entity_spawned = (on_entity_spawned_event*)message;

    actor_handle h_actor = on_entity_spawned->h_actor;
    bot_handle h_bot = on_entity_spawned->h_bot;

    if (bot_handle_valid(h_bot) && actor_handle_valid(h_actor)) {
        bot_ptr(h_bot)->actor = h_actor;
    }
}

tx_result bot_system_init(game_settings* settings)
{
    bot_pool_set_capacity(64);

    event_system_subscribe(EventMessage_OnEntitySpawned, on_entity_spawned);

    return TX_SUCCESS;
}

void bot_system_term(void)
{
    bot_pool_free();
}

void bot_system_unload_level(void)
{
    bot_pool_release_all();
}

void bot_system_update(float dt)
{
    for (int i = 0; i < arrlen(bot_pool.handles); ++i) {
        if (!VALID_HANDLE(bot_pool.handles[i])) {
            continue;
        }

        bot* bot = bot_ptr(bot_pool.handles[i]);

        actor* actor = actor_ptr(bot->actor);
        if (actor) {
            if ((actor->flags & ActorFlags_HitWall) != 0) {
                bot->dir *= -1.0f;
            }
            actor->input.move.x = bot->dir;
            if (bot->jump_timer <= 0.0f) {
                bot->jump_timer += 2.0f;
                actor->input.jump = true;
            } else {
                actor->input.jump = false;
                bot->jump_timer -= dt;
            }
        }
    }
}

bot_handle bot_create(const bot_desc* const desc)
{
    if (desc) {
        bot_handle handle = bot_acquire();
        if (VALID_HANDLE(handle)) {
            uint32_t index = bot_handle_get_index(handle);
            bot_pool.data[index] = (bot){
                .type = desc->type,
                .dir = 1.0f,
                .jump_timer = index / 1.618034f,
            };
            return handle;
        }
    }
    return INVALID_HANDLE(bot);
}

bool bot_destroy(bot_handle handle)
{
    if (bot_handle_valid(handle)) {
        bot_release(handle);
        return true;
    }
    return false;
}
