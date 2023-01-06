#define TEXT(nam,str) struct IntuiText nam = {0,1,JAM2,0,0,NULL,str,NULL}
	/* Commands */
	TEXT(T_HELP,  "?   display help");
	TEXT(T_o,     "o   set options");
	TEXT(T_SHELL, "!   AMIGADOS commands");
	TEXT(T_v,     "v   version number");
	TEXT(T_CR,    "^R   redraw screen");
	TEXT(T_CP,    "^P   repeat last message");
	TEXT(T_Q,     "Q   quit game");
	TEXT(T_S,     "S   save the game");

	/* Inventory */
	TEXT(T_i,     "i   inventory");
	TEXT(T_p,     "p   pay your bill");
	TEXT(T_d,     "d   drop an object");
	TEXT(T_D,     "D   Drop several things");
	TEXT(T_COMMA, ",   Pickup an object");
	TEXT(T_SLASH, "/   identify something");
	TEXT(T_c,     "c   call class of objects");
	TEXT(T_C,     "C   Christen a monster");

	/* Actions */
	TEXT(T_a,    "a   apply/use something");
	TEXT(T_e,    "e   eat something");
	TEXT(T_q,    "q   quaff a potion");
	TEXT(T_r,    "r   read a scroll");
	TEXT(T_t,    "t   throw/shoot weapon");
	TEXT(T_z,    "z   zap a wand");

	/* Preparations */
	TEXT(T_w,    "w   wield a weapon");
	TEXT(T_P,    "P   Put on ring");
	TEXT(T_R,    "R   Remove ring");
	TEXT(T_T,    "T   Take off armor");
	TEXT(T_W,    "W   Wear armor");
	TEXT(T_WPN,   ")   current weapon");
	TEXT(T_ARMOR, "[   current armor");
	TEXT(T_RING,  "=   current rings");

	/* Movement */
	TEXT(T_E,     "E   Engrave msg on floor");
	TEXT(T_s,     "s   search");
	TEXT(T_UP,    "<   Go up stairs");
	TEXT(T_DOWN,  ">   Go down stairs");
	TEXT(T_WAIT,  ".   wait a moment");
	TEXT(T_CT,    "^T   Teleport");

#define IFLAGS ITEMENABLED|ITEMTEXT|HIGHCOMP
#define IDATA(str,off) 0,off,200,9,IFLAGS,0,(APTR)&str,NULL,NULL,NULL,NULL

struct MenuItem cmdsub[] = {
	{ &cmdsub[1], IDATA(T_HELP,   0) },
	{ &cmdsub[2], IDATA(T_o,     10) },
	{ &cmdsub[3], IDATA(T_SHELL, 20) },
	{ &cmdsub[4], IDATA(T_v,     30) },
	{ &cmdsub[5], IDATA(T_CR,    40) },
	{ &cmdsub[6], IDATA(T_CP,    50) },
	{ &cmdsub[7], IDATA(T_Q,     60) },
	{ NULL,       IDATA(T_S,     70) } };

struct MenuItem invsub[] = {
	{ &invsub[1], IDATA(T_i,      0) },
	{ &invsub[2], IDATA(T_p,     10) },
	{ &invsub[3], IDATA(T_d,     20) },
	{ &invsub[4], IDATA(T_D,     30) },
	{ &invsub[5], IDATA(T_COMMA, 40) },
	{ &invsub[6], IDATA(T_SLASH, 50) },
	{ &invsub[7], IDATA(T_c,     60) },
	{ NULL,       IDATA(T_C,     70) } };

struct MenuItem actsub[] = {
	{ &actsub[1], IDATA(T_a,    0) },
	{ &actsub[2], IDATA(T_e,    10) },
	{ &actsub[3], IDATA(T_q,    20) },
	{ &actsub[4], IDATA(T_r,    30) },
	{ &actsub[5], IDATA(T_t,    40) },
	{ NULL,       IDATA(T_z,    50) } };

struct MenuItem armsub[] = {
	{ &armsub[1], IDATA(T_w,      0) },
	{ &armsub[2], IDATA(T_P,     10) },
	{ &armsub[3], IDATA(T_R,     20) },
	{ &armsub[4], IDATA(T_T,     30) },
	{ &armsub[5], IDATA(T_W,     40) },
	{ &armsub[6], IDATA(T_WPN,   50) },
	{ &armsub[7], IDATA(T_ARMOR, 60) },
	{ NULL,       IDATA(T_RING,  70) } };

struct MenuItem movsub[] = {
	{ &movsub[1], IDATA(T_E,    0) },
	{ &movsub[2], IDATA(T_s,    10) },
	{ &movsub[3], IDATA(T_UP,   20) },
	{ &movsub[4], IDATA(T_DOWN, 30) },
	{ &movsub[5], IDATA(T_WAIT, 40) },
	{ NULL,       IDATA(T_CT,   50) } };

struct Menu HackMenu[] = {
   { &HackMenu[1], 10,0, 80,0,MENUENABLED,"Commands",     &cmdsub[0] },
   { &HackMenu[2], 90,0, 80,0,MENUENABLED,"Inventory",    &invsub[0] },
   { &HackMenu[3],180,0, 80,0,MENUENABLED,"Actions",      &actsub[0] },
   { &HackMenu[4],270,0,100,0,MENUENABLED,"Preparations", &armsub[0] },
   { NULL,        390,0, 80,0,MENUENABLED,"Movement",     &movsub[0] } };

char menukey[5][10] = {
	{
	'?',   /*   display help */
	'o',   /*   set options */
	'!',   /*   AMIGADOS commands */
	'v',   /*   version number */
	022,   /*R   redraw screen */
	024,   /*P   repeat last message */
	'Q',   /*   quit game */
	'S',   /*   save the game */
	},{
	/* Inventory */
	'i',   /*   inventory */
	'p',   /*   pay your bill */
	'd',   /*   drop an object */
	'D',   /*   Drop several things */
	',',   /*   Pickup an object */
	'/',   /*   identify something */
	'c',   /*   call a class of objects */
	'C',   /*   Christen a monster */
	},{
	/* Actions */
	'a',   /*   apply/use something */
	'e',   /*   eat something */
	'q',   /*   quaff a potion */
	'r',   /*   read a scroll */
	't',   /*   throw/shoot weapon */
	'z',   /*   zap a wand */
	},{
	/* Preparations */
	'w',   /*   wield a weapon */
	'P',   /*   Put on ring */
	'R',   /*   Remove ring */
	'T',   /*   Take off armor */
	'W',   /*   Wear armor */
	')',   /*   current weapon */
	'[',   /*   current armor */
	'=',   /*   current rings */
	},{
	/* Movement */
	'E',   /*   Engrave msg on floor */
	's',   /*   search */
	'<',   /*   Go up stairs */
	'>',   /*   Go down stairs */
	'.',   /*   wait a moment */
	024,   /*   Teleport */
	} };
