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
   string filename = "00_exist.t";
   const char* cfname = filename.c_str(); 
   int rc = pfm->createFile (cfname);
   printf("test_00: creatFile(%s) returned: %d.\n", 
          cfname, rc);

   filename = "01_create.t";
   cfname = filename.c_str();
   rc = pfm->createFile (cfname);
   printf("test_01: createFile(%s) returned: %d.\n", cfname, rc);
   
   filename = "02_destroy.t";
   cfname = filename.c_str();
   rc = pfm->destroyFile (cfname);
   printf("test_01: destroyFile(%s) returned: %d.\n", cfname, rc);

   cout << "done" << endl;
   return 0;
}
