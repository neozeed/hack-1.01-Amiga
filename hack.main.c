/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.main.c version 1.0.1 - some cosmetic changes */

#include <stdio.h>
#include <signal.h>
/* #include <errno.h> */
#include "hack.h"

extern char plname[PL_NSIZ], pl_character[PL_CSIZ];
#ifndef AMIGA
extern char *getlogin();
extern char *getenv();
#endif

int (*afternmv)();

int done1();
int hangup();

char safelock[] = "safelock";
xchar locknum;            /* max num of players */
char SAVEF[PL_NSIZ + 22] = "Saved Games/";
char perm[] = "perm";
char *hname;      /* name of the game (argv[0] of call) */
char obuf[BUFSIZ];   /* BUFSIZ is defined in stdio.h */

extern char *nomovemsg;
extern long wailmsg;

main(argc,argv)
int argc;
char *argv[];
{
   int fd;
#ifdef NEWS
	int nonews = 0;
#endif NEWS
   char *dir;

   initterm();
#ifdef AMIGA
   if (argc == 0)
	{
	geticon();
	hname = HACKNAME;
	argc = 1;
	}
   else
#endif
   hname = argv[0];

   /*
    * See if we must change directory to the playground.
    * (Perhaps hack runs suid and playground is inaccessible
    *  for the player.)
    * The environment variable HACKDIR is overridden by a
    *  -d command line option.
    */
#ifndef AMIGA
   if ( (dir = getenv("HACKDIR")) == NULL)
#endif
	dir = HACKDIR;

   if(argc > 1 && !strncmp(argv[1], "-d", 2)) {
      argc--;
      argv++;
      dir = argv[0]+2;
      if(*dir == '=' || *dir == ':') dir++;
      if(!*dir && argc > 1) {
         argc--;
         argv++;
         dir = argv[0];
      }
      if(!*dir)
		    error("Flag -d must be followed by a directory name.");
   }
#ifndef AMIGA
	/*
	 * Now we know the directory containing 'record' and
	 * may do a prscore().
	 */
	if(argc > 1 && !strncmp(argv[1], "-s", 2)) {
		if(dir) chdirx(dir);
		prscore(argc, argv);
		hackexit(0);
	}
#endif
   /*
    * It seems he really wants to play. Find the creation date of
    * this game so as to avoid restoring outdated savefiles.
    */
   gethdate(hname);

   /*
    * We cannot do chdir earlier, otherwise gethdate will fail.
    */
   if(dir) chdirx(dir);
#ifdef GRAPHICS
   InitGraphics();
#endif
   /*
    * Who am i? Perhaps we should use $USER instead?
    */
#ifdef AMIGA
   if (!*plname)
#endif
	(void) strncpy(plname, getlogin(), sizeof(plname)-1);

	/*
	 * Process options.
	 */
	initoptions();
	while(argc > 1 && argv[1][0] == '-'){
		argv++;
		argc--;
		switch(argv[0][1]){
#ifdef WIZARD
		case 'w':
			if(!strcmp(getlogin(), WIZARD))
				wizard = TRUE;
			else myprintf("Sorry.\n");
			break;
#endif WIZARD
#ifdef NEWS
		case 'n':
			flags.nonews = TRUE;
			break;
#endif NEWS
		case 'u':
			if(argv[0][2])
			(void) strncpy(plname, argv[0]+2, sizeof(plname)-1);
			else if(argc > 1) {
				argc--;
				argv++;
			(void) strncpy(plname, argv[0], sizeof(plname)-1);
			} else
				myprintf("Player name expected after -u\n");
			break;
		default:
			myprintf("Unknown option: %s\n", *argv);
		}
	}

	if(argc > 1)
		locknum = atoi(argv[1]);
#ifdef WIZARD
	if(wizard) (void) strcpy(plname, "wizard"); else
#endif WIZARD
	if(!*plname || !strncmp(plname, "player", 4)) askname();
#ifdef AMIGA
	if (!pl_character[0])
#endif
	plnamesuffix();      /* strip suffix from name */

	setbuf(stdout,obuf);
	(void) srand(getpid());
	startup();
	cls();
	(void) signal(SIGHUP, hangup);
#ifdef WIZARD
	if(!wizard) {
#endif WIZARD
		(void) signal(SIGQUIT,SIG_IGN);
		(void) signal(SIGINT,SIG_IGN);
		if(locknum)
			lockcheck();
		else
			(void) strcpy(lock,plname);
#ifdef WIZARD
	} else {
		register char *sfoo;
		(void) strcpy(lock,plname);
#ifndef AMIGA
      if(sfoo = getenv("MAGIC"))
         while(*sfoo) {
            switch(*sfoo++) {
            case 'n': (void) srand(*sfoo++);
               break;
            }
         }
      if(sfoo = getenv("GENOCIDED")){
         if(*sfoo == '!'){
            extern struct permonst mons[PMONCOUNT];
            extern char genocided[], fut_geno[];
            register struct permonst *pm = mons;
            register char *gp = genocided;

            while(pm < mons+CMNUM+2){
               if(!index(sfoo, pm->mlet))
                  *gp++ = pm->mlet;
               pm++;
            }
            *gp = 0;
         } else
            (void) strcpy(genocided, sfoo);
         (void) strcpy(fut_geno, genocided);
      }
#endif
   }
#endif WIZARD
   u.uhp = 1;   /* prevent RIP on early quits */
   u.ux = FAR;   /* prevent nscr() */
   (void) strcat(SAVEF,plname);
   if((fd = open(SAVEF,0)) >= 0 &&
      (uptodate(fd) || unlink(SAVEF) == 666)) {
      (void) signal(SIGINT,done1);
      myputs("Restoring old save file...");
      (void) myfflush(stdout);
      dorecover(fd);
      flags.move = 0;
   } else {
#ifdef NEWS
	if(!flags.nonews)
		if((fd = open(NEWS,0)) >= 0)
			outnews(fd);
#endif NEWS
      flags.ident = 1;
      init_objects(0);
      u_init();
      (void) signal(SIGINT,done1);
      glo(1);
      mklev();
      u.ux = xupstair;
      u.uy = yupstair;
      (void) inshop();
      setsee();
      flags.botlx = 1;
      makedog();
      seemons();
      docrt();
      pickup();
      read_engr_at(u.ux,u.uy);   /* superfluous ? */
      flags.move = 1;
      flags.cbreak = ON;
      flags.echo = OFF;
   }
   setftty();
#ifdef TRACK
   initrack();
#endif TRACK
   for(;;) {
      if(flags.move) {
#ifdef TRACK
         settrack();
#endif TRACK
         if(moves%2 == 0 ||
           (!(Fast & ~INTRINSIC) && (!Fast || rn2(3)))) {
            extern struct monst *makemon();
            movemon();
            if(!rn2(70))
                (void) makemon((struct permonst *)0, 0, 0);
         }
         if(Glib) glibr();
         timeout();
			++moves;
			if(flags.time) flags.botl = 1;
			if(u.uhp < 1) {
				pline("You die...");
				done("died");
				}
         if(u.uhp*10 < u.uhpmax && moves-wailmsg > 50){
             wailmsg = moves;
             if(u.uhp == 1)
             pline("You hear the wailing of the Banshee...");
             else
             pline("You hear the howling of the CwnAnnwn...");
         }
         if(u.uhp < u.uhpmax) {
            if(u.ulevel > 9) {
               if(Regeneration || !(moves%3)) {
                   flags.botl = 1;
                   u.uhp += rnd((int) u.ulevel-9);
                   if(u.uhp > u.uhpmax)
                  u.uhp = u.uhpmax;
               }
            } else if(Regeneration ||
               (!(moves%(22-u.ulevel*2)))) {
               flags.botl = 1;
               u.uhp++;
            }
         }
         if(Teleportation && !rn2(85)) tele();
         if(Searching && multi >= 0) (void) dosearch();
         gethungry();
         invault();
      }
      if(multi < 0) {
         if(!++multi){
            pline(nomovemsg ? nomovemsg :
               "You can move again.");
            nomovemsg = 0;
            if(afternmv) (*afternmv)();
            afternmv = 0;
         }
      }
      flags.move = 1;
      find_ac();
#ifndef QUEST
      if(!flags.mv || Blind)
#endif QUEST
      {
         seeobjs();
         seemons();
         nscr();
      }
      if(flags.botl || flags.botlx) bot();
      if(multi > 0) {
#ifdef QUEST
         if(flags.run >= 4) finddir();
#endif QUEST
         lookaround();
         if(!multi) {   /* lookaround may clear multi */
            flags.move = 0;
            continue;
         }
         if(flags.mv) {
            if(multi<COLNO && !--multi)
               flags.mv = flags.run = 0;
            domove();
         } else {
            --multi;
            rhack(save_cm);
         }
		} else if(multi == 0)
			rhack((char *) 0);
		if(multi && multi%7 == 0)
			(void) fflush(stdout);
   }
}

lockcheck()
{
/*   extern int errno;                         */
/*   register int i, fd;                       */
/*                                             */
/* we ignore QUIT and INT at this point        */
/*    if (link(perm,safelock) == -1)           */
/*        error("Cannot link safelock. (Try again or rm safelock.)");*/
/*                                             */
/*                                             */
/*    for(i = 0; i < locknum; i++) {           */
/*       lock[0]= 'a' + i;                     */
/*       if((fd = open(lock,0)) == -1) {       */
/*          if(errno == ENOENT) goto gotlock;  */  /* no such file */
/*          (void) unlink(safelock);           */
/*          error("Cannot open %s", lock);	*/
/*       }					*/
/*       (void) close(fd);			*/
/*    }						*/
/*     (void) unlink(safelock);			*/
/*   error("Too many hacks running now.");	*/
/*	}					*/
/* gotlock:					*/
/*    fd = creat(lock,FMASK);                  */
/*	if(unlink(safelock) == -1) {		*/
/*		error("Cannot unlink safelock.");*/
/*    if(fd == -1) {                           */
/*       error("cannot creat lock file.");     */
/*    } else {                                 */
/*       int pid;                              */
/*                                             */
/*       pid = getpid();                       */ 
/*		if(write(fd, (char *) &pid, sizeof(pid)) != sizeof(pid)){ */
/*          error("cannot write lock");        */
/*       }                                     */
/*	if(close(fd) == -1) {			*/
/*          error("cannot close lock");        */
/*       }                                     */
/*    }                                        */
}

/*VARARGS1*/
error(s,a1,a2,a3,a4) char *s,*a1,*a2,*a3,*a4;
   {
   myprintf("Error: ");
   myprintf(s,a1,a2,a3,a4);
   (void) myputchar('\n');
   hackexit(1);
   }

glo(foo)
register int foo;
{
   /* construct the string  xlock.n  */
   register char *tf;

   tf = lock;
   while(*tf && *tf!='.') tf++;
   (void) sprintf(tf, ".%d", foo);
}

/*
 * plname is filled either by an option (-u Player  or  -uPlayer) or
 * explicitly (-w implies wizard) or by askname.
 * It may still contain a suffix denoting pl_character.
 */
askname(){
register int c,ct;
	myprintf("\nWho are you? ");
	ct = 0;
	(void) myfflush();
	while((c = inchar()) != '\n')
		{
		if (c != '-')
			if (c == 8) { /* backspace */
				if (ct) {
					ct--;
					backsp();
					myputchar(' ');
					backsp();
					myfflush();
					}
			continue;
			}
		else
            if (c < 'A' || (c > 'Z' && c < 'a') || c > 'z') c = '_';
               if (ct < sizeof(plname)-1)
                  {
                  plname[ct++] = c;
                  myprintf("%c", c);
                  }
	(void) myfflush();
      }
   plname[ct] = 0;
   if(ct == 0) askname();
#ifdef QUEST
   else myprintf("Hello %s, welcome to quest!\n", plname);
#else
   else myprintf("Hello %s, welcome to hack!\n", plname);
#endif QUEST
}

impossible(){
   pline("Program in disorder - perhaps you'd better Quit");
}

#ifdef NEWS
int stopnews;

stopnws(){
   (void) signal(SIGINT, SIG_IGN);
   stopnews++;
}

outnews(fd) int fd; {
int (*prevsig)();
char ch;
   prevsig = signal(SIGINT, stopnws);
   while(!stopnews && read(fd,&ch,1) == 1)
      (void) myputchar(ch);
   (void) myputchar('\n');
   (void) myfflush(stdout);
   (void) close(fd);
   (void) signal(SIGINT, prevsig);
   /* See whether we will ask TSKCFW: he might have told us already */
   if(!stopnews && pl_character[0])
      getret();
}
#endif NEWS

chdirx(dir) char *dir; {
   if(chdir(dir) < 0) {
      perror(dir);
      error("Cannot chdir to %s.", dir);
   }
}
