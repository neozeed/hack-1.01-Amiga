/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include <exec/types.h>
#include <exec/io.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include "hack.h"   /* for ROWNO and COLNO */
#include "def.menus.h"

#define XSIZE 8
#define YSIZE 8
#define BASEX 4
#define BASEY 4   /* should be -4 */

#define ICON_REV       0
#define GRAPHICS_REV   29
#define INTUITION_REV  29

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
char *IconBase;
int mousex, mousey;

#define BUFFERED 512
char out_buffer[BUFFERED];
int bufcnt;
char *out_bufp;

#ifdef STUPIDARROWS
#include "def.arrows.h"
struct Image URImage = { 0,0,22,10,2,(short *)&UPRIGHTdata,   0x3, 0, NULL };
struct Image DRImage = { 0,0,22,10,2,(short *)&DOWNRIGHTdata, 0x3, 0, NULL };
struct Image RImage  = { 0,0,22,10,2,(short *)&RIGHTdata,     0x3, 0, NULL };
struct Image DImage  = { 0,0,22,10,2,(short *)&DOWNdata,      0x3, 0, NULL };
struct Image UImage  = { 0,0,22,10,2,(short *)&UPdata,        0x3, 0, NULL };
struct Image LImage  = { 0,0,22,10,2,(short *)&LEFTdata,      0x3, 0, NULL };
struct Image DLImage = { 0,0,22,10,2,(short *)&DOWNLEFTdata,  0x3, 0, NULL };
struct Image ULImage = { 0,0,22,10,2,(short *)&UPLEFTdata,    0x3, 0, NULL };

struct Gadget URGadget = { NULL,      436, 0, 22,10,
			GADGHCOMP | GADGIMAGE, GADGIMMEDIATE, BOOLGADGET,
			(APTR)&URImage, NULL, NULL, 0, NULL, 'u', NULL };
struct Gadget DRGadget = { &URGadget, 568, 0, 22,10,
			GADGHCOMP | GADGIMAGE, GADGIMMEDIATE, BOOLGADGET,
			(APTR)&DRImage, NULL, NULL, 0, NULL, 'n', NULL };
struct Gadget RGadget  = { &DRGadget, 490, 0, 22,10,
			GADGHCOMP | GADGIMAGE, GADGIMMEDIATE, BOOLGADGET,
			(APTR)&RImage, NULL, NULL, 0, NULL, 'l', NULL };
struct Gadget DGadget  = { &RGadget,  544, 0, 22,10,
			GADGHCOMP | GADGIMAGE, GADGIMMEDIATE, BOOLGADGET,
			(APTR)&DImage, NULL, NULL, 0, NULL, 'j', NULL };
struct Gadget UGadget  = { &DGadget,  412, 0, 22,10,
			GADGHCOMP | GADGIMAGE, GADGIMMEDIATE, BOOLGADGET,
			(APTR)&UImage, NULL, NULL, 0, NULL, 'k', NULL };
struct Gadget LGadget  = { &UGadget,  466, 0, 22,10,
			GADGHCOMP | GADGIMAGE, GADGIMMEDIATE, BOOLGADGET,
			(APTR)&LImage, NULL, NULL, 0, NULL, 'h', NULL };
struct Gadget DLGadget = { &LGadget,  520, 0, 22,10,
			GADGHCOMP | GADGIMAGE, GADGIMMEDIATE, BOOLGADGET,
			(APTR)&DLImage, NULL, NULL, 0, NULL, 'b', NULL };
struct Gadget ULGadget = { &DLGadget, 388, 0, 22,10,
			GADGHCOMP | GADGIMAGE, GADGIMMEDIATE, BOOLGADGET,
			(APTR)&ULImage, NULL, NULL, 0, NULL, 'y', NULL };
#endif

struct TextAttr HackFont =
	{ "topaz.font", TOPAZ_EIGHTY,FS_NORMAL, FPF_ROMFONT};

struct NewScreen NewHackScreen = {
0, 0, 640, 200, 3, 0, 1, HIRES, CUSTOMSCREEN, &HackFont,
"HACK V1.0.1a - Ported by  John A. Toebes, VIII", NULL, NULL };

struct Window *HackWindow;
struct Screen *HackScreen;

struct NewWindow NewHackWindow = {
  0,1,640,199, -1,-1,   /* left, top, width, height, detailpen, blockpen */
  MOUSEBUTTONS | CLOSEWINDOW | RAWKEY | MENUPICK
#ifdef STUPIDARROWS
 | GADGETDOWN
#endif
  ,WINDOWDEPTH | WINDOWCLOSE | ACTIVATE | SIMPLE_REFRESH,
#ifdef STUPIDARROWS
  &ULGadget,
#else
  NULL,
#endif
NULL, "HACK V1.0.1a - Ported by John A. Toebes, VIII",
  NULL, NULL, 640,200,640,200, CUSTOMSCREEN };

struct IOStdReq consoleIO;

#define HO "\x9BH"
#define CL "\x0C"
#define CE "\x9BK"
#define UP "\x0B"
#define CM "\x9B%d;%dH"
#define ND "\x09"
#define XD "\x9BB"
#define BC "\x08"
#define SO "\x9B4m"
#define SE "\x9B0m"
#define BELL 7
int myx, myy;

startup()
{
}

/* Cursor movements */
extern xchar curx, cury;

curs(x,y)
register int x,y;   /* not xchar: perhaps xchar is unsigned and
            curx-x would be unsigned as well */
{
   if (y != cury || x != curx)
	myprintf(CM, y, x);
   curx = x;
   cury = y;
}

cl_end() {
	myprintf(CE);
}

clear_screen() {
	myprintf(CL);
	curx = cury = 1;
}

home()
{
	myprintf(HO);
	curx = cury = 1;
}

standoutbeg()
{
	myprintf(SO);
}

standoutend()
{
	myprintf(SE);
}

backsp()
{
	myprintf(BC);
	curx--;
}

bell()
{
	myputchar(BELL);
}

delay_output()
{
   /* delay 40 ms, 50 ticks/sec    */
/*   Delay (2); */
}

initterm()
   {
#ifdef DEBUGIT
	printf("intuition.library?\n");
	fflush(stdout);
#endif
   if ( (IntuitionBase = (struct IntuitionBase *)
	OpenLibrary("intuition.library", INTUITION_REV)) == NULL)
	_exit(2);

#ifdef DEBUGIT
	printf("graphics.library?\n");
	fflush(stdout);
#endif
   if ( (GfxBase = (struct GfxBase *)
	OpenLibrary("graphics.library",GRAPHICS_REV)) == NULL)
	_exit(3);

#ifdef DEBUGIT
	printf("icon.library?\n");
	fflush(stdout);
#endif
   if ( (IconBase = (char *)
	OpenLibrary("icon.library",ICON_REV)) == NULL)
	_exit(4);

   if ( (HackScreen = (struct Screen *)
	OpenScreen(&NewHackScreen)) == NULL)
	_exit(5);

   NewHackWindow.Screen = HackScreen;

#ifdef DEBUGIT
	printf("OpenWindow?\n");
	fflush(stdout);
#endif
   if ( (HackWindow = (struct Window *)
	OpenWindow(&NewHackWindow)) == NULL)
	{
	CloseScreen(HackScreen);
	_exit(6);
	}

#ifdef DEBUGIT
	printf("menu strip?\n");
	fflush(stdout);
#endif
   SetMenuStrip(HackWindow,&HackMenu);

#ifdef DEBUGIT
	printf("console.device?\n");
	fflush(stdout);
#endif
   consoleIO.io_Data = (APTR) HackWindow;
   consoleIO.io_Length = sizeof(*HackWindow);
   if (OpenDevice("console.device",0, &consoleIO, 0) != 0)
	hackexit(7);

#ifdef DEBUGIT
	printf("doneinit\n");
	fflush(stdout);
#endif
   bufcnt = 0;
   out_bufp = out_buffer;
   }

hackexit(code)
int code;
   {
   CloseDevice(&consoleIO);
   ClearMenuStrip(HackWindow);
   CloseWindow(HackWindow);
   CloseScreen(HackScreen);
   CloseLibrary(IconBase);
   CloseLibrary(GfxBase);
   CloseLibrary(IntuitionBase);
   _exit(code);
   }

myfflush()
   {
	register int dummy1, dummy2;
   if (bufcnt)
	{
	consoleIO.io_Command = CMD_WRITE;
	consoleIO.io_Data = (APTR)out_buffer;
	consoleIO.io_Length = bufcnt;
	DoIO(&consoleIO);
	}
   bufcnt = 0;
   }

myputchar(c)
char c;
   {
	if (bufcnt == BUFFERED)
		myfflush();

	out_buffer[bufcnt++] = c;
   }

myputs(str)
char *str;
   {
	register int dummy1, dummy2;
	int len, tocopy;

	len = strlen(str);

	if (len >= BUFFERED)
		{
		myfflush();
		consoleIO.io_Command = CMD_WRITE;
		consoleIO.io_Data = (APTR)str;
		consoleIO.io_Length = len;
		DoIO(&consoleIO);
		}
	else
		{
		if (bufcnt+len >= BUFFERED) /* is there room */
			{
			tocopy = BUFFERED - bufcnt;
			movmem(str, &out_buffer[bufcnt], tocopy);
			bufcnt += tocopy;
			len -= tocopy;
			str += tocopy;
			myfflush();
			}
		if (len)
			{
			/* just move it in */
			movmem(str, &out_buffer[bufcnt], len);
			bufcnt += len;
			}
		}
	myputchar('\n');
   }

/*VARARGS1*/
myprintf(str,a1,a2,a3,a4,a5,a6,a7,a8,a9)
char *str,*a1,*a2,*a3,*a4,*a5,*a6,*a7,*a8,*a9;
   {
	char buf[BUFFERED], *bptr;
	int len, tocopy;

	bptr = &buf;
	len = (int)sprintf(bptr,str,a1,a2,a3,a4,a5,a6,a7,a8,a9);
	if (bufcnt+len >= BUFFERED) /* is there room */
		{
		tocopy = BUFFERED - bufcnt;
		movmem(bptr, &out_buffer[bufcnt], tocopy);
		bufcnt += tocopy;
		len -= tocopy;
		bptr += tocopy;
		myfflush();
		}
	if (len)
		{
		/* just move it in */
		movmem(bptr, &out_buffer[bufcnt], len);
		bufcnt += len;
		}
   }

inchar()
   {
	register int dummy1, dummy2;
	struct IntuiMessage *Message, *GetMsg();
	int c;
	USHORT thismenu, menusel;
#ifdef STUPIDARROWS
   struct Gadget *gadget;
#endif
   struct MenuItem *item, *ItemAddress();
   c = 0;
   while(!c)
      {
      while( (Message = GetMsg(HackWindow->UserPort)) == NULL)
         Wait( 1 << HackWindow->UserPort->mp_SigBit );

      switch(Message->Class)
	{
	case MENUPICK:
		menusel = thismenu = Message->Code;
		while(thismenu != MENUNULL)
		   {
		   menusel = thismenu;
		   item = ItemAddress(&HackMenu, thismenu);
		   thismenu = item->NextSelect;
		   }
		if (menusel != MENUNULL)
		   c = menukey[MENUNUM(menusel)][ITEMNUM(menusel)];
		break;
	case MOUSEBUTTONS:
		mousex = ( (Message->MouseX) + BASEX ) / XSIZE;
 		mousey = ( (Message->MouseY) - BASEY ) / YSIZE;
		if (mousex > 0 && mousey > 0 &&
		    mousex <= COLNO && mousey <= ROWNO )
			{
			if (Message->Code == SELECTDOWN)
				c = MDOWN;
			else if (Message->Code == SELECTUP)
				c = MUP;
			}
		break;
	case CLOSEWINDOW:
		c = 'Q';
		break;
#ifdef STUPIDARROWS
	case GADGETDOWN:
		gadget = (struct Gadget *)Message->IAddress;
		c = gadget->GadgetID;
		break;
#endif
	case RAWKEY:
		c = cnvrtkey(Message->Code,Message->Qualifier);
		break;
	default:
		c = 'Q';
		break;
	}
      ReplyMsg(Message);
      }
   return(c);
   }

#define NORMAL 0
#define SHIFTED 1
#define CONTROL 2
#define ALTED 3
short lookup[4][96] =
{
/* unshifted table */
	'`',	'1',	'2',	'3',	'4',	'5',	'6',	'7',
	'8',	'9',	'0',	'-',	'=',	'\\',	0,	'0',
	'q',	'w',	'e',	'r',	't',	'y',	'u',	'i',
	'o',	'p',	'[',	']',	0,	'b',	'j',	'n',
	'a',	's',	'd',	'f',	'g',	'h',	'j',	'k',
	'l',	';',	'\'',	0,	0,	'h',	'.',	'l',
	0,	'z',	'x',	'c',	'v',	'b',	'n',	'm',
	',',	'.',	'/',	0,	'.',	'y',	'k',	'u',
	' ',	8,	'i',	'\n',	'\n',	022,	8,	0,
	0,	0,	'-',	0,	'k',	'j',	'l',	'h',
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	'?',

/* shifted table */
	'~',	'!',	'@',	'#',	'$',	'%',	'^',	'&',
	'*',	'(',	')',	'_',	'+',	'|',	0,	'0',
	'Q',	'W',	'E',	'R',	'T',	'Y',	'U',	'I',
	'O',	'P',	'{',	'}',	0,	'B',	'J',	'N',
	'A',	'S',	'D',	'F',	'G',	'H',	'J',	'K',
	'L',	':',	'"',	0,	0,	'H',	'.',	'L',
	0,	'Z',	'X',	'C',	'V',	'B',	'N',	'M',
	'<',	'>',	'?',	0,	'.',	'Y',	'K',	'U',
	' ',	'H',	'I',	'\N',	'\N',	022,	'H',	0,
	0,	0,	'-',	0,	'K',	'J',	'L',	'H',
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	'?',

/* controlled table */
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	'Q',	0,	0,
	0,	0,	0,	022,	024,	0,	0,	0,
	0,	020,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	'?',

/* alted table */
	'`',	'1',	'2',	'3',	'4',	'5',	'6',	'7',
	'8',	'9',	'0',	'-',	'=',	'\\',	0,	'0',
	'q',	'w',	'e',	'r',	't',	'y',	'u',	'i',
	'o',	'p',	'[',	']',	0,	'b',	'j',	'n',
	'a',	's',	'd',	'f',	'g',	'h',	'j',	'k',
	'l',	';',	'\'',	0,	0,	'h',	'.',	'l',
	0,	'z',	'x',	'c',	'v',	'b',	'n',	'm',
	',',	'.',	'?',	0,	'.',	'y',	'k',	'u',
	' ',	'h',	'i',	'\n',	'\n',	022,	'h',	0,
	0,	0,	'-',	0,	'k',	'j',	'l',	'h',
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	'?'
};

int cnvrtkey(code, qual )
USHORT code, qual;
   {
   int table;

   if (code > 0x5f)
      return(0);

   if (qual & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
      table = SHIFTED;
   else if (qual & (IEQUALIFIER_LALT | IEQUALIFIER_RALT))
       table = ALTED;
   else if (qual & (IEQUALIFIER_CONTROL))
      table = CONTROL;
   else
      table = NORMAL;
   return((int)lookup[table][code]);
   }
