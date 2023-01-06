#include <workbench/startup.h>
#include <workbench/icon.h>
#include <workbench/workbench.h>

extern struct WBStartup *WBenchMsg;
extern char pl_character[];
extern char plname[];

geticon()
   {
   struct WBArg *argp;
   char *argname;

   argp = WBenchMsg->sm_ArgList;
   if (WBenchMsg->sm_NumArgs > 1)
	{
	argname = (argp+1)->wa_Name;
	pl_character[0] = pl_character[1] = 0;
	/* argp now points to the name */
	if      (!strcmp(argname, "Wizard"      )) pl_character[0] = 'W';
	else if (!strcmp(argname, "Speliologist")) pl_character[0] = 'S';
	else if (!strcmp(argname, "Tourist"     )) pl_character[0] = 'T';
	else if (!strcmp(argname, "Fighter"     )) pl_character[0] = 'F';
	else if (!strcmp(argname, "Knight"      )) pl_character[0] = 'K';
	else if (!strcmp(argname, "Caveman"     )) pl_character[0] = 'C';
	else strcpy(plname, argname);
	}
   }

makeicon(name,type)
char *name;
char type;
   {
   char *source;
   struct DiskObject *object;

   switch(type) {
	case 'w':
	case 'W':	source = "Wizard";
			break;
	case 's':
	case 'S':	source = "Speliologist";
			break;
	case 't':
	case 'T':	source = "Tourist";
			break;
	case 'f':
	case 'F':	source = "Fighter";
			break;
	case 'k':
	case 'K':	source = "Knight";
			break;
	case 'c':
	case 'C':	source = "Caveman";
			break;

	default:	source = "HACK";
			break;
	}
	if ( (object = GetDiskObject(source)) == NULL)
		myprintf("Cannot get source icon - err #%d\n", IoErr() );
	object->do_CurrentX = NO_ICON_POSITION;
	object->do_CurrentY = NO_ICON_POSITION;
	if ( (object = PutDiskObject( name, object )) == NULL)
		myprintf("Cannot create save icon - err #%d\n", IoErr() );
	FreeDiskObject(object);
   }

delicon(name)
   {
   struct WBObject *object;
   char tempname[100];  /* should hold any file name */

   strcpy(tempname,name);
   strcat(tempname,".info");
   if ( unlink(tempname) == -1)
	myprintf("Cannot delete .info file\n");
   }
