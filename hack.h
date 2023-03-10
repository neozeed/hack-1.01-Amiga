/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.h version 1.0.1 - added some flags for HACKOPTIONS */

#include "mklev.h"
#include "hack.onames.h"

/* #define GRAPHICS 1 */

#define MUP    1
#define MDOWN  2

#define ON 1
#define OFF 0

extern struct obj *invent, *uwep, *uarm, *uarm2, *uarmh, *uarms, *uarmg, 
   *uleft, *uright, *fcobj;
extern struct obj *uchain;   /* defined iff PUNISHED */
extern struct obj *uball;   /* defined if PUNISHED */
struct obj *o_at(), *getobj(), *sobj_at();

struct flag {
	unsigned ident;		/* social security number for each monster */
	Bitfield(topl,2);	/* a top line (message) has been printed */
				/* 0: top line empty; 2: no --More-- reqd. */
	Bitfield(cbreak,1);	/* in cbreak mode, rogue format */
	Bitfield(oneline,1);	/* give inventories 1 line at a time */
	Bitfield(time,1);	/* display elapsed 'time' */
	Bitfield(nonews,1);	/* suppress news printing */
	Bitfield(notombstone,1);
 	unsigned end_top, end_around;	/* describe desired score list */
	Bitfield(end_own,1);		/* idem (list all own scores) */
	Bitfield(no_rest_on_space,1);	/* spaces are ignored */
	Bitfield(move,1);
	Bitfield(mv,1);
	Bitfield(run,3);	/* 0: h (etc), 1: H (etc), 2: fh (etc) */
				/* 3: FH, 4: ff+, 5: ff-, 6: FF+, 7: FF- */
	Bitfield(nopick,1);	/* do not pickup objects */
	Bitfield(echo,1);	/* 1 to echo characters */
	Bitfield(botl,1);	/* partially redo status line */
	Bitfield(botlx,1);	/* print an entirely new bottom line */
	Bitfield(nscrinh,1);	/* inhibit nscr() in pline(); */
};
extern struct flag flags;

struct prop {
#define   TIMEOUT      007777   /* mask */
#define   LEFT_RING   W_RINGL   /* 010000L */
#define   RIGHT_RING   W_RINGR   /* 020000L */
#define   INTRINSIC   040000L
#define   LEFT_SIDE   LEFT_RING
#define   RIGHT_SIDE   RIGHT_RING
#define   BOTH_SIDES   (LEFT_SIDE | RIGHT_SIDE)
   long p_flgs;
   int (*p_tofn)();   /* called after timeout */
};

struct you {
   xchar ux, uy;
   schar dx, dy;      /* direction of fast move */
#ifdef QUEST
   schar di;      /* direction of FF */
   xchar ux0, uy0;      /* initial position FF */
#endif QUEST
   xchar udisx, udisy;   /* last display pos */
   char usym;      /* usually '@' */
   schar uluck;
   int last_str_turn:3;   /* 0: none, 1: half turn, 2: full turn */
            /* +: turn right, -: turn left */
   Bitfield(udispl,1);   /* @ on display */
   Bitfield(ulevel,5);
#ifdef QUEST
   Bitfield(uhorizon,7);
#endif QUEST
   Bitfield(utrap,3);   /* trap timeout */
   Bitfield(utraptype,1);   /* defined if utrap nonzero */
#define   TT_BEARTRAP   0
#define   TT_PIT      1
   Bitfield(uinshop,1);


/* perhaps these #define's should also be generated by makedefs */
#define   TELEPAT      LAST_RING      /* not a ring */
#define   Telepat      u.uprops[TELEPAT].p_flgs
#define   FAST      (LAST_RING+1)      /* not a ring */
#define   Fast      u.uprops[FAST].p_flgs
#define   CONFUSION   (LAST_RING+2)      /* not a ring */
#define   Confusion   u.uprops[CONFUSION].p_flgs
#define   INVIS      (LAST_RING+3)      /* not a ring */
#define   Invis      u.uprops[INVIS].p_flgs
#define   GLIB      (LAST_RING+4)      /* not a ring */
#define   Glib      u.uprops[GLIB].p_flgs
#define   PUNISHED   (LAST_RING+5)      /* not a ring */
#define   Punished   u.uprops[PUNISHED].p_flgs
#define   SICK      (LAST_RING+6)      /* not a ring */
#define   Sick      u.uprops[SICK].p_flgs
#define   BLIND      (LAST_RING+7)      /* not a ring */
#define   Blind      u.uprops[BLIND].p_flgs
#define   WOUNDED_LEGS   (LAST_RING+8)      /* not a ring */
#define Wounded_legs   u.uprops[WOUNDED_LEGS].p_flgs
#define PROP(x) (x-RIN_ADORNMENT)       /* convert ring to index in uprops */
   Bitfield(umconf,1);
   char *usick_cause;
   struct prop uprops[LAST_RING+9];

   Bitfield(uswallow,1);      /* set if swallowed by a monster */
   Bitfield(uswldtim,4);      /* time you have been swallowed */
   Bitfield(uhs,3);         /* hunger state - see hack.eat.c */
   schar ustr,ustrmax;
   schar udaminc;
   schar uac;
   int uhp,uhpmax;
   long int ugold,ugold0,uexp,urexp;
   int uhunger;         /* refd only in eat.c and shk.c */
   int uinvault;
   struct monst *ustuck;
   int nr_killed[CMNUM+2];      /* used for experience bookkeeping */
};

extern struct you u;

extern char *traps[];
extern char *plur(), *monnam(), *Monnam(), *amonnam(), *Amonnam(),
   *doname(), *aobjnam();
extern char readchar();
extern char vowels[];

extern xchar curx,cury;   /* cursor location on screen */

extern coord bhitpos;   /* place where thrown weapon falls to the ground */

extern xchar seehx,seelx,seehy,seely; /* where to see*/
extern char *save_cm,*killer;

extern xchar dlevel, maxdlevel; /* dungeon level */

extern long moves;

extern int multi;


extern char lock[];


#define DIST(x1,y1,x2,y2)       (((x1)-(x2))*((x1)-(x2)) + ((y1)-(y2))*((y1)-(y2)))

#define   PL_CSIZ      20   /* sizeof pl_character */
#define   MAX_CARR_CAP   120   /* so that boulders can be heavier */
#define   FAR   (COLNO+2)   /* position outside screen */
