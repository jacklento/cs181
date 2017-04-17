//p1test.cc -- unit tests for project 1

#include <iostream>
#include <string>
#include <cassert>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <stdio.h>
#include <fstream>

#include "pfm.h"
#include "rbfm.h"

using namespace std;

int main() {
   PagedFileManager* pfm = PagedFileManager::instance();
   FileHandle fh;

   string sfname = "00_exist.t";
   const char* cfname = sfname.c_str(); 
   int rc = pfm->createFile (sfname);
   printf("test_00: createFile(%s) returned: %d.\n", 
          cfname, rc);

   sfname = "01_create.t";
   cfname = sfname.c_str();
   rc = pfm->createFile (sfname);
   printf("test_01: createFile(%s) returned: %d.\n", cfname, rc);
   
   sfname = "02_destroy.t";
   cfname = sfname.c_str();
   rc = pfm->destroyFile (sfname);
   printf("test_01: destroyFile(%s) returned: %d.\n", cfname, rc);

   sfname = "03_00_no_exist.t"; 
   cfname = sfname.c_str();
   rc = pfm->openFile (sfname, fh);
   printf("test_03_00: openFile(%s) returned: %d.\n", cfname, rc);

   cout << "done" << endl;
   return 0;
}
