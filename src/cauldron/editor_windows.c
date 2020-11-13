#include "editor_windows.h"

#include "stb_ds.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

// TODO remove this dependency
#include <SDL2/SDL.h>

struct ew_shortcut {
    txinp_mod mod;
    txinp_key key;
    strhash text_id;
};

struct {
    struct ew_shortcut shortcut[EDITOR_WINDOWS_MAX_WINDOWS];
    editor_window_proc window_proc[EDITOR_WINDOWS_MAX_WINDOWS];
    bool open[EDITOR_WINDOWS_MAX_WINDOWS];
    strhash* sub_menu_ids[EDITOR_WINDOWS_MAX_WINDOWS];
} ew_;

void editor_windows_init(editor_windows_sys_desc* desc)
{
    if (!desc) {
        return;
    }

    memset(&ew_, 0, sizeof(ew_));

    for (int index = 0; index < EDITOR_WINDOWS_MAX_WINDOWS; ++index) {
        editor_window_desc* win_desc = &desc->windows[index];
        if (win_desc->window_proc == NULL) {
            continue;
        }

        enum { BUF_LEN = 32 };
        char buffer[BUF_LEN] = {0};
        SDL_Keycode keycode = SDL_GetKeyFromScancode((SDL_Scancode)win_desc->shortcut.key);
        const char* key_name = SDL_GetKeyName(keycode);
        txinp_write_mod_strn(win_desc->shortcut.mod, key_name, buffer, BUF_LEN);

        ew_.shortcut[index] = (struct ew_shortcut){
            .mod = win_desc->shortcut.mod,
            .key = win_desc->shortcut.key,
            .text_id = strhash_get(buffer),
        };

        ew_.window_proc[index] = win_desc->window_proc;

        size_t menu_len = strlen(win_desc->menu_path);
        if (win_desc->menu_path && menu_len > 0) {
            char* full_menu = calloc(menu_len + 1, sizeof(char));
            strcpy(full_menu, win_desc->menu_path);

            strhash* sub_ids = NULL;

            const char* delim = "/";
            char* menu = strtok(full_menu, delim);
            while (menu) {
                strhash menu_id = strhash_get(menu);
                arrput(sub_ids, menu_id);
                menu = strtok(NULL, delim);
            }

            ew_.sub_menu_ids[index] = sub_ids;

            free(full_menu);
        }
    }
}

void editor_windows_term(void)
{
    for (int i = 0; i < EDITOR_WINDOWS_MAX_WINDOWS; ++i) {
        arrfree(ew_.sub_menu_ids[i]);
    }
}

void editor_windows_process_shortcuts(void)
{
    for (int index = 0; index < EDITOR_WINDOWS_MAX_WINDOWS; ++index) {
        if (txinp_mods_down(ew_.shortcut[index].mod)
            && igIsKeyPressed(ew_.shortcut[index].key, true)) {
            ew_.open[index] = !ew_.open[index];
        }
    }
}

void editor_windows_draw_menu_bar(void)
{
    igBeginMainMenuBar();

    for (int index = 0; index < EDITOR_WINDOWS_MAX_WINDOWS; ++index) {
        size_t sub_len = arrlen(ew_.sub_menu_ids[index]);

        if (sub_len == 0) {
            continue;
        }

        int menu_ct = 0;
        for (size_t ti = 0; ti < sub_len - 1; ++ti) {
            if (igBeginMenu(strhash_cstr(ew_.sub_menu_ids[index][ti]), true)) {
                menu_ct++;
            } else {
                break;
            }
        }

        if (menu_ct >= sub_len - 1) {
            igMenuItemBoolPtr(
                strhash_cstr(ew_.sub_menu_ids[index][sub_len - 1]),
                strhash_cstr(ew_.shortcut[index].text_id),
                &ew_.open[index],
                true);
        }

        for (int i = 0; i < menu_ct; ++i) {
            igEndMenu();
        }
    }
    igEndMainMenuBar();
}

void editor_windows_draw_windows(void)
{
    for (int index = 0; index < EDITOR_WINDOWS_MAX_WINDOWS; ++index) {
        if (ew_.open[index] && ew_.window_proc[index]) {
            ew_.window_proc[index](&ew_.open[index]);
        }
    }
}