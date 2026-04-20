#pragma once

constexpr int screenWidth = 240;
constexpr int screenHeight = 180;
constexpr int scale = 4;
constexpr int numTexures = 1;

extern int numSect;
extern int numWalls;

typedef struct {
    float cos[360];
    float sin[360];
}math; 
extern math M;

typedef struct {
    int x, y, z;
    int a; //   Rotation angle
    int l; //   Look up and down
}player; 
extern player P;

typedef struct {
    int x1,y1; //bottom line point 1
    int x2,y2; //bottom line point 2
    Color c;   //wall color
    int wt,u,v; //wall texture and u/v tile
    int shade; //shade of the wall
}walls; 
extern walls W[256];

typedef struct {
    int ws,we;      //wall number start and end
    int z1,z2;      //height of bottom and top 
    int d;          //add y distances to sort drawing order
    Color c1,c2;     //bottom and top color
    int st,ss;      //surface texture, surface scale 
    int surfBottom[screenWidth]; // Bottom limits
    int surfTop[screenWidth]; // Top limits
    int surface;   //is there a surface to draw
}sectors; 
extern sectors S[128];


typedef struct {
    int w,h;                          //texture width/height
    const unsigned char *name;        //texture name
}TextureMaps; 
extern TextureMaps Textures[64]; //increase for more textures

extern Texture2D texturesRaylib[64];