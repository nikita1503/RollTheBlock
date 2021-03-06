#include <iostream>
#include "tile_info.h"

TileDetailsS TileDetails[3];
int TileArrange[3][11][11];
void initaliseTile()
{

    TileDetails[1].color_base[0]=1.0;
    TileDetails[1].color_base[1]=1.0;
    TileDetails[1].color_base[2]=1.0;

    TileDetails[1].color_top[0]=0.0;
    TileDetails[1].color_top[1]=1.0;
    TileDetails[1].color_top[2]=1.0;

    TileDetails[1].color_side[0]=1.0;
    TileDetails[1].color_side[1]=0.0;
    TileDetails[1].color_side[2]=1.0;


    //LEVEL 1
    TileArrange[1][1][1]=1;
    TileArrange[1][1][2]=1;
    TileArrange[1][1][3]=1;
    TileArrange[1][1][4]=1;
    TileArrange[1][1][5]=2;
    TileArrange[1][1][6]=1;
    TileArrange[1][1][7]=1;
    TileArrange[1][1][8]=1;
    TileArrange[1][1][9]=1;
    TileArrange[1][1][10]=1;

    TileArrange[1][2][1]=1;
    TileArrange[1][2][2]=1;
    TileArrange[1][2][3]=1;
    TileArrange[1][2][4]=1;
    TileArrange[1][2][5]=1;
    TileArrange[1][2][6]=1;
    TileArrange[1][2][7]=1;
    TileArrange[1][2][8]=1;
    TileArrange[1][2][9]=1;
    TileArrange[1][2][10]=1;

    TileArrange[1][3][1]=0;
    TileArrange[1][3][2]=0;
    TileArrange[1][3][3]=1;
    TileArrange[1][3][4]=1;
    TileArrange[1][3][5]=1;
    TileArrange[1][3][6]=1;
    TileArrange[1][3][7]=1;
    TileArrange[1][3][8]=1;
    TileArrange[1][3][9]=0;
    TileArrange[1][3][10]=1;

    TileArrange[1][4][1]=0;
    TileArrange[1][4][2]=0;
    TileArrange[1][4][3]=1;
    TileArrange[1][4][4]=1;
    TileArrange[1][4][5]=1;
    TileArrange[1][4][6]=1;
    TileArrange[1][4][7]=1;
    TileArrange[1][4][8]=1;
    TileArrange[1][4][9]=0;
    TileArrange[1][4][10]=1;

    TileArrange[1][5][1]=1;
    TileArrange[1][5][2]=1;
    TileArrange[1][5][3]=1;
    TileArrange[1][5][4]=0;
    TileArrange[1][5][5]=0;
    TileArrange[1][5][6]=1;
    TileArrange[1][5][7]=1;
    TileArrange[1][5][8]=1;
    TileArrange[1][5][9]=0;
    TileArrange[1][5][10]=0;

    TileArrange[1][6][1]=1;
    TileArrange[1][6][2]=1;
    TileArrange[1][6][3]=1;
    TileArrange[1][6][4]=0;
    TileArrange[1][6][5]=0;
    TileArrange[1][6][6]=1;
    TileArrange[1][6][7]=1;
    TileArrange[1][6][8]=1;
    TileArrange[1][6][9]=0;
    TileArrange[1][6][10]=0;

    TileArrange[1][7][1]=1;
    TileArrange[1][7][2]=1;
    TileArrange[1][7][3]=1;
    TileArrange[1][7][4]=0;
    TileArrange[1][7][5]=0;
    TileArrange[1][7][6]=1;
    TileArrange[1][7][7]=1;
    TileArrange[1][7][8]=1;
    TileArrange[1][7][9]=0;
    TileArrange[1][7][10]=0;

    TileArrange[1][8][1]=1;
    TileArrange[1][8][2]=1;
    TileArrange[1][8][3]=1;
    TileArrange[1][8][4]=0;
    TileArrange[1][8][5]=0;
    TileArrange[1][8][6]=1;
    TileArrange[1][8][7]=1;
    TileArrange[1][8][8]=1;
    TileArrange[1][8][9]=0;
    TileArrange[1][8][10]=0;

    TileArrange[1][9][1]=1;
    TileArrange[1][9][2]=1;
    TileArrange[1][9][3]=1;
    TileArrange[1][9][4]=0;
    TileArrange[1][9][5]=0;
    TileArrange[1][9][6]=1;
    TileArrange[1][9][7]=1;
    TileArrange[1][9][8]=1;
    TileArrange[1][9][9]=0;
    TileArrange[1][9][10]=0;

    TileArrange[1][10][1]=1;
    TileArrange[1][10][2]=1;
    TileArrange[1][10][3]=1;
    TileArrange[1][10][4]=0;
    TileArrange[1][10][5]=0;
    TileArrange[1][10][6]=1;
    TileArrange[1][10][7]=1;
    TileArrange[1][10][8]=1;
    TileArrange[1][10][9]=0;
    TileArrange[1][10][10]=0;

    //LEVEL 2
    TileArrange[2][1][1]=0;
    TileArrange[2][1][2]=1;
    TileArrange[2][1][3]=1;
    TileArrange[2][1][4]=0;
    TileArrange[2][1][5]=0;
    TileArrange[2][1][6]=1;
    TileArrange[2][1][7]=1;
    TileArrange[2][1][8]=1;
    TileArrange[2][1][9]=1;
    TileArrange[2][1][10]=1;

    TileArrange[2][2][1]=1;
    TileArrange[2][2][2]=0;
    TileArrange[2][2][3]=1;
    TileArrange[2][2][4]=1;
    TileArrange[2][2][5]=0;
    TileArrange[2][2][6]=1;
    TileArrange[2][2][7]=1;
    TileArrange[2][2][8]=0;
    TileArrange[2][2][9]=1;
    TileArrange[2][2][10]=1;

    TileArrange[2][3][1]=0;
    TileArrange[2][3][2]=0;
    TileArrange[2][3][3]=1;
    TileArrange[2][3][4]=1;
    TileArrange[2][3][5]=1;
    TileArrange[2][3][6]=1;
    TileArrange[2][3][7]=1;
    TileArrange[2][3][8]=1;
    TileArrange[2][3][9]=1;
    TileArrange[2][3][10]=1;

    TileArrange[2][4][1]=0;
    TileArrange[2][4][2]=1;
    TileArrange[2][4][3]=1;
    TileArrange[2][4][4]=1;
    TileArrange[2][4][5]=1;
    TileArrange[2][4][6]=2;
    TileArrange[2][4][7]=1;
    TileArrange[2][4][8]=1;
    TileArrange[2][4][9]=0;
    TileArrange[2][4][10]=0;

    TileArrange[2][5][1]=1;
    TileArrange[2][5][2]=1;
    TileArrange[2][5][3]=1;
    TileArrange[2][5][4]=1;
    TileArrange[2][5][5]=1;
    TileArrange[2][5][6]=1;
    TileArrange[2][5][7]=0;
    TileArrange[2][5][8]=0;
    TileArrange[2][5][9]=1;
    TileArrange[2][5][10]=0;

    TileArrange[2][6][1]=1;
    TileArrange[2][6][2]=1;
    TileArrange[2][6][3]=1;
    TileArrange[2][6][4]=0;
    TileArrange[2][6][5]=0;
    TileArrange[2][6][6]=1;
    TileArrange[2][6][7]=1;
    TileArrange[2][6][8]=1;
    TileArrange[2][6][9]=0;
    TileArrange[2][6][10]=0;

    TileArrange[2][7][1]=1;
    TileArrange[2][7][2]=1;
    TileArrange[2][7][3]=1;
    TileArrange[2][7][4]=0;
    TileArrange[2][7][5]=0;
    TileArrange[2][7][6]=1;
    TileArrange[2][7][7]=1;
    TileArrange[2][7][8]=1;
    TileArrange[2][7][9]=0;
    TileArrange[2][7][10]=0;

    TileArrange[2][8][1]=1;
    TileArrange[2][8][2]=1;
    TileArrange[2][8][3]=1;
    TileArrange[2][8][4]=0;
    TileArrange[2][8][5]=0;
    TileArrange[2][8][6]=1;
    TileArrange[2][8][7]=1;
    TileArrange[2][8][8]=1;
    TileArrange[2][8][9]=0;
    TileArrange[2][8][10]=0;

    TileArrange[2][9][1]=1;
    TileArrange[2][9][2]=1;
    TileArrange[2][9][3]=1;
    TileArrange[2][9][4]=0;
    TileArrange[2][9][5]=0;
    TileArrange[2][9][6]=1;
    TileArrange[2][9][7]=1;
    TileArrange[2][9][8]=1;
    TileArrange[2][9][9]=0;
    TileArrange[2][9][10]=0;

    TileArrange[2][10][1]=1;
    TileArrange[2][10][2]=1;
    TileArrange[2][10][3]=1;
    TileArrange[2][10][4]=0;
    TileArrange[2][10][5]=0;
    TileArrange[2][10][6]=1;
    TileArrange[2][10][7]=1;
    TileArrange[2][10][8]=1;
    TileArrange[2][10][9]=0;
    TileArrange[2][10][10]=0;


}
