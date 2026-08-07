#ifndef RESOURCES_H
#define RESOURCES_H
#include "raylib.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define DECLARE_KEY_SOUND(x)                                                   \
  static const char *rkey_##audio_##x = "rkey_"                                \
                                        "sound_" #x
#define DECLARE_KEY_TEXTURE(x)                                                 \
  static const char *rkey_##texture_##x = "rkey_"                              \
                                          "texture_" #x
#define DECLARE_KEY_FONT(x)                                                    \
  static const char *rkey_##font_##x = "rkey_"                                 \
                                       "font_" #x

#define HT_TABLE_SIZE 101 // prime number reduces clustering (?)
#define HT_MAX_KEY_SIZE 32

typedef enum {
  RESOURCE_KIND_TEXTURE,
  RESOURCE_KIND_SOUND,
  RESOURCE_KIND_FONT
} ResourceKind;

typedef struct {
  ResourceKind kind;
  const char *key;

  union {
    struct {
      Texture texture;
    };
    struct {
      Sound sound;
    };
    struct {
      Font font;
    };
  } data;

} ResourceData;

typedef struct Resources_HT_Entry {
  char key[HT_MAX_KEY_SIZE];
  ResourceData *resource;
  struct Resources_HT_Entry *next;
} ResourceHashTableEntry;

typedef struct {
  ResourceKind rkind;
  const char *rpath;
  const char *rkey;
} ResourceParams;

#define HT_MAX_RESOURCES 128

typedef struct {
  ResourceHashTableEntry *buckets[HT_TABLE_SIZE];
  ResourceData pool[HT_MAX_RESOURCES];
  int pool_used;
} ResourceHashTable;

static unsigned long rkey_hash(const char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c (?)
  }

  return hash % HT_TABLE_SIZE;
}

static ResourceHashTable *resource_hashtable_create(void) {
  ResourceHashTable *ht = malloc(sizeof(ResourceHashTable));

  if (!ht) {
    return NULL;
  }

  memset(ht->buckets, 0, sizeof(ht->buckets));
  ht->pool_used = 0;
  return ht;
}

static void resource_hashtable_set_item(ResourceHashTable *ht,
                                        ResourceData *resource,
                                        const char *key) {
  unsigned long idx = rkey_hash(key);
  ResourceHashTableEntry *entry = ht->buckets[idx];

  /* Check if key already exists */
  while (entry != NULL) {
    if (strcmp(entry->key, key) == 0) {
      entry->resource = resource;
      return;
    }

    entry = entry->next;
  }

  ResourceHashTableEntry *new_entry = malloc(sizeof(ResourceHashTableEntry));

  strncpy(new_entry->key, key, HT_MAX_KEY_SIZE - 1);
  new_entry->key[HT_MAX_KEY_SIZE - 1] = '\0';
  new_entry->resource = resource;
  new_entry->next = ht->buckets[idx];
  ht->buckets[idx] = new_entry;
}

static ResourceData *resource_hashtable_get_item(ResourceHashTable *ht,
                                                 const char *key) {
  unsigned long idx = rkey_hash(key);
  ResourceHashTableEntry *entry = ht->buckets[idx];

  while (entry != NULL) {
    if (strcmp(entry->key, key) == 0) {
      return entry->resource;
    }

    entry = entry->next;
  }

  return NULL;
}

static void resource_hashtable_remove_item(ResourceHashTable *ht,
                                           ResourceData *resource) {
  unsigned long idx = rkey_hash(resource->key);
  ResourceHashTableEntry *entry = ht->buckets[idx];
  ResourceHashTableEntry *prev = NULL;

  while (entry != NULL) {
    if (strcmp(entry->key, resource->key) == 0) {
      /* Remove match and fill in the gap left in the linked list */

      /* first fill in the gap */
      if (prev == NULL) {
        ht->buckets[idx] = entry->next;
      } else {
        prev->next = entry->next;
      }

      /* Then remove entry */
      free(entry);

      return;
    }
    prev = entry;
    entry = entry->next;
  }
}

static void resource_hashtable_free(ResourceHashTable *ht) {
  for (int i = 0; i < HT_TABLE_SIZE; i++) {
    ResourceHashTableEntry *entry = ht->buckets[i];

    while (entry != NULL) {
      ResourceHashTableEntry *next = entry->next;
      free(entry);
      entry = next;
    }
  }

  free(ht);
}

static ResourceData *resource_data_create(ResourceHashTable *ht,
                                          ResourceParams params) {
  ResourceKind resource_kind = params.rkind;
  const char *resource_path = params.rpath;
  const char *resource_key = params.rkey;

  assert((ht->pool_used < HT_MAX_RESOURCES) && "Resource Pool Exhausted!!!");

  ResourceData *resource = &ht->pool[ht->pool_used++];
  resource->kind = resource_kind;
  // resource->key = resource_key;

  switch (resource_kind) {
  case RESOURCE_KIND_TEXTURE:
    resource->data.texture = LoadTexture(resource_path);
    break;
  case RESOURCE_KIND_SOUND:
    resource->data.sound = LoadSound(resource_path);
    break;
  case RESOURCE_KIND_FONT:
    resource->data.font = LoadFont(resource_path);
    break;
  }

  resource_hashtable_set_item(ht, resource, resource_key);

  return resource;
}

static void resource_data_unload(ResourceData *rdata) {
  switch (rdata->kind) {

  case RESOURCE_KIND_TEXTURE:
    UnloadTexture(rdata->data.texture);
    break;
  case RESOURCE_KIND_SOUND:
    UnloadSound(rdata->data.sound);
    break;
  case RESOURCE_KIND_FONT:
    UnloadFont(rdata->data.font);
    break;
  }
}

static void resource_data_unload_with_key(ResourceHashTable *ht, const char* rkey){
  ResourceData *rdata = resource_hashtable_get_item(ht, rkey);
  resource_data_unload(rdata);
}

#endif
/* vim:set ts=3 sw=2 sts=2 et: */
