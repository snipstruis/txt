int tessellate(stbtt__point** out, const stbtt_fontinfo *info, int glyph, int scale_x, int scale_y) {
    stbtt_vertex* vertices;
    int num_verts = stbtt_GetGlyphShape(info, glyph, &vertices);
    
    float flatness_in_pixels = 0.35f;
    float scale = scale_x > scale_y ? scale_y : scale_x;
    int winding_count, *winding_lengths;
    stbtt__point* windings =
        stbtt_FlattenCurves(vertices, num_verts, flatness_in_pixels / scale,
                            &winding_lengths, &winding_count, info->userdata);
    *out = windings;
    return winding_count;
};
