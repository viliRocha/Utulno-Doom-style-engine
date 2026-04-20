#include <raylib.h>
#include <math.h>
#include <stdio.h>

#include "../textures/T_00.h"
#include "../textures/T_VIEW2D.h"

#include "game.h"
#include "levelEditor.h"

typedef struct {
 int mx,my;        //rounded mouse position
 int addSect;      //0=nothing, 1=add sector
 int wt,wu,wv;     //wall    texture, uv texture tile
 int st,ss;        //surface texture, surface scale 
 int z1,z2;        //bottom and top height
 int scale;        //scale down grid
 int move[4];      //0=wall ID, 1=v1v2, 2=wallID, 3=v1v2
 int selS,selW;    //select sector/wall
}grid; grid G;

//save file
void save() {int w,s;
    FILE *fp = fopen("./level.h", "w");
    if (fp == NULL) { 
        printf("Error opening the file level.h"); 
        return;
    }
    //clear file 
    if (numSect==0) { 
        fclose(fp); 
        return;
    }

    fprintf(fp,"%i\n", numSect); //number of sectors
    //save sector
    for (s=0; s < numSect; s++) { 
        fprintf(fp,"%i %i %i %i %i %i\n", S[s].ws, S[s].we, S[s].z1, S[s].z2, S[s].st, S[s].ss); 
    }
 
    fprintf(fp,"%i\n", numWalls); //number of walls
    //save walls
    for(w=0; w<numWalls; w++) { 
        fprintf(fp,"%i %i %i %i %i %i %i %i\n", W[w].x1, W[w].y1, W[w].x2, W[w].y2, W[w].wt, W[w].u, W[w].v, W[w].shade);
    }
    fprintf(fp,"\n%i %i %i %i %i\n", P.x, P.y, P.z, P.a, P.l); //player position 
    fclose(fp);
}

//define grid globals
void initGlobals() {
    G.scale=4;                //scale down grid
    G.selS=0; G.selW=0;       //select sector, walls
    G.z1=0;   G.z2=40;        //sector bottom top height
    G.st=1;   G.ss=4;         //sector texture, scale
    G.wt=0;   G.wu=1; G.wv=1; //wall texture, u,v
}


void draw2D() {
    // fundo: em vez de iterar pixel a pixel, desenhe direto a textura
    DrawTextureEx(texturesRaylib[G.st], Vector2{0,0}, 0.0f, scale, WHITE);

    // desenhar setores e paredes
    for(int s=0; s<numSect; s++) {
        for(int w=S[s].ws; w<S[s].we; w++) {
            DrawLine(W[w].x1/scale, W[w].y1/scale, W[w].x2/scale, W[w].y2/scale, GRAY);
        }
    }

    // desenhar player
    int dx = M.sin[P.a]*12; 
    int dy = M.cos[P.a]*12; 
    DrawRectangle(P.x/scale, P.y/scale, scale, scale, GREEN); 
    DrawRectangle((P.x+dx)/scale, (P.y+dy)/scale, scale, scale, DARKGREEN);

    // preview da textura da parede
    Rectangle src = {0, 0, 15, 15};
    Vector2 pos = {145, 105};
    DrawTextureRec(texturesRaylib[G.wt], src, pos, WHITE);

    // preview da textura da superfície
    Rectangle src2 = {0, 0, 15, 15};
    Vector2 pos2 = {145, 80};
    DrawTextureRec(texturesRaylib[G.st], src2, pos2, WHITE);
    
    //draw numbers
    DrawText(TextFormat("%i", G.wu), 140, 90, 10, GREEN);   //wall u
    DrawText(TextFormat("%i", G.wv), 148, 90, 10, GREEN);   //wall v
    DrawText(TextFormat("%i", G.ss), 148, 66, 10, GREEN);   //surface v
    DrawText(TextFormat("%i", G.z2), 148, 58, 10, GREEN);   //top height
    DrawText(TextFormat("%i", G.z1), 148, 50, 10, GREEN);   //bottom height
    DrawText(TextFormat("%i", G.selS), 148, 26, 10, GREEN); //sector number
    DrawText(TextFormat("%i", G.selW), 148, 18, 10, GREEN); //wall number
}