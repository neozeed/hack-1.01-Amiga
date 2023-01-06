#include <stdio.h>
#include <libraries/dosextens.h>

int getpid()
   {
   long now[3];

   DateStamp(&now);

   return(now[0] ^ now[1] ^ now[2]);
   }

int *signal(num,func)
int num;
int *func;
   {
   return(NULL);
   }


getenv(var)
char *var;
   {
   return(NULL);
   }


execl()
   {
   /* this should flag an error */
   }


chdir(dir)
char *dir;
   {
   struct FileLock *lock;

   if ( (lock = Lock(dir, ACCESS_READ)) == NULL)
	return(1);  /* cannot find the directory */

   lock = CurrentDir( lock );

   if (lock)
	UnLock(lock);

   /* change to the desired directory */
   return(0);  /* phoney success */
   }


char *getlogin()
   {
   /* return the login name - perhaps we can use getenv */
   return (NULL);
   }

perror(string)
char *string;
   {
   myprintf("Call to perror for '%s'\n", string);
   }

char *index(p,c)
char *p;
char c;
   {
   char *strchr();

   return(strchr(p,c));
   }

char *rindex(p,c)
char *p;
char c;
   {
   char *strrchr();

   return(strrchr(p,c));
   }
