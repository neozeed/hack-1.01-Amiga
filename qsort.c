qsort( v, n, size, comp)
char *v;
int n;
int size;
int (*comp)();
   {
   int gap, i, j, x, cnt;
   char temp, *p1, *p2;

   cnt = 0;
   for (gap=n/2; gap > 0 ; gap /= 2)
      for (i=gap; i<n; i++)
         for (j = i-gap; j >= 0; j -= gap)
            if ( (*comp) ( (p1=v+j*size), (p2=v+(j+gap)*size) ) < 0)
               {
               cnt++;
               /* exchange them */
               for (x=0; x<size; x++)
                  {
                  temp = *p1;
                  *p1++ = *p2;
                  *p2++ = temp;
                  }
               }
   return(cnt);
   }
