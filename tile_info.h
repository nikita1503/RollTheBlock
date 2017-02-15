typedef struct TileDetailsS {
    static const float width=0.5,height=0.5;
    float color_base[3];
    float color_top[3];
    float color_side[3];
}TileDetailsS;
extern TileDetailsS TileDetails[3];


extern int TileArrange[3][11][11];

void initaliseTile();
