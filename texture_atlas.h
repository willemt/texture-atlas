#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

void* ren_texture_atlas_init(
    int width,
    void (*write_pixels_to_texture) (const void *pixels,
                                     const rect_t * rect,
                                     const unsigned int texture),
    int (*create_texture_cb) (const int w,
                              const int h)
);

int ren_texture_atlas_push_pixels(
    void* att,
    const void *pixel_data,
    int w,
    int h
);

void ren_texture_atlas_remove_file(
    void* att,
    const char *fname
);

void ren_texture_atlas_remove_texid(
    void* att,
    const unsigned long texid
);

int ren_texture_atlas_contains_texid(
    const void* att,
    const unsigned long texid
);

void ren_texture_atlas_get_coords_from_texid(
    const void* att,
    const unsigned long texid,
    vec2_t begin,
    vec2_t end
);

int ren_texture_atlas_get_texture(
    const void* att
);

int ren_texture_atlas_get_ntextures(
    const void* att
);

#endif
