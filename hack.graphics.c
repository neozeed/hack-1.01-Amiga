/* Copyright (c) John A. Toebes, VIII 1986 */
#include <exec/types.h>
#include <exec/io.h>
#include <intuition/intuition.h>
#include <graphics/view.h>
#include <stdio.h>
#include <fcntl.h>
#include "config.h"
#include "hack.h"   /* for ROWNO and COLNO */

#define XSIZE 8
#define YSIZE 8
#define BASEX (-4)
#define BASEY 19
#define PICSIZE (8*4)
#define USEDPLANES 3
extern struct Window *HackWindow;
extern struct Screen *HackScreen;
UWORD colormap[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
USHORT mondata[] = 
  { 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00,
    0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00,
    0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00,
    0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00 };
struct Image monImage = 
   { 0, 0, 8, 8, 3, &mondata[0], 0x0f, 0x0, NULL };
char *monpics;
char basemon;

InitGraphics()
   {
   char maxchar;
   int file;
   int size;
   int i;
   register int dummy1, dummy2;
   if ( (file = open(HACKCSET, O_RDONLY)) == -1 )
	panic("Cannot Open Graphics Characters");
/*   cm = GetColorMap(16);
   HackScreen->ViewPort.ColorMap = cm; */
   mread(file,colormap, 32);
   for (i=0; i<8; i++)
	SetRGB4(&HackScreen->ViewPort, i,
		(colormap[i] >> 8) & 0x0f,
		(colormap[i] >> 4) & 0x0f,
		(colormap[i]) &0x0f);
   mread(file,&basemon,1);
   mread(file,&maxchar,1);
   size = (maxchar-basemon)*PICSIZE;
   if ((monpics = (char *)malloc(size)) == NULL)
	panic("Cannot get char area");
   mread(file,monpics,size);
   }

at(x,y,ch)
register xchar x,y;
char ch;
   {
   char *thisone;
   int i;

   /* first construct the picture */
   thisone = monpics+PICSIZE*(ch-basemon);
   for (i=0; i<PICSIZE; i++)
	mondata[i] = thisone[i] << 8;
   if ((curx == x) & (cury == (y+2)) )
	home();
   myfflush();
   DrawImage(HackWindow->RPort, &monImage, BASEX+(x*XSIZE), BASEY+(y*YSIZE));
   }
