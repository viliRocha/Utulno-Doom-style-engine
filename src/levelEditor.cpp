#include <raylib.h>
#include <math.h>
#include <stdio.h>

#include "../textures/T_00.h"
#include "../textures/T_VIEW2D.h"

constexpr int screenWidth = 240;
constexpr int screenHeight = 180;
constexpr int scale = 4;
constexpr int numTexures = 1;

int numSect = 4;
int numWalls = 16;

typedef struct {
    float cos[360];
    float sin[360];
}math; math M;

typedef struct {
    int x, y, z;
    int a; //   Rotation angle
    int l; //   Look up and down
}player; player P;

typedef struct {
    int x1,y1; //bottom line point 1
    int x2,y2; //bottom line point 2
    int wt,u,v; //wall texture and u/v tile
    int shade; //shade of the wall
}walls; walls W[256];

typedef struct {
    int ws,we;      //wall number start and end
    int z1,z2;      //height of bottom and top 
    int d;          //add y distances to sort drawing order
    int st,ss;      //surface texture, surface scale 
    int surfBottom[screenWidth]; // Bottom limits
    int surfTop[screenWidth]; // Top limits
}sectors; sectors S[128];


typedef struct {
    int w, h;                          //texture width/height
    const unsigned char *name;        //texture name
}TextureMaps; TextureMaps Textures[64]; //increase for more textures

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

void rad_to_degrees() {
    for (int x=0; x<360; x++) {
        M.cos[x]=cos(x/180.0*PI);
        M.sin[x]=sin(x/180.0*PI);
    }
}

void load() {
    FILE *fp = fopen("level.h","r");

    if (fp == NULL) { 
        printf("Error opening level.h"); 
        return;
    }
    int s,w;

    fscanf(fp,"%i",&numSect);   //number of sectors
    //load all sectors
    for(s=0; s < numSect; s++) {
        fscanf(fp,"%i",&S[s].ws);  
        fscanf(fp,"%i",&S[s].we); 
        fscanf(fp,"%i",&S[s].z1);  
        fscanf(fp,"%i",&S[s].z2); 
        fscanf(fp,"%i",&S[s].st); 
        fscanf(fp,"%i",&S[s].ss);  
    }

    fscanf(fp,"%i",&numWalls);   //number of walls
    //load all walls
    for(s=0; s < numWalls; s++) {
        fscanf(fp,"%i",&W[s].x1);  
        fscanf(fp,"%i",&W[s].y1); 
        fscanf(fp,"%i",&W[s].x2);  
        fscanf(fp,"%i",&W[s].y2); 
        fscanf(fp,"%i",&W[s].wt);
        fscanf(fp,"%i",&W[s].u); 
        fscanf(fp,"%i",&W[s].v);  
        fscanf(fp,"%i",&W[s].shade);  
    }
    fscanf(fp,"%i %i %i %i %i", &P.x, &P.y, &P.z, &P.a, &P.l); //player position, angle, look direction 
    fclose(fp); 
}

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
    int s,w,x,y,c;

    //draw background color
    for(y=0; y<120; y++) { 
        int y2=(screenHeight-y-1)*3*160; //invert height, x3 for rgb, x15 for texture width
        for(x=0; x<160 ;x++) { 
            int pixel=x*3+y2;
            int r=T_VIEW2D[pixel+0]; 
            int g=T_VIEW2D[pixel+1]; 
            int b=T_VIEW2D[pixel+2];

            if(G.addSect>0 && y>48-8 && y<56-8 && x>144){ 
                r=r>>1; g=g>>1; b=b>>1; //darken sector button
            }
            DrawRectangle(x * scale, y * scale, scale, scale, Color{r, g, b, 255});
            //drawPixel(x,y,r,g,b);
        }
    }

    //draw sectors
    for(s=0; s<numSect; s++) {
        for(w=S[s].ws; w<S[s].we; w++) {
            //if this sector is selected
            if(s==G.selS-1) {
                //set sector to globals
                S[G.selS-1].z1=G.z1;
                S[G.selS-1].z2=G.z2;
                S[G.selS-1].st=G.st;
                S[G.selS-1].ss=G.ss;
    
                //yellow select
                if (G.selW==0) { 
                    c = 80; //all walls yellow
                }
                //one wall selected
                else if (G.selW+S[s].ws-1==w){ 
                    c = 80; 
                    W[w].wt=G.wt; 
                    W[w].u=G.wu; 
                    W[w].v=G.wv;
                }
                else { 
                    c = 0; //grey walls
                }
            }
            else{ 
                 c= 0; //sector not selected, grey
            }

            //DrawLine(W[w].x1/G.scale, W[w].y1/G.scale, W[w].x2/G.scale, W[w].y2/G.scale, Color{128+c, 128+c, 128-c, 255});
            DrawRectangle(W[w].x1/G.scale, W[w].y1/G.scale, scale, scale, Color{255, 255, 255, 255});
            DrawRectangle(W[w].x2/G.scale, W[w].y2/G.scale, scale, scale, Color{255, 255, 255, 255});
        }
    }

    //draw player
    int dx=M.sin[P.a]*12; 
    int dy=M.cos[P.a]*12; 
    DrawRectangle(P.x/G.scale, P.y/G.scale, scale, scale, Color{0, 255, 0, 255}); 
    DrawRectangle((P.x+dx)/G.scale, (P.y+dy)/G.scale, scale, scale, Color{0, 175, 0, 255});

    //draw wall texture
    float tx=0, tx_stp=Textures[G.wt].w/15.0;
    float ty=0, ty_stp=Textures[G.wt].h/15.0;
    for(y=0;y<15;y++) { 
        tx=0;
        for(x=0;x<15;x++) { 
            int x2=(int)tx%Textures[G.wt].w; tx+=tx_stp;//*G.wu;
            int y2=(int)ty%Textures[G.wt].h; 
            int r=Textures[G.wt].name[(Textures[G.wt].h-y2-1)*3*Textures[G.wt].w+x2*3+0]; 
            int g=Textures[G.wt].name[(Textures[G.wt].h-y2-1)*3*Textures[G.wt].w+x2*3+1]; 
            int b=Textures[G.wt].name[(Textures[G.wt].h-y2-1)*3*Textures[G.wt].w+x2*3+2]; 
            DrawRectangle(x+145, y+105-8, scale, scale, Color{r, g, b, 255});
        } 
        ty+=ty_stp;//*G.wv;
    }
    //draw surface texture
    tx=0, tx_stp=Textures[G.st].w/15.0;
    ty=0, ty_stp=Textures[G.st].h/15.0;
    for(y=0;y<15;y++) { 
    tx=0;
    for(x=0;x<15;x++) { 
        int x2=(int)tx%Textures[G.st].w; tx+=tx_stp;//*G.ss;
        int y2=(int)ty%Textures[G.st].h; 
        int r=Textures[G.st].name[(Textures[G.st].h-y2-1)*3*Textures[G.st].w+x2*3+0]; 
        int g=Textures[G.st].name[(Textures[G.st].h-y2-1)*3*Textures[G.st].w+x2*3+1]; 
        int b=Textures[G.st].name[(Textures[G.st].h-y2-1)*3*Textures[G.st].w+x2*3+2]; 
        DrawRectangle(x+145,y+105-24-8, scale, scale, Color{r, g, b, 255});
    } 
    ty+=ty_stp;//*G.ss;
    }
    
    //draw numbers
    drawNumber(140,90,G.wu);   //wall u
    drawNumber(148,90,G.wv);   //wall v
    drawNumber(148,66,G.ss);   //surface v
    drawNumber(148,58,G.z2);   //top height
    drawNumber(148,50,G.z1);   //bottom height
    drawNumber(148,26,G.selS); //sector number
    drawNumber(148,18,G.selW); //wall number
}