/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include <stdio.h>
#include "config.h"   /* for ROWNO and COLNO */

#define HEIGHT 8
#define WIDTH  8
#define TOP    16
#define LEFT   3

startup()
{
}

/* Cursor movements */
extern xchar curx, cury;

curs(x,y)
register int x,y;   /* not xchar: perhaps xchar is unsigned and
            curx-x would be unsigned as well */
{
   if (y == cury && x == curx) return;
   cmov(x,y);
}

nocmov(x,y)
{
  cmov(x,y);   /* always go to the requested position */
}

cmov(x,y)
register int x,y;
{
   setxy(LEFT+x*WIDTH,TOP+y*HEIGHT);
   cury = y;
   curx = x;
}


cl_end() {
   weraeol();
}

clear_screen() {
   /* printf(CL); */
   home();
}

home()
{
   setxy(TOP,LEFT);
   curx = cury = 1;
}

standoutbeg()
{
   /* printf(SO); */
}

standoutend()
{
   /* printf(SE); */
}

backsp()
{
   cmov(curx-1,cury);
}

bell()
{
    /* putchar('\007'); */
}

delay_output()
{
   /* delay 40 ms, 50 ticks/sec    */
   Delay (2);
}

