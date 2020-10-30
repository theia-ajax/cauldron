#include "game_settings.h"

#include "futils.h"
#include "jsonutil.h"
#include "stb_ds.h"

game_settings settings = {0};

game_settings* const get_game_settings()
{
    return &settings;
}

bool load_game_settings(const char* file_override)
{
    char* filename = "assets/game_settings.json";
    if (file_override) {
        filename = (char*)file_override;
    }

    char* js;
    size_t len;
    tx_result result = read_file_to_buffer(filename, &js, &len);

    if (result != TX_SUCCESS) {
        return result;
    }

    jsmn_parser parser;
    jsmn_init(&parser);

    int tok_needed = jsmn_parse(&parser, js, len, NULL, 0);
    jsmntok_t* tokens = NULL;
    arrsetlen(tokens, tok_needed);

    jsmn_init(&parser);
    int tok_len = jsmn_parse(&parser, js, len, tokens, tok_needed);

    int opt_id = jsget_id(js, tokens, 0, "options");
    {
        int video_opt_id = jsget_id(js, tokens, opt_id, "video");
        {
            settings.options.video.display_width =
                jstoi_or(js, jsget(js, tokens, video_opt_id, "display_width"), 0);

            settings.options.video.display_height =
                jstoi_or(js, jsget(js, tokens, video_opt_id, "display_height"), 0);

            settings.options.video.enable_vsync =
                jstob_or(js, jsget(js, tokens, video_opt_id, "enable_vsync"), false);

            settings.options.video.frame_limit =
                jstoi_or(js, jsget(js, tokens, video_opt_id, "frame_limit"), 0);
        }
    }

    arrfree(tokens);

    return TX_SUCCESS;
}