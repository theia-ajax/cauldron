#pragma once

#include "stb_ds.h"
#include "strhash.h"
#include "tx_types.h"

// just copied from SDL2 SDL_Scancode
// will probably rename and adjust later but it's a good starting point
// we will just always use scancodes as keys because keyboard position is more important right now
typedef enum {
    TXINP_KEY_UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
    /* @{ */

    TXINP_KEY_A = 4,
    TXINP_KEY_B = 5,
    TXINP_KEY_C = 6,
    TXINP_KEY_D = 7,
    TXINP_KEY_E = 8,
    TXINP_KEY_F = 9,
    TXINP_KEY_G = 10,
    TXINP_KEY_H = 11,
    TXINP_KEY_I = 12,
    TXINP_KEY_J = 13,
    TXINP_KEY_K = 14,
    TXINP_KEY_L = 15,
    TXINP_KEY_M = 16,
    TXINP_KEY_N = 17,
    TXINP_KEY_O = 18,
    TXINP_KEY_P = 19,
    TXINP_KEY_Q = 20,
    TXINP_KEY_R = 21,
    TXINP_KEY_S = 22,
    TXINP_KEY_T = 23,
    TXINP_KEY_U = 24,
    TXINP_KEY_V = 25,
    TXINP_KEY_W = 26,
    TXINP_KEY_X = 27,
    TXINP_KEY_Y = 28,
    TXINP_KEY_Z = 29,

    TXINP_KEY_1 = 30,
    TXINP_KEY_2 = 31,
    TXINP_KEY_3 = 32,
    TXINP_KEY_4 = 33,
    TXINP_KEY_5 = 34,
    TXINP_KEY_6 = 35,
    TXINP_KEY_7 = 36,
    TXINP_KEY_8 = 37,
    TXINP_KEY_9 = 38,
    TXINP_KEY_0 = 39,

    TXINP_KEY_RETURN = 40,
    TXINP_KEY_ESCAPE = 41,
    TXINP_KEY_BACKSPACE = 42,
    TXINP_KEY_TAB = 43,
    TXINP_KEY_SPACE = 44,

    TXINP_KEY_MINUS = 45,
    TXINP_KEY_EQUALS = 46,
    TXINP_KEY_LEFTBRACKET = 47,
    TXINP_KEY_RIGHTBRACKET = 48,
    TXINP_KEY_BACKSLASH = 49, /**< Located at the lower left of the return
                               *   key on ISO keyboards and at the right end
                               *   of the QWERTY row on ANSI keyboards.
                               *   Produces REVERSE SOLIDUS (backslash) and
                               *   VERTICAL LINE in a US layout, REVERSE
                               *   SOLIDUS and VERTICAL LINE in a UK Mac
                               *   layout, NUMBER SIGN and TILDE in a UK
                               *   Windows layout, DOLLAR SIGN and POUND SIGN
                               *   in a Swiss German layout, NUMBER SIGN and
                               *   APOSTROPHE in a German layout, GRAVE
                               *   ACCENT and POUND SIGN in a French Mac
                               *   layout, and ASTERISK and MICRO SIGN in a
                               *   French Windows layout.
                               */
    TXINP_KEY_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                               *   instead of 49 for the same key, but all
                               *   OSes I've seen treat the two codes
                               *   identically. So, as an implementor, unless
                               *   your keyboard generates both of those
                               *   codes and your OS treats them differently,
                               *   you should generate TXINP_KEY_BACKSLASH
                               *   instead of this code. As a user, you
                               *   should not rely on this code because SDL
                               *   will never generate it with most (all?)
                               *   keyboards.
                               */
    TXINP_KEY_SEMICOLON = 51,
    TXINP_KEY_APOSTROPHE = 52,
    TXINP_KEY_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                           *   and ISO keyboards). Produces GRAVE ACCENT and
                           *   TILDE in a US Windows layout and in US and UK
                           *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                           *   and NOT SIGN in a UK Windows layout, SECTION
                           *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                           *   layouts on ISO keyboards, SECTION SIGN and
                           *   DEGREE SIGN in a Swiss German layout (Mac:
                           *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                           *   DEGREE SIGN in a German layout (Mac: only on
                           *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                           *   French Windows layout, COMMERCIAL AT and
                           *   NUMBER SIGN in a French Mac layout on ISO
                           *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                           *   SIGN in a Swiss German, German, or French Mac
                           *   layout on ANSI keyboards.
                           */
    TXINP_KEY_COMMA = 54,
    TXINP_KEY_PERIOD = 55,
    TXINP_KEY_SLASH = 56,

    TXINP_KEY_CAPSLOCK = 57,

    TXINP_KEY_F1 = 58,
    TXINP_KEY_F2 = 59,
    TXINP_KEY_F3 = 60,
    TXINP_KEY_F4 = 61,
    TXINP_KEY_F5 = 62,
    TXINP_KEY_F6 = 63,
    TXINP_KEY_F7 = 64,
    TXINP_KEY_F8 = 65,
    TXINP_KEY_F9 = 66,
    TXINP_KEY_F10 = 67,
    TXINP_KEY_F11 = 68,
    TXINP_KEY_F12 = 69,

    TXINP_KEY_PRINTSCREEN = 70,
    TXINP_KEY_SCROLLLOCK = 71,
    TXINP_KEY_PAUSE = 72,
    TXINP_KEY_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    TXINP_KEY_HOME = 74,
    TXINP_KEY_PAGEUP = 75,
    TXINP_KEY_DELETE = 76,
    TXINP_KEY_END = 77,
    TXINP_KEY_PAGEDOWN = 78,
    TXINP_KEY_RIGHT = 79,
    TXINP_KEY_LEFT = 80,
    TXINP_KEY_DOWN = 81,
    TXINP_KEY_UP = 82,

    TXINP_KEY_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                  */
    TXINP_KEY_KP_DIVIDE = 84,
    TXINP_KEY_KP_MULTIPLY = 85,
    TXINP_KEY_KP_MINUS = 86,
    TXINP_KEY_KP_PLUS = 87,
    TXINP_KEY_KP_ENTER = 88,
    TXINP_KEY_KP_1 = 89,
    TXINP_KEY_KP_2 = 90,
    TXINP_KEY_KP_3 = 91,
    TXINP_KEY_KP_4 = 92,
    TXINP_KEY_KP_5 = 93,
    TXINP_KEY_KP_6 = 94,
    TXINP_KEY_KP_7 = 95,
    TXINP_KEY_KP_8 = 96,
    TXINP_KEY_KP_9 = 97,
    TXINP_KEY_KP_0 = 98,
    TXINP_KEY_KP_PERIOD = 99,

    TXINP_KEY_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                     *   keyboards have over ANSI ones,
                                     *   located between left shift and Y.
                                     *   Produces GRAVE ACCENT and TILDE in a
                                     *   US or UK Mac layout, REVERSE SOLIDUS
                                     *   (backslash) and VERTICAL LINE in a
                                     *   US or UK Windows layout, and
                                     *   LESS-THAN SIGN and GREATER-THAN SIGN
                                     *   in a Swiss German, German, or French
                                     *   layout. */
    TXINP_KEY_APPLICATION = 101,    /**< windows contextual menu, compose */
    TXINP_KEY_POWER = 102,          /**< The USB document says this is a status flag,
                                     *   not a physical key - but some Mac keyboards
                                     *   do have a power key. */
    TXINP_KEY_KP_EQUALS = 103,
    TXINP_KEY_F13 = 104,
    TXINP_KEY_F14 = 105,
    TXINP_KEY_F15 = 106,
    TXINP_KEY_F16 = 107,
    TXINP_KEY_F17 = 108,
    TXINP_KEY_F18 = 109,
    TXINP_KEY_F19 = 110,
    TXINP_KEY_F20 = 111,
    TXINP_KEY_F21 = 112,
    TXINP_KEY_F22 = 113,
    TXINP_KEY_F23 = 114,
    TXINP_KEY_F24 = 115,
    TXINP_KEY_EXECUTE = 116,
    TXINP_KEY_HELP = 117,
    TXINP_KEY_MENU = 118,
    TXINP_KEY_SELECT = 119,
    TXINP_KEY_STOP = 120,
    TXINP_KEY_AGAIN = 121, /**< redo */
    TXINP_KEY_UNDO = 122,
    TXINP_KEY_CUT = 123,
    TXINP_KEY_COPY = 124,
    TXINP_KEY_PASTE = 125,
    TXINP_KEY_FIND = 126,
    TXINP_KEY_MUTE = 127,
    TXINP_KEY_VOLUMEUP = 128,
    TXINP_KEY_VOLUMEDOWN = 129,
    /* not sure whether there's a reason to enable these */
    /*     TXINP_KEY_LOCKINGCAPSLOCK = 130,  */
    /*     TXINP_KEY_LOCKINGNUMLOCK = 131, */
    /*     TXINP_KEY_LOCKINGSCROLLLOCK = 132, */
    TXINP_KEY_KP_COMMA = 133,
    TXINP_KEY_KP_EQUALSAS400 = 134,

    TXINP_KEY_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    TXINP_KEY_INTERNATIONAL2 = 136,
    TXINP_KEY_INTERNATIONAL3 = 137, /**< Yen */
    TXINP_KEY_INTERNATIONAL4 = 138,
    TXINP_KEY_INTERNATIONAL5 = 139,
    TXINP_KEY_INTERNATIONAL6 = 140,
    TXINP_KEY_INTERNATIONAL7 = 141,
    TXINP_KEY_INTERNATIONAL8 = 142,
    TXINP_KEY_INTERNATIONAL9 = 143,
    TXINP_KEY_LANG1 = 144, /**< Hangul/English toggle */
    TXINP_KEY_LANG2 = 145, /**< Hanja conversion */
    TXINP_KEY_LANG3 = 146, /**< Katakana */
    TXINP_KEY_LANG4 = 147, /**< Hiragana */
    TXINP_KEY_LANG5 = 148, /**< Zenkaku/Hankaku */
    TXINP_KEY_LANG6 = 149, /**< reserved */
    TXINP_KEY_LANG7 = 150, /**< reserved */
    TXINP_KEY_LANG8 = 151, /**< reserved */
    TXINP_KEY_LANG9 = 152, /**< reserved */

    TXINP_KEY_ALTERASE = 153, /**< Erase-Eaze */
    TXINP_KEY_SYSREQ = 154,
    TXINP_KEY_CANCEL = 155,
    TXINP_KEY_CLEAR = 156,
    TXINP_KEY_PRIOR = 157,
    TXINP_KEY_RETURN2 = 158,
    TXINP_KEY_SEPARATOR = 159,
    TXINP_KEY_OUT = 160,
    TXINP_KEY_OPER = 161,
    TXINP_KEY_CLEARAGAIN = 162,
    TXINP_KEY_CRSEL = 163,
    TXINP_KEY_EXSEL = 164,

    TXINP_KEY_KP_00 = 176,
    TXINP_KEY_KP_000 = 177,
    TXINP_KEY_THOUSANDSSEPARATOR = 178,
    TXINP_KEY_DECIMALSEPARATOR = 179,
    TXINP_KEY_CURRENCYUNIT = 180,
    TXINP_KEY_CURRENCYSUBUNIT = 181,
    TXINP_KEY_KP_LEFTPAREN = 182,
    TXINP_KEY_KP_RIGHTPAREN = 183,
    TXINP_KEY_KP_LEFTBRACE = 184,
    TXINP_KEY_KP_RIGHTBRACE = 185,
    TXINP_KEY_KP_TAB = 186,
    TXINP_KEY_KP_BACKSPACE = 187,
    TXINP_KEY_KP_A = 188,
    TXINP_KEY_KP_B = 189,
    TXINP_KEY_KP_C = 190,
    TXINP_KEY_KP_D = 191,
    TXINP_KEY_KP_E = 192,
    TXINP_KEY_KP_F = 193,
    TXINP_KEY_KP_XOR = 194,
    TXINP_KEY_KP_POWER = 195,
    TXINP_KEY_KP_PERCENT = 196,
    TXINP_KEY_KP_LESS = 197,
    TXINP_KEY_KP_GREATER = 198,
    TXINP_KEY_KP_AMPERSAND = 199,
    TXINP_KEY_KP_DBLAMPERSAND = 200,
    TXINP_KEY_KP_VERTICALBAR = 201,
    TXINP_KEY_KP_DBLVERTICALBAR = 202,
    TXINP_KEY_KP_COLON = 203,
    TXINP_KEY_KP_HASH = 204,
    TXINP_KEY_KP_SPACE = 205,
    TXINP_KEY_KP_AT = 206,
    TXINP_KEY_KP_EXCLAM = 207,
    TXINP_KEY_KP_MEMSTORE = 208,
    TXINP_KEY_KP_MEMRECALL = 209,
    TXINP_KEY_KP_MEMCLEAR = 210,
    TXINP_KEY_KP_MEMADD = 211,
    TXINP_KEY_KP_MEMSUBTRACT = 212,
    TXINP_KEY_KP_MEMMULTIPLY = 213,
    TXINP_KEY_KP_MEMDIVIDE = 214,
    TXINP_KEY_KP_PLUSMINUS = 215,
    TXINP_KEY_KP_CLEAR = 216,
    TXINP_KEY_KP_CLEARENTRY = 217,
    TXINP_KEY_KP_BINARY = 218,
    TXINP_KEY_KP_OCTAL = 219,
    TXINP_KEY_KP_DECIMAL = 220,
    TXINP_KEY_KP_HEXADECIMAL = 221,

    TXINP_KEY_LCTRL = 224,
    TXINP_KEY_LSHIFT = 225,
    TXINP_KEY_LALT = 226, /**< alt, option */
    TXINP_KEY_LGUI = 227, /**< windows, command (apple), meta */
    TXINP_KEY_RCTRL = 228,
    TXINP_KEY_RSHIFT = 229,
    TXINP_KEY_RALT = 230, /**< alt gr, option */
    TXINP_KEY_RGUI = 231, /**< windows, command (apple), meta */

    TXINP_KEY_MODE = 257, /**< I'm not sure if this is really not covered
                           *   by any of the above, but since there's a
                           *   special KMOD_MODE for it I'm adding it here
                           */

    /* @} */ /* Usage page 0x07 */

    /**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    TXINP_KEY_AUDIONEXT = 258,
    TXINP_KEY_AUDIOPREV = 259,
    TXINP_KEY_AUDIOSTOP = 260,
    TXINP_KEY_AUDIOPLAY = 261,
    TXINP_KEY_AUDIOMUTE = 262,
    TXINP_KEY_MEDIASELECT = 263,
    TXINP_KEY_WWW = 264,
    TXINP_KEY_MAIL = 265,
    TXINP_KEY_CALCULATOR = 266,
    TXINP_KEY_COMPUTER = 267,
    TXINP_KEY_AC_SEARCH = 268,
    TXINP_KEY_AC_HOME = 269,
    TXINP_KEY_AC_BACK = 270,
    TXINP_KEY_AC_FORWARD = 271,
    TXINP_KEY_AC_STOP = 272,
    TXINP_KEY_AC_REFRESH = 273,
    TXINP_KEY_AC_BOOKMARKS = 274,

    /* @} */ /* Usage page 0x0C */

    /**
     *  \name Walther keys
     *
     *  These are values that Christian Walther added (for mac keyboard?).
     */
    /* @{ */

    TXINP_KEY_BRIGHTNESSDOWN = 275,
    TXINP_KEY_BRIGHTNESSUP = 276,
    TXINP_KEY_DISPLAYSWITCH = 277, /**< display mirroring/dual display
                                           switch, video mode switch */
    TXINP_KEY_KBDILLUMTOGGLE = 278,
    TXINP_KEY_KBDILLUMDOWN = 279,
    TXINP_KEY_KBDILLUMUP = 280,
    TXINP_KEY_EJECT = 281,
    TXINP_KEY_SLEEP = 282,

    TXINP_KEY_APP1 = 283,
    TXINP_KEY_APP2 = 284,

    /* @} */ /* Walther keys */

    /**
     *  \name Usage page 0x0C (additional media keys)
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    TXINP_KEY_AUDIOREWIND = 285,
    TXINP_KEY_AUDIOFASTFORWARD = 286,

    /* @} */ /* Usage page 0x0C (additional media keys) */

    /* Add any other keys here. */

    TXINP_KEY_COUNT = 512 /**< not a key, just marks the number of scancodes
                                 for array bounds */
} txinp_key;

typedef enum txinp_mod {
    TXINP_MOD_NONE = 0,
    TXINP_MOD_CTRL = 0x1,
    TXINP_MOD_SHIFT = 0x2,
    TXINP_MOD_ALT = 0x4,
    TXINP_MOD_SUPER = 0x8,
} txinp_mod;

const char* txinp_mod_names[];
const int txinp_mod_lens[];

typedef struct txinp_event_key {
    txinp_key key;
    bool is_down;
} txinp_event_key;

void txinp_init();
void txinp_update();
void txinp_on_key_event(txinp_event_key key_event);

bool txinp_get_key(txinp_key key);
bool txinp_get_key_down(txinp_key key);
bool txinp_get_key_up(txinp_key key);
txinp_mod txinp_mods_down(txinp_mod mod);

// key_name is a char* for now because I don't feel like writing a converter yet
// snprintf style s and n parameters and return value
int32_t txinp_write_mod_strn(txinp_mod mod, const char* key_name, char* s, size_t n);

#if defined(TX_INPUT_IMPLEMENTATION)

#define TXINP_VALID_KEY(key) (((uint32_t)(key)) < TXINP_KEY_COUNT)

#include <string.h>

enum txinp_state_frame {
    TXINP_CURR,
    TXINP_PREV,
    TXINP_COUNT,
};

struct txinp_state {
    uint8_t keys[TXINP_KEY_COUNT];
    txinp_mod mod;
} txinp_state[TXINP_COUNT];

void txinp_init(void)
{
    memset(&txinp_state, 0, sizeof(txinp_state));
}

void txinp_update(void)
{
    memcpy(&txinp_state[TXINP_PREV], &txinp_state[TXINP_CURR], sizeof(txinp_state[0]));
}

txinp_mod mod_from_key(uint8_t key)
{
    if (((key ^ 0b11100000) & 0b11111000) != 0) {
        return TXINP_MOD_NONE;
    }

    txinp_mod mods[] = {
        TXINP_MOD_CTRL,
        TXINP_MOD_SHIFT,
        TXINP_MOD_ALT,
        TXINP_MOD_SUPER,
        TXINP_MOD_CTRL,
        TXINP_MOD_SHIFT,
        TXINP_MOD_ALT,
        TXINP_MOD_SUPER,
    };

    return mods[key & 0x7];
}

const char* txinp_mod_names[] = {"", "ctrl+", "shift+", "alt+", "super+"};
const int txinp_mod_lens[] = {0, 5, 6, 4, 6};

void txinp_on_key_event(txinp_event_key key_event)
{
    uint8_t key = key_event.key;

    TX_ASSERT(TXINP_VALID_KEY(key));

    txinp_state[TXINP_CURR].keys[key] = (key_event.is_down) ? 1 : 0;

    txinp_mod mod = mod_from_key(key);
    if (mod != TXINP_MOD_NONE) {
        if (key_event.is_down) {
            txinp_state[TXINP_CURR].mod |= mod;
        } else {
            txinp_state[TXINP_CURR].mod &= ~mod;
        }
    }
}

bool txinp_get_key(txinp_key key)
{
    TX_ASSERT(TXINP_VALID_KEY(key));
    return txinp_state[TXINP_CURR].keys[key];
}

bool txinp_get_key_down(txinp_key key)
{
    TX_ASSERT(TXINP_VALID_KEY(key));
    return txinp_state[TXINP_CURR].keys[key] && !txinp_state[TXINP_PREV].keys[key];
}

bool txinp_get_key_up(txinp_key key)
{
    TX_ASSERT(TXINP_VALID_KEY(key));
    return txinp_state[TXINP_PREV].keys[key] && !txinp_state[TXINP_CURR].keys[key];
}

int32_t txinp_write_mod_strn(txinp_mod mod, const char* key_name, char* s, size_t n)
{
    TX_ASSERT(key_name);
    TX_ASSERT(s);

    int32_t needed = 0;
    for (int i = 0; i < 4; ++i) {
        if ((mod & (1 << i)) != 0) {
            needed += txinp_mod_lens[i + 1];
        }
    }
    needed += (int32_t)strlen(key_name);

    if (needed >= n) {
        return needed;
    }

    char* cur = s;
    for (int i = 0; i < 4; ++i) {
        if ((mod & (1 << i)) != 0) {
            cur = strcpy(cur, txinp_mod_names[i + 1]) + txinp_mod_lens[i + 1];
        }
    }
    strcpy(cur, key_name);

    return needed;
}

txinp_mod txinp_mods_down(txinp_mod mod)
{
    return (txinp_state[TXINP_CURR].mod & mod) == mod;
}

strhash txinp_shortcut_strhash(txinp_mod mod, const char* key)
{
    char* buf = NULL;
    int32_t need = txinp_write_mod_strn(mod, key, buf, 0);
    arrsetlen(buf, need + 1);
    memset(buf, 0, (size_t)need);
    txinp_write_mod_strn(mod, key, buf, need + 1);
    strhash ret = strhash_get(buf);
    arrfree(buf);
    return ret;
}

#endif