/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.pri.c version 1.0.1 - tiny change in mnewsym() - added time */

#include "hack.h"
#include <stdio.h>
xchar scrlx, scrhx, scrly, scrhy;   /* corners of new area on screen */

extern char *hu_stat[];   /* in eat.c */

swallowed()
{
   char *ulook = "|@|";
   ulook[1] = u.usym;

   cls();
   curs(u.ux-1, u.uy+1);
   myprintf("/-\\");
   curx = u.ux+2;
   curs(u.ux-1, u.uy+2);
   myprintf(ulook);
   curx = u.ux+2;
   curs(u.ux-1, u.uy+3);
   myprintf("\\-/");
   curx = u.ux+2;
   u.udispl = 1;
   u.udisx = u.ux;
   u.udisy = u.uy;
}


/*VARARGS1*/
boolean panicking;

panic(str,a1,a2,a3,a4,a5,a6)
char *str;
{
   if(panicking++) hackexit(1);   /* avoid loops */
   home();
   myprintf(" Suddenly, the dungeon collapses.\n");
   myprintf(" ERROR:  ");
   myprintf(str,a1,a2,a3,a4,a5,a6);
/* if(fork()) */
      done("panic");
/* else          */
/*    abort();   */   /* generate core dump */
}

atl(x,y,ch)
register int x,y;
{
   register struct rm *crm = &levl[x][y];

   if(x<0 || x>COLNO-1 || y<0 || y>ROWNO-1)
      panic("at(%d,%d,%c_%o_)",x,y,ch,ch);
   if(crm->seen && crm->scrsym == ch) return;
   crm->scrsym = ch;
   crm->new = 1;
   on_scr(x,y);
}

on_scr(x,y)
register int x,y;
{
   if(x<scrlx) scrlx = x;
   if(x>scrhx) scrhx = x;
   if(y<scrly) scrly = y;
   if(y>scrhy) scrhy = y;
}

/* call: (x,y) - display
   (-1,0) - close (leave last symbol)
   (-1,-1)- close (undo last symbol)
   (-1,let)-open: initialize symbol
   (-2,let)-change let
*/

tmp_at(x,y) schar x,y; {
static schar prevx, prevy;
static char let;
   if((int)x == -2){   /* change let call */
      let = y;
      return;
   }
   if((int)x == -1 && (int)y >= 0){   /* open or close call */
      let = y;
      prevx = -1;
      return;
   }
   if(prevx >= 0 && cansee(prevx,prevy)) {
      delay_output();
      prl(prevx, prevy);   /* in case there was a monster */
      at(prevx, prevy, levl[prevx][prevy].scrsym);
   }
   if(x >= 0){   /* normal call */
      if(cansee(x,y)) at(x,y,let);
      prevx = x;
      prevy = y;
   } else {   /* close call */
      let = 0;
      prevx = -1;
   }
}

/* like the previous, but the symbols are first erased on completion */
Tmp_at(x,y) schar x,y; {
static char let;
static xchar cnt;
static coord tc[COLNO];      /* but watch reflecting beams! */
register int xx,yy;
   if((int)x == -1) {
      if(y > 0) {   /* open call */
         let = y;
         cnt = 0;
         return;
      }
      /* close call (do not distinguish y==0 and y==-1) */
      while(cnt--) {
         xx = tc[cnt].x;
         yy = tc[cnt].y;
         prl(xx, yy);
         at(xx, yy, levl[xx][yy].scrsym);
      }
      cnt = let = 0;   /* superfluous */
      return;
   }
   if((int)x == -2) {   /* change let call */
      let = y;
      return;
   }
   /* normal call */
   if(cansee(x,y)) {
      if(cnt) delay_output();
      at(x,y,let);
      tc[cnt].x = x;
      tc[cnt].y = y;
      if(++cnt >= COLNO) panic("Tmp_at overflow?");
      levl[x][y].new = 0;   /* prevent pline-nscr erasing --- */
   }
}

#ifndef GRAPHICS
at(x,y,ch)
register xchar x,y;
char ch;
{
#ifndef lint
   /* if xchar is unsigned, lint will complain about  if(x < 0)  */
   if(x < 0 || x > COLNO-1 || y < 0 || y > ROWNO-1)
      panic("At gets 0%o at %d %d(%d %d)",ch,x,y,u.ux,u.uy);
#endif lint
   if(!ch) {
      home();
      myprintf("At gets null at %2d %2d.",x,y);
      curx = ROWNO+1;
      return;
   }
   y += 2;
   curs(x,y);
   myputchar(ch);
   curx++;
}
#endif

prme(){
   if(!Invis) at(u.ux,u.uy,u.usym);
}

docrt()
{
   register int x,y;
   register struct rm *room;
   register struct monst *mtmp;

   if(u.uswallow) {
      swallowed();
      return;
   }
   cls();
   if(!Invis){
      levl[(u.udisx = u.ux)][(u.udisy = u.uy)].scrsym = u.usym;
      levl[u.udisx][u.udisy].seen = 1;
      u.udispl = 1;
   } else   u.udispl = 0;

   /* %% - is this really necessary? */
   for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
      if(mtmp->mdispl && !(room = &levl[mtmp->mx][mtmp->my])->new &&
         !room->seen)
         mtmp->mdispl = 0;

   for(y = 0; y < ROWNO; y++)
      for(x = 0; x < COLNO; x++)
         if((room = &levl[x][y])->new) {
            room->new = 0;
            at(x,y,room->scrsym);
         } else if(room->seen) at(x,y,room->scrsym);
   scrlx = COLNO;
   scrly = ROWNO;
   scrhx = scrhy = 0;
   flags.botlx = 1;
   bot();
}

docorner(xmin,ymax) register int xmin,ymax; {
   register int x,y;
   register struct rm *room;
   if(u.uswallow) {   /* Can be done more efficiently */
      swallowed();
      return;
   }
   for(y = 0; y < ymax; y++) {
      curs(xmin,y+2);
      cl_end();
      for(x = xmin; x < COLNO; x++) {
         if((room = &levl[x][y])->new) {
            room->new = 0;
            at(x,y,room->scrsym);
         } else if(room->seen) at(x,y,room->scrsym);
      }
   }
}

pru()
{
   if(u.udispl && (Invis || u.udisx != u.ux || u.udisy != u.uy))
      /* if(! levl[u.udisx][u.udisy].new) */
         if(!vism_at(u.udisx, u.udisy))
            newsym(u.udisx, u.udisy);
   if(Invis) {
      u.udispl = 0;
      prl(u.ux,u.uy);
   } else
   if(!u.udispl || u.udisx != u.ux || u.udisy != u.uy) {
      atl(u.ux, u.uy, u.usym);
      u.udispl = 1;
      u.udisx = u.ux;
      u.udisy = u.uy;
   }
   levl[u.ux][u.uy].seen = 1;
}

#ifndef NOWORM
#include   "def.wseg.h"
extern struct wseg *m_atseg;
#endif NOWORM

/* print a position that is visible for @ */
prl(x,y)
{
   register struct rm *room;
   register struct monst *mtmp;
   register struct obj *otmp;

   if(x == u.ux && y == u.uy && !Invis) {
      pru();
      return;
   }
   room = &levl[x][y];
   if((!room->typ) || (room->typ<DOOR && levl[u.ux][u.uy].typ == CORR))
      return;
   if((mtmp = m_at(x,y)) && !mtmp->mhide &&
      (!mtmp->minvis || See_invisible)) {
#ifndef NOWORM
      if(m_atseg)
         pwseg(m_atseg);
      else
#endif NOWORM
      pmon(mtmp);
   }
   else if(otmp = o_at(x,y))
      atl(x,y,otmp->olet);
   else if(mtmp && (!mtmp->minvis || See_invisible)) {
      /* must be a hiding monster, but not hiding right now */
      /* assume for the moment that long worms do not hide */
      pmon(mtmp);
   }
   else if(g_at(x,y,fgold)) atl(x,y,'$');
   else if(!room->seen || room->scrsym == ' ') {
      room->new = room->seen = 1;
      newsym(x,y);
      on_scr(x,y);
   }
   room->seen = 1;
}

char
news0(x,y)
register xchar x,y;
{
   register struct obj *otmp;
   register struct gen *gtmp;
   struct rm *room;
   register char tmp;

   room = &levl[x][y];
   if(!room->seen) tmp = ' ';
   else if(!Blind && (otmp = o_at(x,y))) tmp = otmp->olet;
   else if(!Blind && g_at(x,y,fgold)) tmp = '$';
   else if(x == xupstair && y == yupstair) tmp = '<';
   else if(x == xdnstair && y == ydnstair) tmp = '>';
   else if((gtmp = g_at(x,y,ftrap)) && (gtmp->gflag & SEEN)) tmp = '^';
   else switch(room->typ) {
   case SCORR:
   case SDOOR:
      tmp = room->scrsym;   /* %% wrong after killing mimic ! */
      break;
   case HWALL:
      tmp = '-';
      break;
   case VWALL:
      tmp = '|';
      break;
   case LDOOR:
   case DOOR:
      tmp = '+';
      break;
   case CORR:
      tmp = CORR_SYM;
      break;
   case ROOM:
      if(room->lit || cansee(x,y) || Blind) tmp = '.';
      else tmp = ' ';
      break;
   default: tmp = ERRCHAR;
   }
   return(tmp);
}

newsym(x,y)
register int x,y;
{
   atl(x,y,news0(x,y));
}

/* used with wand of digging: fill scrsym and force display */
mnewsym(x,y)
register int x,y;
{
	register struct monst *mtmp = m_at(x,y);
	register struct rm *room;
	char newscrsym;

   if(!mtmp || (mtmp->minvis && !See_invisible) ||
          (mtmp->mhide && o_at(x,y))){
		room = &levl[x][y];
		newscrsym = news0(x,y);
		if(room->scrsym != newscrsym) {
			room->scrsym = newscrsym;
			room->seen = 0;
		}
   }
}

nosee(x,y)
register int x,y;
{
   register struct rm *room;

   room = &levl[x][y];
   if(room->scrsym == '.' && !room->lit && !Blind) {
      room->scrsym = ' ';
      room->new = 1;
      on_scr(x,y);
   }
}

#ifndef QUEST
prl1(x,y)
register int x,y;
{
   if(u.dx) {
      if(u.dy) {
         prl(x-(2*u.dx),y);
         prl(x-u.dx,y);
         prl(x,y);
         prl(x,y-u.dy);
         prl(x,y-(2*u.dy));
      } else {
         prl(x,y-1);
         prl(x,y);
         prl(x,y+1);
      }
   } else {
      prl(x-1,y);
      prl(x,y);
      prl(x+1,y);
   }
}

nose1(x,y)
register int x,y;
{
   if(u.dx) {
      if(u.dy) {
         nosee(x,u.uy);
         nosee(x,u.uy-u.dy);
         nosee(x,y);
         nosee(u.ux-u.dx,y);
         nosee(u.ux,y);
      } else {
         nosee(x,y-1);
         nosee(x,y);
         nosee(x,y+1);
      }
   } else {
      nosee(x-1,y);
      nosee(x,y);
      nosee(x+1,y);
   }
}
#endif QUEST

vism_at(x,y) register int x,y; {
register struct monst *mtmp;
register int csi = (See_invisible != 0);
   return((x == u.ux && y == u.uy && (!Invis || csi)) ? 1 :
      ((mtmp = m_at(x,y)) && (!mtmp->minvis || csi) &&
         (!mtmp->mhide || !o_at(mtmp->mx,mtmp->my)))
      ? cansee(x,y) : 0);
}

#ifdef NEWSCR
pobj(obj) register struct obj *obj; {
register int show = (!obj->oinvis || See_invisible) &&
      cansee(obj->ox,obj->oy);
   if(obj->odispl){
      if(obj->odx != obj->ox || obj->ody != obj->oy || !show)
      if(!vism_at(obj->odx,obj->ody)){
         newsym(obj->odx, obj->ody);
         obj->odispl = 0;
      }
   }
   if(show && !vism_at(obj->ox,obj->oy)){
      atl(obj->ox,obj->oy,obj->olet);
      obj->odispl = 1;
      obj->odx = obj->ox;
      obj->ody = obj->oy;
   }
}
#endif NEWSCR

unpobj(obj) register struct obj *obj; {
/*    if(obj->odispl){
      if(!vism_at(obj->odx, obj->ody))
         newsym(obj->odx, obj->ody);
      obj->odispl = 0;
   }
*/
   if(!vism_at(obj->ox,obj->oy))
      newsym(obj->ox,obj->oy);
}

seeobjs(){
register struct obj *obj, *obj2;
   for(obj = fobj; obj; obj = obj2) {
      obj2 = obj->nobj;
      if(obj->olet == FOOD_SYM && obj->otyp >= CORPSE
         && obj->age + 250 < moves)
            delobj(obj);
   }
   for(obj = invent; obj; obj = obj2) {
      obj2 = obj->nobj;
      if(obj->olet == FOOD_SYM && obj->otyp >= CORPSE
         && obj->age + 250 < moves)
            useup(obj);
   }
}

seemons(){
register struct monst *mtmp;
   for(mtmp = fmon; mtmp; mtmp = mtmp->nmon){
      pmon(mtmp);
#ifndef NOWORM
      if(mtmp->wormno) wormsee(mtmp->wormno);
#endif NOWORM
   }
}

pmon(mon) register struct monst *mon; {
register int show =
   ((!mon->minvis || See_invisible) &&
      (!mon->mhide || !o_at(mon->mx,mon->my)) &&
      cansee(mon->mx,mon->my))
    || (Blind && Telepat);
   if(mon->mdispl){
      if(mon->mdx != mon->mx || mon->mdy != mon->my || !show)
         unpmon(mon);
   }
   if(show && !mon->mdispl){
      atl(mon->mx,mon->my,
        mon->mimic ? mon->mimic : mon->data->mlet);
      mon->mdispl = 1;
      mon->mdx = mon->mx;
      mon->mdy = mon->my;
   }
}

unpmon(mon) register struct monst *mon; {
   if(mon->mdispl){
      newsym(mon->mdx, mon->mdy);
      mon->mdispl = 0;
   }
}

nscr()
{
   register int x,y;
   register struct rm *room;

   if(u.uswallow || u.ux == FAR || flags.nscrinh) return;
   pru();
   for(y = scrly; y <= scrhy; y++)
      for(x = scrlx; x <= scrhx; x++)
         if((room = &levl[x][y])->new) {
            room->new = 0;
            at(x,y,room->scrsym);
         }
   scrhx = scrhy = 0;
   scrlx = COLNO;
   scrly = ROWNO;
}

char oldbot[100], newbot[100];      /* 100 >= COLNO */
extern char *eos();
bot()
{
register char *ob = oldbot, *nb = newbot;
register int i;
	if(flags.botlx) *ob = 0;
	flags.botl = flags.botlx = 0;
	(void) sprintf(newbot,
		"Level %-2d  Gold %-5lu  Hp %3d(%d)  Ac %-2d  Str ",
		dlevel, u.ugold, u.uhp, u.uhpmax, u.uac);
	if(u.ustr>18) {
	    if(u.ustr>117)
		(void) strcat(newbot,"18/**");
	    else
		(void) sprintf(eos(newbot), "18/%02d",u.ustr-18);
	} else
	    (void) sprintf(eos(newbot), "%-2d   ",u.ustr);
	(void) sprintf(eos(newbot), "  Exp %2d/%-5lu ", u.ulevel,u.uexp);
	(void) strcat(newbot, hu_stat[u.uhs]);
	if(flags.time)
	    (void) sprintf(eos(newbot), "  %ld", moves);
	if(strlen(newbot) >= COLNO) {
		register char *bp0, *bp1;
		bp0 = bp1 = newbot;
		do {
			if(*bp0 != ' ' || bp0[1] != ' ' || bp0[2] != ' ')
				*bp1++ = *bp0;
		} while(*bp0++);
	}
	for(i = 1; i<COLNO; i++) {
      if(*ob != *nb){
         curs(i,ROWNO+2);
         (void) myputchar(*nb ? *nb : ' ');
         curx++;
      }
      if(*ob) ob++;
      if(*nb) nb++;
   }
   (void) strcpy(oldbot, newbot);
}

#ifdef WAN_PROBING
mstatusline(mtmp) register struct monst *mtmp; {
   pline("Status of %s: ", monnam(mtmp));
   pline("Level %-2d  Gold %-5lu  Hp %3d(%d)  Ac %-2d  Dam %d",
       mtmp->data->mlevel, mtmp->mgold, mtmp->mhp, mtmp->orig_hp,
       mtmp->data->ac, (mtmp->data->damn + 1) * (mtmp->data->damd + 1));
}
#endif WAN_PROBING

cls(){
   if(flags.topl == 1)
      more();
   flags.topl = 0;

   clear_screen();

   flags.botlx = 1;
}

