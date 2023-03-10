/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.do_wear.c version 1.0.1 - changed an int to long */

#include "hack.h"
#include <stdio.h>
extern char *nomovemsg;

off_msg(otmp) register struct obj *otmp; {
	pline("You were wearing %s.", doname(otmp));
}

doremarm() {
	register struct obj *otmp;
	if(!uarm && !uarmh && !uarms && !uarmg) {
		pline("Not wearing any armor.");
		return(0);
	}
	otmp = (!uarmh && !uarms && !uarmg) ? uarm :
		(!uarms && !uarm && !uarmg) ? uarmh :
		(!uarmh && !uarm && !uarmg) ? uarms :
		(!uarmh && !uarm && !uarms) ? uarmg :
		getobj("[", "take off");
	if(!otmp) return(0);
	if(!(otmp->owornmask & (W_ARMOR - W_ARM2))) {
		pline("You can't take that off.");
		return(0);
	}
	(void) armoroff(otmp);
	return(1);
}

doremring() {
	if(!uleft && !uright){
		pline("Not wearing any ring.");
		return(0);
	}
	if(!uleft)
		return(dorr(uright));
	if(!uright)
		return(dorr(uleft));
	if(uleft && uright) while(1) {
		pline("What ring, Right or Left? ");
		switch(readchar()) {
		case ' ':
		case '\n':
		case '\033':
			return(0);
		case 'l':
		case 'L':
			return(dorr(uleft));
		case 'r':
		case 'R':
			return(dorr(uright));
		}
	}
	/* NOTREACHED */
#ifdef lint
	return(0);
#endif lint
}

dorr(otmp) register struct obj *otmp; {
	if(cursed(otmp)) return(0);
	ringoff(otmp);
	off_msg(otmp);
	return(1);
}

cursed(otmp) register struct obj *otmp; {
	if(otmp->cursed){
		pline("You can't. It appears to be cursed.");
		return(1);
	}
	return(0);
}

armoroff(otmp) register struct obj *otmp; {
register int delay = -objects[otmp->otyp].oc_delay;
	if(cursed(otmp)) return(0);
	setworn((struct obj *) 0, otmp->owornmask & W_ARMOR);
	if(delay) {
		nomul(delay);
		switch(otmp->otyp) {
		case HELMET:
			nomovemsg = "You finished taking off your helmet.";
			break;
		case PAIR_OF_GLOVES:
			nomovemsg = "You finished taking off your gloves";
			break;
		default:
			nomovemsg = "You finished taking off your suit.";
		}
	} else {
		off_msg(otmp);
	}
	return(1);
}

doweararm() {
	register struct obj *otmp;
	register int delay;
	register int err = 0;
	long mask = 0;

	otmp = getobj("[", "wear");
	if(!otmp) return(0);
	if(otmp->owornmask & W_ARMOR) {
		pline("You are already wearing that!");
		return(0);
	}
	if(otmp->otyp == HELMET){
		if(uarmh) {
			pline("You are already wearing a helmet.");
			err++;
		} else
			mask = W_ARMH;
	} else if(otmp->otyp == SHIELD){
		if(uarms) pline("You are already wearing a shield."), err++;
		if(uwep && uwep->otyp == TWO_HANDED_SWORD)
	pline("You cannot wear a shield and wield a two-handed sword."), err++;
		if(!err) mask = W_ARMS;
	} else if(otmp->otyp == PAIR_OF_GLOVES){
		if(uarmg) pline("You are already wearing gloves."); else
		if(uwep && uwep->cursed)
			pline("You cannot wear gloves over your weapon.");
		else mask = W_ARMG;
	} else {
		if(uarm) {
			if(otmp->otyp != ELVEN_CLOAK || uarm2) {
				pline("You are already wearing some armor.");
				err++;
			}
		}
		if(!err) mask = W_ARM;
	}
	if(err) return(0);
	setworn(otmp, mask);
	if(otmp == uwep)
		setuwep((struct obj *) 0);
	delay = -objects[otmp->otyp].oc_delay;
	if(delay){
		nomul(delay);
		nomovemsg = "You finished your dressing manoeuvre.";
	}
	otmp->known = 1;
	return(1);
}

dowearring() {
	register struct obj *otmp;
	long mask = 0;
	long oldprop;

	if(uleft && uright){
		pline("There are no more ring-fingers to fill.");
		return(0);
	}
	otmp = getobj("=", "wear");
	if(!otmp) return(0);
	if(otmp->owornmask & W_RING) {
		pline("You are already wearing that!");
		return(0);
	}
	if(otmp == uleft || otmp == uright) {
		pline("You are already wearing that.");
		return(0);
	}
	if(uleft) mask = RIGHT_RING;
	else if(uright) mask = LEFT_RING;
	else do {
 		pline("What ring-finger, Right or Left? ");
		switch(readchar()){
		case 'l':
		case 'L':
			mask = LEFT_RING;
			break;
		case 'r':
		case 'R':
			mask = RIGHT_RING;
			break;
		case ' ':
		case '\n':
		case '\033':
			return(0);
		}
	} while(!mask);
	setworn(otmp, mask);
	if(otmp == uwep)
		setuwep((struct obj *) 0);
	oldprop = u.uprops[PROP(otmp->otyp)].p_flgs;
	u.uprops[PROP(otmp->otyp)].p_flgs |= mask;
	switch(otmp->otyp){
	case RIN_LEVITATION:
		if(!oldprop) float_up();
		break;
	case RIN_PROT_SHAPE_CHANGERS:
		rescham();
		break;
	case RIN_GAIN_STRENGTH:
		u.ustr += otmp->spe;
		u.ustrmax += otmp->spe;
		flags.botl=1;
		break;
	case RIN_INCREASE_DAMAGE:
		u.udaminc += otmp->spe;
		break;
	}
	prinv(otmp);
	return(1);
}

ringoff(obj)
register struct obj *obj;
{
register long mask;
	mask = obj->owornmask & W_RING;
	setworn((struct obj *) 0, obj->owornmask);
	if(!(u.uprops[PROP(obj->otyp)].p_flgs & mask)){
		pline("Strange... I didnt know you had that ring.");
		impossible();
	}
	u.uprops[PROP(obj->otyp)].p_flgs &= ~mask;
	switch(obj->otyp) {
	case RIN_LEVITATION:
		if(!Levitation) {	/* no longer floating */
			float_down();
		}
		break;
	case RIN_GAIN_STRENGTH:
		u.ustr -= obj->spe;
		u.ustrmax -= obj->spe;
		flags.botl = 1;
		break;
	case RIN_INCREASE_DAMAGE:
		u.udaminc -= obj->spe;
		break;
	}
}

find_ac(){
register int uac = 10;
	if(uarm) uac -= uarm->spe;
	if(uarm2) uac -= uarm2->spe;
	if(uarmh) uac -= uarmh->spe;
	if(uarms) uac -= uarms->spe;
	if(uarmg) uac -= uarmg->spe;
	if(uleft && uleft->otyp == RIN_PROTECTION) uac -= uleft->spe;
	if(uright && uright->otyp == RIN_PROTECTION) uac -= uright->spe;
	if(uac != u.uac){
		u.uac = uac;
		flags.botl = 1;
	}
}

glibr(){
register struct obj *otmp;
int xfl = 0;
	if(!uarmg) if(uleft || uright) {
		/* Note: at present also cursed rings fall off */
		pline("Your %s off your fingers.",
			(uleft && uright) ? "rings slip" : "ring slips");
		xfl++;
		if(otmp = uleft){
			ringoff(uleft);
			dropx(otmp);
		}
		if(otmp = uright){
			ringoff(uright);
			dropx(otmp);
		}
	}
	if(otmp = uwep){
		/* Note: at present also cursed weapons fall */
		setuwep((struct obj *) 0);
		dropx(otmp);
		pline("Your weapon %sslips from your hands.",
			xfl ? "also " : "");
	}
}

struct obj *
some_armor(){
register struct obj *otmph = uarm;
	if(uarmh && (!otmph || !rn2(4))) otmph = uarmh;
	if(uarmg && (!otmph || !rn2(4))) otmph = uarmg;
	if(uarms && (!otmph || !rn2(4))) otmph = uarms;
	return(otmph);
}

corrode_armor(){
register struct obj *otmph = some_armor();
	if(otmph){
		if(otmph->rustfree ||
		   otmph->otyp == ELVEN_CLOAK ||
		   otmph->otyp == LEATHER_ARMOR ||
		   otmph->otyp == STUDDED_LEATHER_ARMOR) {
			pline("Your %s not affected!",
				aobjnam(otmph, "are"));
			return;
		}
		pline("Your %s!", aobjnam(otmph, "corrode"));
		otmph->spe--;
	}
}
