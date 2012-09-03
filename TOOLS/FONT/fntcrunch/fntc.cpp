


#include <stdio.h>
#include "pngio.h"



pngT* src;
pngT* dst;
int dx = 1, dy = 0;



bool isColumnUsed(int x)
{
 for(int y = 0; y < src->h; y++)
 {
    rgbaT p = src->p[x + y*src->w];
    if(p.a == 0) continue;
    if(p.r != 255) return true;
    if(p.g != 255) return true;
    if(p.b != 255) return true;
 }

 return false;
}



void copyChar(int sx, int ex)
{
 int w = ex-sx+1;

 if(dx + w >= dst->w)
 {
    dy += src->h;
    dx = 1;

    if(dy + src->h >= dst->h)
	fprintf(stderr, "font doesn't fit 8 lines\n");
 }

// printf("copy from x:%d-%d to x,y: %d %d\n", sx, ex, dx, dy);

 for(int x = 0; x < w; x++)
    for(int y = 0; y < src->h; y++)
	dst->p[dx+x + (dy+y)*dst->w] = src->p[sx+x + y*src->w];

 dx += w+2;
}





int main(int argc, char** argv)
{
 src = pngLoad(argv[1]);
 if(!src)
 {
    perror("pngLoad");
    return -1;
 }
 dst = pngEmpty(src->h*8, src->h*8);

 // clean
 for(int k = 0; k < dst->w*dst->h*4; k++)
    dst->d[k] = 0;


 bool prevused = true;
 int lastCharX = 0;

 for(int x = 0; x < src->w; x++)
    if(isColumnUsed(x))
    {
	if(!lastCharX) lastCharX = x;
	prevused = true;
    }
    else
    {
	if(prevused)
	    copyChar(lastCharX, x);
	lastCharX = 0;
	prevused = false;
    }

 pngSave("out.png", dst);
}



// EOF ========================================================================
