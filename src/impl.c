#include "tx_types.h"

#include <GL/gl3w.h>

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_ASSERT TX_ASSERT
#include "sokol_gfx.h"

#define TX_MATH_IMPLEMENTATION
#include "tx_math.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define JSMN_PARENT_LINKS
#include "jsmn.h"