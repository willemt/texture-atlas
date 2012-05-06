/*
 * =====================================================================================
 *
 *       Filename:  r_texture_atlas.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/27/11 19:52:56
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "tea_vec.h"
#include "linked_list_hashmap.h"
#include "fixed_arraylist.h"

static arraylistf_t *__atlases = NULL;

typedef struct texture_s texture_t;

//static char *str = "found %d %d %d %d %s \n";

struct texture_s
{
    texture_t *kids[2];
    rect_t rect;
    int id;
};

typedef struct
{
    int glimage;
    hashmap_t *textures;
    texture_t *root;
    int ntextures;

    void (
    *write_pixels_to_texture_cb
    )    (
    const void *pixels,
    const rect_t * rect,
    const unsigned int texture
    );

    int (
    *create_texture_cb
    )   (
    const int w,
    const int h
    );
} atlas_t;

static atlas_t *__get_atlas_from_id(
    int id
)
{
    return (atlas_t *) arraylistf_get(__atlases, id);
}

static unsigned long __ulong_hash(
    const void *e1
)
{
    const long i1 = (unsigned long) e1;

    assert(i1 >= 0);
    return i1;
}

static long __ulong_compare(
    const void *e1,
    const void *e2
)
{
    const long i1 = (unsigned long) e1, i2 = (unsigned long) e2;

//      return !(*i1 == *i2); 
    return i1 - i2;
}

/**
 * @param width The size of this atlas
 * @param write_pixels_to_texture Callback for writing pixels to the atlas
 * @param create_texture_cb Callback for creating a new texture */
int ren_texture_atlas_init(
    int width,
    void (*write_pixels_to_texture) (const void *pixels,
                                     const rect_t * rect,
                                     const unsigned int texture),
    int (*create_texture_cb) (const int w,
                              const int h)
)
{
    atlas_t *at;

    texture_t *tex;

    if (!__atlases)
    {
        __atlases = arraylistf_new();
    }

    at = calloc(1, sizeof(atlas_t));
    tex = at->root = calloc(1, sizeof(texture_t));
    tex->rect.x = 0;
    tex->rect.y = 0;
    tex->rect.w = width;        //1 << 8;
    tex->rect.h = width;        //1 << 8;
    at->textures = hashmap_new(__ulong_hash, __ulong_compare);
    at->write_pixels_to_texture_cb = write_pixels_to_texture;
    at->create_texture_cb = create_texture_cb;
    if (at->create_texture_cb)
    {
        at->glimage = at->create_texture_cb(tex->rect.w, tex->rect.h);
    }
    return arraylistf_add(__atlases, at);
}

/*----------------------------------------------------------------------------*/

#if 0
static void __print(
    texture_t * tex,
    int depth
)
{
    if (!tex)
        return;
    int ii;

    for (ii = 0; ii < depth; ii++)
        printf(" ");
    printf
        ("%d,%d,%d,%d %d\n",
         tex->rect.x, tex->rect.y, tex->rect.w, tex->rect.h, tex->id);
    __print(tex->kids[0], depth + 1);
    __print(tex->kids[1], depth + 1);
}
#endif

static texture_t *__insert(
    texture_t * tex,
    int w,
    int h,
    int id
)
{
    assert(tex);

    /* not leaf */
    if (tex->kids[0])
    {
        texture_t *new;

        if ((new = __insert(tex->kids[0], w, h, id)))
        {
            return new;
        }
        else
        {
            return __insert(tex->kids[1], w, h, id);
        }
    }
    else
    {
        /*  fits perfectly */
        if (tex->rect.w == w && tex->rect.h == h)
        {
            tex->id = id;
            assert(!tex->kids[0]);
            assert(!tex->kids[1]);
            return tex;
        }
        /*  doesn't fit */
        else if (tex->rect.w < w || tex->rect.h < h || tex->id != 0)
        {
//            printf("doesn't fit, %d %d %d %d\n", tex->rect.w, w, tex->rect.h, h);
//            assert(0);
            return NULL;
        }
        else
        {
            assert(!tex->kids[0]);
            assert(!tex->kids[1]);
            tex->kids[0] = calloc(1, sizeof(texture_t));
            tex->kids[1] = calloc(1, sizeof(texture_t));
            int dw, dh;

            dw = tex->rect.w - w;
            dh = tex->rect.h - h;
            if (dh < dw)
            {
                tex->kids[0]->rect.x = tex->rect.x;
                tex->kids[0]->rect.y = tex->rect.y;
                tex->kids[0]->rect.w = w;
                tex->kids[0]->rect.h = tex->rect.h;
                tex->kids[1]->rect.x = tex->rect.x + w;
                tex->kids[1]->rect.y = tex->rect.y;
                tex->kids[1]->rect.w = tex->rect.w - w;
                tex->kids[1]->rect.h = tex->rect.h;
            }
            else
            {
                tex->kids[0]->rect.x = tex->rect.x;
                tex->kids[0]->rect.y = tex->rect.y;
                tex->kids[0]->rect.w = tex->rect.w;
                tex->kids[0]->rect.h = h;
                tex->kids[1]->rect.x = tex->rect.x;
                tex->kids[1]->rect.y = tex->rect.y + h;
                tex->kids[1]->rect.w = tex->rect.w;
                tex->kids[1]->rect.h = tex->rect.h - h;
            }

            return __insert(tex->kids[0], w, h, id);
        }
    }
}

/**
 * Write pixels to atlas, return the 'virtual texture id'
 * @return 'virtual texture id'
 */
int ren_texture_atlas_push_pixels(
    int id,
    const void *pixel_data,
    int w,
    int h
)
{
    texture_t *tex;

    atlas_t *at;

    unsigned long new_id;

    at = __get_atlas_from_id(id);

    assert(at);

    new_id = at->ntextures + 1;
//      __print(at->root,0);

    if (!(tex = __insert(at->root, w, h, new_id)))
    {
//        printf("(%d, %d) \n", w, h);
//        printf("(%d, %d) \n", at->root->rect.w, at->root->rect.h);
        assert(false);
        return 0;
    }

    hashmap_put(at->textures, (void *) new_id, (void *) tex);

//    printf(str, tex->rect.x, tex->rect.y, tex->rect.w, tex->rect.h, fname);

    if (at->write_pixels_to_texture_cb)
    {
        at->write_pixels_to_texture_cb(pixel_data, &tex->rect, at->glimage);
    }

    at->ntextures += 1;

    return new_id;
}

/**
 * Remove texture from atlas using filename
 */
void ren_texture_atlas_remove_file(
    int id,
    const char *fname
)
{
    atlas_t *at;

    at = __get_atlas_from_id(id);

    assert(false);
}

/**
 * Remove texture based off texture ID */
void ren_texture_atlas_remove_texid(
    int id,
    const unsigned long texid
)
{
    assert(false);
}

/**
 * @return true if atlas contains texture id, otherwise false */
int ren_texture_atlas_contains_texid(
    int id,
    const unsigned long texid
)
{
    atlas_t *at;

    texture_t *tex;

    at = __get_atlas_from_id(id);
    return NULL != hashmap_get(at->textures, (void *) texid);
}

/**
 * Get coordiantes using texture id
 */
int ren_texture_atlas_get_coords_from_texid(
    const int id,
    const unsigned long texid,
    vec2_t begin,
    vec2_t end
)
{
    atlas_t *at;

    texture_t *tex;

    assert(ren_texture_atlas_contains_texid(id, texid));

    at = __get_atlas_from_id(id);

    tex = hashmap_get(at->textures, (void *) texid);

    vec2Set(begin, (float) tex->rect.x / at->root->rect.h,
            (float) tex->rect.y / at->root->rect.h);
    vec2Copy(begin, end);
    end[0] += (float) tex->rect.w / at->root->rect.w;
    end[1] += (float) tex->rect.h / at->root->rect.h;
    return 0;
}

/**
 * @return texture id */
int ren_texture_atlas_get_texture(
    const int id
)
{
    atlas_t *at;

    at = __get_atlas_from_id(id);

    return at->glimage;
}

/**
 * @return number of textures */
int ren_texture_atlas_get_ntextures(
    const int id
)
{
    atlas_t *at;

    at = __get_atlas_from_id(id);

    return at->ntextures;
}
