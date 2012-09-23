#include <assert.h>
#include <setjmp.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "tea_vec.h"
#include "texture_atlas.h"

#include "CuTest.h"



#define WIDTH 512

void TestTextureAtlas_PushImage(
    CuTest * tc
)
{
    void* at;
    int texid;

    at = ren_texture_atlas_init(WIDTH, NULL, NULL);
    CuAssertTrue(tc, 0 == ren_texture_atlas_get_ntextures(at));

    ren_texture_atlas_push_pixels(at, NULL, 32, 32);
    CuAssertTrue(tc, 1 == ren_texture_atlas_get_ntextures(at));
}

void TestTextureAtlas_ContainsTexid(
    CuTest * tc
)
{
    void* at;
    int texid = 0;

    at = ren_texture_atlas_init(WIDTH, NULL, NULL);
    CuAssertTrue(tc, !ren_texture_atlas_contains_texid(at, texid));

    texid = ren_texture_atlas_push_pixels(at, NULL, 32, 32);
    CuAssertTrue(tc, ren_texture_atlas_contains_texid(at, texid));
}

void TestTextureAtlas_PushImageNonZeroTexID(
    CuTest * tc
)
{
    void* at;
    int texid;

    at = ren_texture_atlas_init(WIDTH, NULL, NULL);

    texid = ren_texture_atlas_push_pixels(at, NULL, 32, 32);
    CuAssertTrue(tc, 0 < texid);
}

void TestTextureAtlas_PushImageGetImage(
    CuTest * tc
)
{
    void* at;
    int texid;

    vec2_t offset, extent;

    at = ren_texture_atlas_init(WIDTH, NULL, NULL);

    texid = ren_texture_atlas_push_pixels(at, NULL, 32, 32);
    ren_texture_atlas_get_coords_from_texid(at, texid, offset, extent);
    CuAssertTrue(tc, offset[0] == 0.0f);
    CuAssertTrue(tc, offset[1] == 0.0f);
    CuAssertTrue(tc, extent[0] == 32.0f / 512);
    CuAssertTrue(tc, extent[1] == 32.0f / 512);
}

void TestTextureAtlas_PushImageGetImage2(
    CuTest * tc
)
{
    void* at;
    int texid;

    vec2_t offset, extent;

    at = ren_texture_atlas_init(WIDTH, NULL, NULL);

    ren_texture_atlas_push_pixels(at, NULL, 32, 32);
    texid = ren_texture_atlas_push_pixels(at, NULL, 64, 32);
    ren_texture_atlas_get_coords_from_texid(at, texid, offset, extent);
    CuAssertTrue(tc, offset[0] == 32.0f / 512);
    CuAssertTrue(tc, offset[1] == 0.0f / 512);
    CuAssertTrue(tc, extent[0] == 96.0f / 512);
    CuAssertTrue(tc, extent[1] == 32.0f / 512);
}
