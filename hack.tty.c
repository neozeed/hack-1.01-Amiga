/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include   "hack.h"
#include   <stdio.h>

char inchar();

gettty(){
}

/* reset terminal to original state */
settty(s) char *s; {
   clear_screen();
   if(s) myprintf(s);
   (void) myfflush(stdout);
   flags.echo = OFF;
   flags.cbreak = OFF;
}

setctty(){
}

setftty(){
}

echo(n)
register int n;
{
}

/* always want to expand tabs, or to send a clear line char before
   printing something on topline */
xtabs()
{
}

#ifdef LONG_CMD
cbreak(n)
register int n;
{
}
#endif LONG_CMD

getlin(bufp)
register char *bufp;
{
   register char *obufp = bufp;
   register int c;

   flags.topl = 2;      /* nonempty, no --More-- required */
   for(;;) {
      (void) myfflush(stdout);
      c = inchar();
      if(c == '\b') {
         if(bufp != obufp) {
            bufp--;
            putstr("\b \b"); /* putsym converts \b */
         } else   bell();
      } else if(c == '\n') {
         *bufp = 0;
         return;
      } else {
         *bufp = c;
         bufp[1] = 0;
         putstr(bufp);
         if(bufp-obufp < BUFSZ-1 && bufp-obufp < COLNO)
            bufp++;
      }
   }
}

getret() {
   xgetret(TRUE);
}

cgetret() {
   xgetret(FALSE);
}

xgetret(spaceflag)
boolean spaceflag;   /* TRUE if space (return) required */
{
   myprintf("\nHit %s to continue: ",
      flags.cbreak ? "space" : "return");
   xwaitforspace(spaceflag);
}

char morc;   /* tell the outside world what char he used */

xwaitforspace(spaceflag)
boolean spaceflag;
{
register int c;

   (void) myfflush(stdout);
   morc = 0;

   while((c = inchar()) != '\n')
      {
      if (flags.cbreak)
         {
         if (c == ' ')
            break;
         if (!spaceflag && letter(c))
            {
            morc = c;
            break;
            }
         }
      }
   }

char *
parse()
{
   static char inline[COLNO];
   register int foo;

   flags.move = 1;
   if(!Invis) curs(u.ux,u.uy+2); else home();
   (void) myfflush(stdout);
   while((foo = inchar()) >= '0' && foo <= '9')
      multi += 10*multi+foo-'0';
   if(multi) {
      multi--;
      save_cm = inline;
   }
   inline[0] = foo;
   inline[1] = 0;
   if(foo == 'f' || foo == 'F'){
      inline[1] = inchar();
#ifdef QUEST
      if(inline[1] == foo) inline[2] = inchar(); else
#endif QUEST
      inline[2] = 0;
   }
   if(foo == 'm' || foo == 'M'){
      inline[1] = inchar();
      inline[2] = 0;
   }
   clrlin();
   return(inline);
}

char
readchar() {
   register int sym;
   (void) myfflush(stdout);
   sym = inchar();
   if(flags.topl == 1) flags.topl = 2;
   return((char) sym);
}
