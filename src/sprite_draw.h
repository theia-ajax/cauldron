#pragma once

#include "sokol_gfx.h"
#include "tx_math.h"
#include "tx_types.h"

void spr_init();
void spr_shutdown();
void spr_render(int width, int height);
void spr_draw(vec3 pos);