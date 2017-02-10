typedef struct tile_infoS {
    static const float width=0.5,height=0.5;
    float color_base[3];
    float color_top[3];
    float color_side[3];
}tile_infoS;
extern tile_infoS tile_info[3];


extern int tile_for_level[3][11][11];

void initalise_tile_for_levels();
