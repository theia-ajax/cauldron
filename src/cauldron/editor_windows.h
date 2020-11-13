#pragma once

#include "tx_input.h"
#include "tx_types.h"

typedef void (*editor_window_proc)(bool* open);

typedef struct editor_window_desc {
    const char* menu_path;
    struct {
        txinp_mod mod;
        txinp_key key;
    } shortcut;
    editor_window_proc window_proc;
} editor_window_desc;

enum { EDITOR_WINDOWS_MAX_WINDOWS = 64 };

typedef struct editor_windows_sys_desc {
    editor_window_desc windows[EDITOR_WINDOWS_MAX_WINDOWS];
} editor_windows_sys_desc;

void editor_windows_init(editor_windows_sys_desc* desc);
void editor_windows_term(void);
void editor_windows_process_shortcuts(void);
void editor_windows_draw_menu_bar(void);
void editor_windows_draw_windows(void);
