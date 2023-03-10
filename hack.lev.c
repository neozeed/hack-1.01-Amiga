/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.lev.c version 1.0.1 - somewhat more careful monster regeneration */

#include "hack.h"
#include <signal.h>
#include <stdio.h>
extern struct monst *restmonchn();
extern struct obj *restobjchn();
extern struct obj *billobjs;
extern char *itoa();

extern char nul[];
#ifndef NOWORM
#include   "def.wseg.h"

extern struct wseg *wsegs[32], *wheads[32];
extern long wgrowtime[32];
#endif NOWORM

getlev(fd)
{
   register struct gen *gtmp;
#ifndef NOWORM
   register struct wseg *wtmp;
#endif NOWORM
   register int tmp;
   long omoves;

   if(fd<0 || read(fd, (char *) levl, sizeof(levl)) != sizeof(levl))
      return(1);
   fgold = 0;
   ftrap = 0;
   mread(fd, (char *)&omoves, sizeof(omoves));   /* 0 from MKLEV */
   mread(fd, (char *)&xupstair, sizeof(xupstair));
   mread(fd, (char *)&yupstair, sizeof(yupstair));
   mread(fd, (char *)&xdnstair, sizeof(xdnstair));
   mread(fd, (char *)&ydnstair, sizeof(ydnstair));

   fmon = restmonchn(fd);
   if(omoves) {
	/* regenerate animals while on another level */
	long tmoves = (moves > omoves) ? moves-omoves : 0;
	register struct monst *mtmp, *mtmp2;
	extern char genocided[];
	long newhp;

       for(mtmp = fmon; mtmp; mtmp = mtmp2) {
      mtmp2 = mtmp->nmon;
      if(index(genocided, mtmp->data->mlet)) {
         mondead(mtmp);
         continue;
		}
		newhp = mtmp->mhp +
			(index("ViT", mtmp->data->mlet) ? tmoves : tmoves/20);
		if(newhp > mtmp->orig_hp)
			mtmp->mhp = mtmp->orig_hp;
		else
			mtmp->mhp = newhp;
       }
   }

   setshk();
   setgd();
   gtmp = newgen();
   mread(fd, (char *)gtmp, sizeof(struct gen));
   while(gtmp->gx) {
      gtmp->ngen = fgold;
      fgold = gtmp;
      gtmp = newgen();
      mread(fd, (char *)gtmp, sizeof(struct gen));
   }
   mread(fd, (char *)gtmp, sizeof(struct gen));
   while(gtmp->gx) {
      gtmp->ngen = ftrap;
      ftrap = gtmp;
      gtmp = newgen();
      mread(fd, (char *)gtmp, sizeof(struct gen));
   }
   free((char *) gtmp);
   fobj = restobjchn(fd);
   billobjs = restobjchn(fd);
   rest_engravings(fd);
#ifndef QUEST
   mread(fd, (char *)rooms, sizeof(rooms));
   mread(fd, (char *)doors, sizeof(doors));
#endif QUEST
   if(!omoves) return(0);   /* from MKLEV */
#ifndef NOWORM
   mread(fd, (char *)wsegs, sizeof(wsegs));
   for(tmp = 1; tmp < 32; tmp++) if(wsegs[tmp]){
      wheads[tmp] = wsegs[tmp] = wtmp = newseg();
      while(1) {
         mread(fd, (char *)wtmp, sizeof(struct wseg));
         if(!wtmp->nseg) break;
         wheads[tmp]->nseg = wtmp = newseg();
         wheads[tmp] = wtmp;
      }
   }
   mread(fd, (char *)wgrowtime, sizeof(wgrowtime));
#endif NOWORM
   return(0);
}

mread(fd, buf, len)
register int fd;
register char *buf;
register unsigned len;
{
register int rlen;
   rlen = read(fd, buf, (int) len);
   if(rlen != len){
      pline("Read %d instead of %d bytes\n", rlen, len);
      panic("Cannot read %d bytes from file #%d\n", len, fd);
   }
}
