/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */



struct edog {

	long hungrytime;	/* at this time dog gets hungry */

	long eattime;		/* dog is eating */

	long droptime;		/* moment dog dropped object */

	unsigned dropdist;		/* dist of drpped obj from @ */

	unsigned apport;		/* amount of training */

	long whistletime;		/* last time he whistled */

};

#define	EDOG(mp)	((struct edog *)(&(mp->mextra[0])))

