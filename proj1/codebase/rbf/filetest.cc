#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main() {
   
   FILE* fstrm = fopen("empty.t", "w+");
   char* word = (char*)malloc(10);
   snprintf (word, 10, "word");
   fseek (fstrm, 0, SEEK_SET);
   fwrite (word, 1, 10, fstrm);
   fflush (fstrm);
   fclose (fstrm);
 
   struct stat st;
   stat ("empty.t", &st); 
   printf("st_size = %d\n", st.st_size);

   fstrm = fopen("empty.t", "rb+");

 /*  //1-----------
   char buffer[100];

   int fread_rc = fread(buffer, 1, 10, fstrm);
   printf("fread = %d\nfeof = %d\nferror = %d\n", fread_rc, feof(fstrm), ferror(fstrm));
   printf("buffer = %s\n", buffer);
   //-1----------  
*/

   //2-----------
   char* buffer = (char*)malloc(100);

   int fread_rc = fread(buffer, 1, 10, fstrm);
   printf("fread = %d\nfeof = %d\nferror = %d\n", fread_rc, feof(fstrm), ferror(fstrm));
   printf("buffer = %s\n", buffer);
   free(buffer);
   //-2----------

   fclose(fstrm);
  
   printf("done\n");
   return 0;

}
