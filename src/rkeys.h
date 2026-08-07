#ifndef RKEYS_H
#define RKEYS_H
#include "./resources.h"

/* vvv [ TEXTURES ] vvv */

DECLARE_KEY_TEXTURE(chicken);
DECLARE_KEY_TEXTURE(egg);

/* ^^^ [ TEXTURES ] ^^^ */

/* vvv [ AUDIO ] vvv */
/* ^^^ [ AUDIO ] ^^^ */

/* vvv [ FONTS ] vvv */

DECLARE_KEY_FONT(open_dyslexic);

/* ^^^ [ FONTS ] ^^^ */


static void resource_data_unload_all(ResourceHashTable *ht){
/* vvv [ TEXTURES ] vvv */
  resource_data_unload_with_key(ht, rkey_texture_chicken);
  resource_data_unload_with_key(ht, rkey_texture_egg);
/* ^^^ [ TEXTURES ] ^^^ */

/* vvv [ AUDIO ] vvv */
/* ^^^ [ AUDIO ] ^^^ */

/* vvv [ FONTS ] vvv */
  resource_data_unload_with_key(ht, rkey_font_open_dyslexic);
/* ^^^ [ FONTS ] ^^^ */
}
#endif
/* vim:set ts=3 sw=2 sts=2 et: */
