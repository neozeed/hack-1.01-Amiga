/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.eat.c version 1.0.1 - added morehungry() and FAINTED */

#include   "hack.h"
char POISONOUS[] = "ADKSVabhks";
extern char *nomovemsg;
extern int (*afternmv)();

/* hunger texts used on bottom line (each 8 chars long) */
#define SATIATED	0
#define NOT_HUNGRY	1
#define HUNGRY		2
#define WEAK		3
#define FAINTING	4
#define FAINTED		5
#define STARVED		6

char *hu_stat[] = {
	"Satiated",
	"        ",
	"Hungry  ",
	"Weak    ",
	"Fainting",
	"Fainted ",
	"Starved "
};

init_uhunger(){
   u.uhunger = 900;
   u.uhs = NOT_HUNGRY;
}

struct { char *txt; int nut; } tintxts[] = {
   {"It contains first quality peaches - what a surprise!",   40},
   {"It contains salmon - not bad!",   60},
   {"It contains apple juice - perhaps not what you hoped for.", 20},
   {"It contains some nondescript substance, tasting awfully.", 500},
   {"It contains rotten meat. You vomit.", -50},
   {"It turns out to be empty.",   0}
};

tinopen(){
#define   TTSZ   (sizeof(tintxts)/sizeof(tintxts[0]))
register int r = rn2(2*TTSZ);
   if(r < TTSZ){
       pline(tintxts[r].txt);
       lesshungry(tintxts[r].nut);
       if(r == 1)   /* SALMON */ {
      Glib = rnd(15);
      pline("Eating salmon made your fingers very slippery.");
       }
   } else {
       pline("It contains spinach - this makes you feel like Popeye!");
       lesshungry(600);
       if(u.ustr < 118)
      u.ustr += rnd( ((u.ustr < 17) ? 19 : 118) - u.ustr);
       if(u.ustr > u.ustrmax) u.ustrmax = u.ustr;
       flags.botl = 1;
   }
}

Meatdone(){
   u.usym = '@';
   prme();
}

doeat(){
   register struct obj *otmp;
   register struct objclass *ftmp;
   register int tmp;

   otmp = getobj("%", "eat");
   if(!otmp) return(0);
   if(otmp->otyp == TIN){
      if(uwep && (uwep->otyp == AXE || uwep->otyp == DAGGER ||
             uwep->otyp == CRYSKNIFE)){
         pline("Using your %s you try to open the tin",
            aobjnam(uwep, (char *) 0));
         tmp = 3;
      } else {
         pline("It is not so easy to open this tin.");
				if(Glib) {
					pline("The tin slips out of your hands.");
					dropx(otmp);
					return(1);
				}
				if(otmp->quan > 1) {
					register struct obj *obj;
					extern struct obj *splitobj();

					obj = splitobj(otmp, 1);
					if(otmp == uwep) setuwep(obj);
				}
         tmp = 2 + rn2(1 + 500/((int)(u.ulevel + u.ustr)));
      }
      if(tmp > 50){
         nomul(-50);
         nomovemsg="You give up your attempt to open the tin.";
      } else {
         nomul(-tmp);
         nomovemsg = "You succeed in opening the tin.";
         afternmv = tinopen;
         useup(otmp);
      }
      return(1);
   }
   ftmp = &objects[otmp->otyp];
   if(otmp->otyp >= CORPSE && eatcorpse(otmp)) goto eatx;
   if(!rn2(7) && otmp->otyp != FORTUNE_COOKIE) {
      pline("Blecch!  Rotten food!");
      if(!rn2(4)) {
         pline("You feel rather light headed.");
         Confusion += d(2,4);
      } else if(!rn2(4)&& !Blind) {
         pline("Everything suddenly goes dark.");
         Blind = d(2,10);
         seeoff(0);
      } else if(!rn2(3)) {
         if(Blind)
           pline("The world spins and you slap against the floor.");
         else
           pline("The world spins and goes dark.");
         nomul(-rnd(10));
         nomovemsg = "You are conscious again.";
      }
      lesshungry(ftmp->nutrition / 4);
   } else {
      multi = -ftmp->oc_delay;
      if(u.uhunger >= 1500) {
         pline("You choke over your food.");
         pline("You die...");
         killer = ftmp->oc_name;
         done("choked");
      }
      switch(otmp->otyp){
      case FOOD_RATION:
         if(u.uhunger <= 200)
            pline("That food really hit the spot!");
         else if(u.uhunger <= 700)
            pline("That satiated your stomach!");
         else {
	pline("You're having a hard time getting all that food down.");
            multi -= 2;
         }
         lesshungry(ftmp->nutrition);
         if(multi < 0) nomovemsg = "You finished your meal.";
         break;
      case TRIPE_RATION:
         pline("Yak - dog food!");
         u.uexp++;
         u.urexp += 4;
         flags.botl = 1;
			if(rn2(2)){
				pline("You vomit.");
				morehungry(20);
         } else   lesshungry(ftmp->nutrition);
         break;
      default:
         if(otmp->otyp >= CORPSE)
         pline("That %s tasted terrible!",ftmp->oc_name);
         else
         pline("That %s was delicious!",ftmp->oc_name);
         lesshungry(ftmp->nutrition);
#ifdef QUEST
         if(otmp->otyp == CARROT && !Blind){
            u.uhorizon++;
            setsee();
            pline("Your vision improves.");
         }
#endif QUEST
         if(otmp->otyp == FORTUNE_COOKIE) {
           if(Blind) {
             pline("This cookie has a scrap of paper inside!");
             pline("What a pity, that you cannot read it!");
           } else
             outrumor();
         }
         break;
      }
   }
eatx:
   if(multi<0 && !nomovemsg){
      static char msgbuf[BUFSZ];
      (void) sprintf(msgbuf, "You finished eating the %s.",
            ftmp->oc_name);
      nomovemsg = msgbuf;
   }
   useup(otmp);
   return(1);
}

/* called in hack.main.c */
gethungry(){
   --u.uhunger;
   if((Regeneration || Hunger) && moves%2) u.uhunger--;
	newuhs(TRUE);
}

/* called after vomiting and after performing feats of magic */
morehungry(num) register int num; {
	u.uhunger -= num;
	newuhs(TRUE);
}

/* called after eating something (and after drinking fruit juice) */
lesshungry(num) register int num; {
	u.uhunger += num;
	newuhs(FALSE);
}

unfaint(){
	u.uhs = FAINTING;
	flags.botl = 1;
}

newuhs(incr) boolean incr; {
	register int newhs, h = u.uhunger;

	newhs = (h > 1000) ? SATIATED :
		(h > 150) ? NOT_HUNGRY :
		(h > 50) ? HUNGRY :
		(h > 0) ? WEAK : FAINTING;

	if(newhs == FAINTING) {
		if(u.uhs == FAINTED)
			newhs = FAINTED;
		if(u.uhs <= WEAK || rn2(20-u.uhunger/10) >= 19) {
			if(u.uhs != FAINTED && multi >= 0 /* %% */) {
				pline("You faint from lack of food.");
				nomul(-10+(u.uhunger/10));
				nomovemsg = "You regain consciousness.";
				afternmv = unfaint;
				newhs = FAINTED;
			}
		} else
		if(u.uhunger < -(int)(200 + 25*u.ulevel)) {
			u.uhs = STARVED;
			flags.botl = 1;
			bot();
			pline("You die from starvation.");
			done("starved");
		}
	}

	if(newhs != u.uhs) {
		if(newhs >= WEAK && u.uhs < WEAK)
			losestr(1);
		else
		if(newhs < WEAK && u.uhs >= WEAK && u.ustr < u.ustrmax)
			losestr(-1);
		switch(newhs){
		case HUNGRY:
			pline((!incr) ? "You only feel hungry now." :
			      (u.uhunger < 145) ? "You feel hungry." :
				"You are beginning to feel hungry.");
			break;
		case WEAK:
			pline((!incr) ? "You feel weak now." :
			      (u.uhunger < 45) ? "You feel weak." :
				"You are beginning to feel weak.");
			break;
		}
		u.uhs = newhs;
		flags.botl = 1;
   }
}

/* returns 1 if some text was printed */
eatcorpse(otmp) register struct obj *otmp; {
register schar let = otmp->spe;
register int tp = 0;
   if(moves > otmp->age + 50 + rn2(100)) {
      tp++;
      pline("Ulch -- that meat was tainted!");
      pline("You get very sick.");
      Sick = 10 + rn2(10);
      u.usick_cause = objects[otmp->otyp].oc_name;
   } else if(index(POISONOUS, let) && rn2(5)){
      tp++;
      pline("Ecch -- that must have been poisonous!");
      if(!Poison_resistance){
         losehp(rnd(15), "poisonous corpse");
         losestr(rnd(4));
      } else
         pline("You don't seem affected by the poison.");
   } else if(index("ELNOPQRUuxz", let) && rn2(5)){
      tp++;
      pline("You feel sick.");
      losehp(rnd(8), "cadaver");
   }
   switch(let) {
   case 'L':
   case 'N':
   case 't':
      Teleportation |= INTRINSIC;
      break;
   case 'W':
      pluslvl();
      break;
   case 'n':
      u.uhp = u.uhpmax;
      flags.botl = 1;
      /* fall into next case */
   case '@':
      pline("You cannibal! You will be sorry for this!");
      /* not tp++; */
      /* fall into next case */
   case 'd':
      Aggravate_monster |= INTRINSIC;
      break;
   case 'I':
      See_invisible |= INTRINSIC;
      if(!Invis) newsym(u.ux, u.uy);
      Invis += 50;
      /* fall into next case */
   case 'y':
#ifdef QUEST
      u.uhorizon++;
#endif QUEST
      /* fall into next case */
   case 'B':
      Confusion = 50;
      break;
   case 'D':
      Fire_resistance |= INTRINSIC;
      break;
   case 'E':
      Telepat |= INTRINSIC;
      break;
   case 'F':
   case 'Y':
      Cold_resistance |= INTRINSIC;
      break;
   case 'k':
   case 's':
      Poison_resistance |= INTRINSIC;
      break;
   case 'c':
      pline("You turn to stone.");
      killer = "dead cockatrice";
      done("died");
   case 'M':
     pline("You cannot resist the temptation to mimic a treasure chest.");
     tp++;
     nomul(-30);
     afternmv = Meatdone;
     nomovemsg = "You now again prefer mimicking a human.";
     u.usym = '$';
     prme();
     break;
   }
   return(tp);
}

