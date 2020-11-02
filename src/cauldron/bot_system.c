#include "bot_system.h"

#include "entity_system.h"
#include "stb_ds.h"

bot* bots = NULL;
bot_handle* bot_handles = NULL;

uint16_t bot_gen = 1;
int bot_free = 0;

static void on_entity_created(entity ent)
{
    bot_handle* bot_h = entity_get_bot(ent);
    actor_handle* actor_h = entity_get_actor(ent);
    if (bot_h && actor_h && bot_handle_valid(*bot_h)) {
        bot* bot = bot_get(*bot_h);
        bot->actor = *actor_h;
    }
}

void bot_system_init(game_settings* settings)
{
    arrsetlen(bots, 64);
    arrsetlen(bot_handles, 64);

    memset(bots, 0, sizeof(bot) * 64);
    memset(bot_handles, 0, sizeof(bot_handle) * 64);

    entity_system_subscribe_entity_created(on_entity_created);
}

void bot_system_shutdown(void)
{
    arrfree(bots);
    arrfree(bot_handles);
}

void bot_system_load_level(game_level* level)
{
}

void bot_system_unload_level(void)
{
}

void bot_system_update(float dt)
{
    for (int i = 0; i < arrlen(bot_handles); ++i) {
        if (!VALID_HANDLE(bot_handles[i])) {
            continue;
        }

        bot* bot = bot_get(bot_handles[i]);

        actor* actor = actor_get(bot->actor);
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

void bot_system_render(float rt)
{
}

HANDLE(bot) * bot_system_get_handles()
{
    return bot_handles;
}

size_t bot_system_get_handles_len()
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
