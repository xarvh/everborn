



#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "net.h"







static int fileRead(connectionC *server, netReqC *request)
{
 FILE *f;
 char *d;
 int size, r;


 f = fopen("xxx.pack", "wb");
 if(f == NULL) return -1;

 size = request->size;
 d = (char*)request->data;
 do {
    r = fwrite(d, 1, size, f);
    d += r;
    size -= r;
 } while(r);
 fclose(f);

 return size;
}






static netPwtT *fileWrite(netReqC *header)
{
 FILE *f;
 netPwtT *p;
 int size, r;
 char *d;

 f = fopen(header->item, "rb");
 if(f == NULL) return NULL;

 fseek(f, 0, SEEK_END);
 size = ftell(f);
 fseek(f, 0, SEEK_SET);

 p = (netPwtT*)malloc( sizeof(netPwtT) + size );
 if(p == NULL) return NULL;

 p->size = size;
 

 d = p->data;

 do {
    r = fread(d, 1, size, f);
    size -= r;
    d += r;
 } while(r);

 fclose(f);
 return p;
}


static int fileError(connectionC *server, netReqC *request)
{
}




static netPackC netPackFile("file", fileError, fileRead, fileWrite);


//EOF =========================================================================
