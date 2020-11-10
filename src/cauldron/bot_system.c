#include "bot_system.h"

#include "entity_system.h"
#include "event_messages.h"
#include "event_system.h"
#include "stb_ds.h"

bot* bots = NULL;
bot_handle* bot_handles = NULL;

uint16_t bot_gen = 1;
int bot_free = 0;

static void on_entity_spawned(event_message* message)
{
    on_entity_spawned_event* on_entity_spawned = (on_entity_spawned_event*)message;

    actor_handle h_actor = on_entity_spawned->h_actor;
    bot_handle h_bot = on_entity_spawned->h_bot;

    if (bot_handle_valid(h_bot) && actor_handle_valid(h_actor)) {
        bot_get(h_bot)->actor = h_actor;
    }
}

tx_result bot_system_init(game_settings* settings)
{
    arrsetlen(bots, 64);
    arrsetlen(bot_handles, 64);

    memset(bots, 0, sizeof(bot) * 64);
    memset(bot_handles, 0, sizeof(bot_handle) * 64);

    event_system_subscribe(EventMessage_OnEntitySpawned, on_entity_spawned);

    return TX_SUCCESS;
}

void bot_system_term(void)
{
    arrfree(bots);
    arrfree(bot_handles);
}
void bot_system_update(float dt)
{
    for (int i = 0; i < arrlen(bot_handles); ++i) {
        if (!VALID_HANDLE(bot_handles[i])) {
            continue;
        }

        bot* bot = bot_get(bot_handles[i]);

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

HANDLE(bot) * get_bot_handles()
{
    return bot_handles;
}

size_t get_bot_handles_len()
{
    return arrlen(bot_handles);
}

bot_handle bot_create(const bot_desc* const desc)
{
    if (desc != NULL) {
        int start = bot_free;
        while (bot_handles[bot_free].value != INVALID_RAW_HANDLE) {
            bot_free = (bot_free + 1) % arrlen(bot_handles);
            if (bot_free == start) {
                return INVALID_HANDLE(bot);
            }
        }
        uint32_t index = (uint32_t)bot_free;
        bot_handles[index] = bot_handle_make(index, bot_gen);
        bots[index] = (bot){
            .type = desc->type,
            .dir = 1.0f,
            .jump_timer = index / 1.618034f,
        };
        return bot_handles[index];
    }
    return INVALID_HANDLE(bot);
}

bool bot_destroy(bot_handle handle)
{
    if (bot_handle_valid(handle)) {
        uint32_t index = bot_handle_get_index(handle);
        bot_handles[index].value = INVALID_RAW_HANDLE;
        return true;
    }
    return false;
}

bot* bot_get(bot_handle handle)
{
    if (bot_handle_valid(handle)) {
        return &bots[bot_handle_get_index(handle)];
    }
    return NULL;
}
