/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.wield.c version 1.0.1 - evaporate%s */

#include	"hack.h"

setuwep(obj) register struct obj *obj; {
	setworn(obj, W_WEP);
}

dowield()
{
	register struct obj *wep;
	register int res = 0;

	multi = 0;
	if(!(wep = getobj("#-)", "wield"))) /* nothing */;
	else if(uwep == wep)
		pline("You are already wielding that!");
	else if(uwep && uwep->cursed)
		pline("The %s welded to your hand!",
			aobjnam(uwep, "are"));
	else if((int) wep == 1) {
		if(uwep == 0){
			pline("You are already empty handed.");
		} else {
			setuwep((struct obj *) 0);
			res++;
			pline("You are empty handed.");
		}
	} else if(uarms && wep->otyp == TWO_HANDED_SWORD)
	pline("You cannot wield a two-handed sword and wear a shield.");
	else if(wep->owornmask & (W_ARMOR | W_RING))
		pline("You cannot wield that!");
	else {
		setuwep(wep);
		res++;
		if(uwep->cursed) pline("The %s itself to your hand!",
			aobjnam(uwep, "weld"));
		else prinv(uwep);
	}
	return(res);
}

corrode_weapon(){
	if(!uwep || uwep->olet != WEAPON_SYM) return;	/* %% */
	if(uwep->rustfree)
		pline("Your %s not affected.", aobjnam(uwep, "are"));
	else {
		pline("Your %s!", aobjnam(uwep, "corrode"));
		uwep->spe--;
	}
}

chwepon(otmp,amount)
register struct obj *otmp;
register int amount;
{
register char *color = (amount < 0) ? "black" : "green";
register char *time;
	if(!uwep || uwep->olet != WEAPON_SYM) {
		strange_feeling(otmp);
		return(0);
	}

	if(uwep->otyp == WORM_TOOTH && amount > 0) {
		uwep->otyp = CRYSKNIFE;
		pline("Your weapon seems sharper now.");
		uwep->cursed = 0;
		return(1);
	}

	if(uwep->otyp == CRYSKNIFE && amount < 0) {
		uwep->otyp = WORM_TOOTH;
		pline("Your weapon looks duller now.");
		return(1);
	}

	/* there is a (soft) upper limit to uwep->spe */
	if(amount > 0 && uwep->spe > 5 && rn2(3)) {
	    pline("Your %s violently green for a while and then evaporate%s.",
		aobjnam(uwep, "glow"), (uwep->quan == 1) ? "s" : "");
	    useup(uwep);
	    return(1);
	}
	if(!rn2(6)) amount *= 2;
	time = (amount*amount == 1) ? "moment" : "while";
	pline("Your %s %s for a %s.",
		aobjnam(uwep, "glow"), color, time);
	uwep->spe += amount;
	if(amount > 0) uwep->cursed = 0;
	return(1);
}
