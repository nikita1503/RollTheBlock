#include "header.h"
#include"tile_info.h"
using namespace std;
tile_infoS tile_info[6];
int tile_for_level[3][11][11];
void initalise_tile_for_levels()
{
    //normal tile
    tile_info[1].color_base[0]=1.0;
    tile_info[1].color_base[1]=0.65;
    tile_info[1].color_base[2]=0.3;

    tile_info[1].color_top[0]=1.0;
    tile_info[1].color_top[1]=0.65;
    tile_info[1].color_top[2]=0.0;

    tile_info[1].color_side[0]=1.0;
    tile_info[1].color_side[1]=0.32;
    tile_info[1].color_side[2]=0.10;

    //2 is success square

    //fragile tile
    tile_info[3].color_base[0]=0.37;
    tile_info[3].color_base[1]=0.25;
    tile_info[3].color_base[2]=0.12;

    tile_info[3].color_top[0]=1.0;
    tile_info[3].color_top[1]=0.2;
    tile_info[3].color_top[2]=0.0;

    tile_info[3].color_side[0]=0.6;
    tile_info[3].color_side[1]=0.12;
    tile_info[3].color_side[2]=0.0;

    //bridge button
    tile_info[4].color_base[0]=0.0;
    tile_info[4].color_base[1]=1.0;
    tile_info[4].color_base[2]=0.0;

    tile_info[4].color_top[0]=0.0;
    tile_info[4].color_top[1]=1.0;
    tile_info[4].color_top[2]=0.0;

    tile_info[4].color_side[0]=0.0;
    tile_info[4].color_side[1]=0.0;
    tile_info[4].color_side[2]=0.0;

    //bridge
    tile_info[5].color_base[0]=0.0;
    tile_info[5].color_base[1]=0.0;
    tile_info[5].color_base[2]=1.0;

    tile_info[5].color_top[0]=0.0;
    tile_info[5].color_top[1]=0.0;
    tile_info[5].color_top[2]=1.0;

    tile_info[5].color_side[0]=0.0;
    tile_info[5].color_side[1]=0.0;
    tile_info[5].color_side[2]=0.0;


    //LEVEL 1
    tile_for_level[1][1][1]=1;
    tile_for_level[1][1][2]=1;
    tile_for_level[1][1][3]=1;
    tile_for_level[1][1][4]=1;
    tile_for_level[1][1][5]=0;
    tile_for_level[1][1][6]=1;
    tile_for_level[1][1][7]=1;
    tile_for_level[1][1][8]=1;
    tile_for_level[1][1][9]=1;
    tile_for_level[1][1][10]=1;

    tile_for_level[1][2][1]=1;
    tile_for_level[1][2][2]=1;
    tile_for_level[1][2][3]=4;
    tile_for_level[1][2][4]=1;
    tile_for_level[1][2][5]=0;
    tile_for_level[1][2][6]=3;
    tile_for_level[1][2][7]=1;
    tile_for_level[1][2][8]=1;
    tile_for_level[1][2][9]=1;
    tile_for_level[1][2][10]=1;

    tile_for_level[1][3][1]=1;
    tile_for_level[1][3][2]=1;
    tile_for_level[1][3][3]=3;
    tile_for_level[1][3][4]=0;
    tile_for_level[1][3][5]=1;
    tile_for_level[1][3][6]=1;
    tile_for_level[1][3][7]=1;
    tile_for_level[1][3][8]=1;
    tile_for_level[1][3][9]=1;
    tile_for_level[1][3][10]=1;

    tile_for_level[1][4][1]=1;
    tile_for_level[1][4][2]=1;
    tile_for_level[1][4][3]=1;
    tile_for_level[1][4][4]=0;
    tile_for_level[1][4][5]=1;
    tile_for_level[1][4][6]=1;
    tile_for_level[1][4][7]=1;
    tile_for_level[1][4][8]=1;
    tile_for_level[1][4][9]=1;
    tile_for_level[1][4][10]=1;

    tile_for_level[1][5][1]=0;
    tile_for_level[1][5][2]=5;
    tile_for_level[1][5][3]=0;
    tile_for_level[1][5][4]=0;
    tile_for_level[1][5][5]=0;
    tile_for_level[1][5][6]=0;
    tile_for_level[1][5][7]=0;
    tile_for_level[1][5][8]=0;
    tile_for_level[1][5][9]=0;
    tile_for_level[1][5][10]=1;

    tile_for_level[1][6][1]=0;
    tile_for_level[1][6][2]=5;
    tile_for_level[1][6][3]=0;
    tile_for_level[1][6][4]=0;
    tile_for_level[1][6][5]=0;
    tile_for_level[1][6][6]=0;
    tile_for_level[1][6][7]=0;
    tile_for_level[1][6][8]=0;
    tile_for_level[1][6][9]=0;
    tile_for_level[1][6][10]=1;

    tile_for_level[1][7][1]=1;
    tile_for_level[1][7][2]=1;
    tile_for_level[1][7][3]=1;
    tile_for_level[1][7][4]=0;
    tile_for_level[1][7][5]=0;
    tile_for_level[1][7][6]=1;
    tile_for_level[1][7][7]=1;
    tile_for_level[1][7][8]=1;
    tile_for_level[1][7][9]=1;
    tile_for_level[1][7][10]=0;

    tile_for_level[1][8][1]=1;
    tile_for_level[1][8][2]=1;
    tile_for_level[1][8][3]=1;
    tile_for_level[1][8][4]=0;
    tile_for_level[1][8][5]=3;
    tile_for_level[1][8][6]=1;
    tile_for_level[1][8][7]=1;
    tile_for_level[1][8][8]=2;
    tile_for_level[1][8][9]=1;
    tile_for_level[1][8][10]=0;

    tile_for_level[1][9][1]=1;
    tile_for_level[1][9][2]=1;
    tile_for_level[1][9][3]=1;
    tile_for_level[1][9][4]=1;
    tile_for_level[1][9][5]=0;
    tile_for_level[1][9][6]=0;
    tile_for_level[1][9][7]=1;
    tile_for_level[1][9][8]=1;
    tile_for_level[1][9][9]=1;
    tile_for_level[1][9][10]=0;

    tile_for_level[1][10][1]=1;
    tile_for_level[1][10][2]=1;
    tile_for_level[1][10][3]=1;
    tile_for_level[1][10][4]=0;
    tile_for_level[1][10][5]=0;
    tile_for_level[1][10][6]=0;
    tile_for_level[1][10][7]=1;
    tile_for_level[1][10][8]=1;
    tile_for_level[1][10][9]=1;
    tile_for_level[1][10][10]=0;

    //LEVEL 2
    tile_for_level[2][1][1]=0;
    tile_for_level[2][1][2]=0;
    tile_for_level[2][1][3]=0;
    tile_for_level[2][1][4]=1;
    tile_for_level[2][1][5]=1;
    tile_for_level[2][1][6]=1;
    tile_for_level[2][1][7]=1;
    tile_for_level[2][1][8]=1;
    tile_for_level[2][1][9]=1;
    tile_for_level[2][1][10]=1;

    tile_for_level[2][2][1]=0;
    tile_for_level[2][2][2]=0;
    tile_for_level[2][2][3]=1;
    tile_for_level[2][2][4]=1;
    tile_for_level[2][2][5]=1;
    tile_for_level[2][2][6]=1;
    tile_for_level[2][2][7]=1;
    tile_for_level[2][2][8]=1;
    tile_for_level[2][2][9]=1;
    tile_for_level[2][2][10]=1;

    tile_for_level[2][3][1]=1;
    tile_for_level[2][3][2]=1;
    tile_for_level[2][3][3]=1;
    tile_for_level[2][3][4]=0;
    tile_for_level[2][3][5]=0;
    tile_for_level[2][3][6]=3;
    tile_for_level[2][3][7]=0;
    tile_for_level[2][3][8]=0;
    tile_for_level[2][3][9]=0;
    tile_for_level[2][3][10]=1;

    tile_for_level[2][4][1]=1;
    tile_for_level[2][4][2]=1;
    tile_for_level[2][4][3]=1;
    tile_for_level[2][4][4]=0;
    tile_for_level[2][4][5]=0;
    tile_for_level[2][4][6]=3;
    tile_for_level[2][4][7]=0;
    tile_for_level[2][4][8]=0;
    tile_for_level[2][4][9]=0;
    tile_for_level[2][4][10]=3;

    tile_for_level[2][5][1]=1;
    tile_for_level[2][5][2]=3;
    tile_for_level[2][5][3]=3;
    tile_for_level[2][5][4]=3;
    tile_for_level[2][5][5]=3;
    tile_for_level[2][5][6]=3;
    tile_for_level[2][5][7]=0;
    tile_for_level[2][5][8]=0;
    tile_for_level[2][5][9]=0;
    tile_for_level[2][5][10]=1;

    tile_for_level[2][6][1]=0;
    tile_for_level[2][6][2]=3;
    tile_for_level[2][6][3]=3;
    tile_for_level[2][6][4]=3;
    tile_for_level[2][6][5]=3;
    tile_for_level[2][6][6]=3;
    tile_for_level[2][6][7]=0;
    tile_for_level[2][6][8]=0;
    tile_for_level[2][6][9]=0;
    tile_for_level[2][6][10]=1;

    tile_for_level[2][7][1]=0;
    tile_for_level[2][7][2]=3;
    tile_for_level[2][7][3]=1;
    tile_for_level[2][7][4]=3;
    tile_for_level[2][7][5]=2;
    tile_for_level[2][7][6]=1;
    tile_for_level[2][7][7]=1;
    tile_for_level[2][7][8]=0;
    tile_for_level[2][7][9]=0;
    tile_for_level[2][7][10]=3;

    tile_for_level[2][8][1]=0;
    tile_for_level[2][8][2]=3;
    tile_for_level[2][8][3]=3;
    tile_for_level[2][8][4]=3;
    tile_for_level[2][8][5]=0;
    tile_for_level[2][8][6]=0;
    tile_for_level[2][8][7]=0;
    tile_for_level[2][8][8]=0;
    tile_for_level[2][8][9]=1;
    tile_for_level[2][8][10]=1;

    tile_for_level[2][9][1]=0;
    tile_for_level[2][9][2]=0;
    tile_for_level[2][9][3]=1;
    tile_for_level[2][9][4]=1;
    tile_for_level[2][9][5]=1;
    tile_for_level[2][9][6]=1;
    tile_for_level[2][9][7]=1;
    tile_for_level[2][9][8]=1;
    tile_for_level[2][9][9]=1;
    tile_for_level[2][9][10]=1;

    tile_for_level[2][10][1]=0;
    tile_for_level[2][10][2]=0;
    tile_for_level[2][10][3]=0;
    tile_for_level[2][10][4]=1;
    tile_for_level[2][10][5]=1;
    tile_for_level[2][10][6]=1;
    tile_for_level[2][10][7]=1;
    tile_for_level[2][10][8]=1;
    tile_for_level[2][10][9]=1;
    tile_for_level[2][10][10]=1;


}
