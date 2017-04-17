#include <vector>

#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "pfm.h"


// rc stands for return code
// usage: rc::SUCCESS

namespace rc {

    enum RC { 
        success = 0, 
        failure,
        file_already_exists, 
        file_does_not_exist, 
        file_create_error,
        file_delete_error,
        file_open_error,
        file_close_error,
        file_read_error, 
        file_write_error, 
        file_handle_in_use,
        last_rc  // This must be the last RC
    };
}

const vector<string> rc_msgs = {
   "Success",                      
   "error",                         
   "error: file already exists",  
   "error: file does not exist",
   "error: file create error",
   "error: file delete error",
   "error: file open error",
   "error: file close error",
   "error: file read error",
   "error: file write error",
   "error: file handle in use",
   "last return code"
};


#define RC_MSG(RCODE, FORMAT, ...) do { \
    eprintf("%s: %s %d: rc#%d: %s ", __FILE__, __func__, __LINE__, \
            RCODE, rc_msgs.at(RCODE).c_str()); \
    if (RCODE != rc::success) eprintf (FORMAT,__VA_ARGS__); } while(0)

//
// PRIVATE HELPER FUNCTIONS
//

bool existsFile(const char* cfname);
void rcprintf(int rc);

//
// PRIVATE HELPER FUNCTION DEFINITIONS
//

bool existsFile(const char* cfname) {
   struct stat buffer;
   return (stat (cfname, &buffer) == 0);
}

//
// MEMBER FUNCTION DEFINITIONS
//

PagedFileManager* PagedFileManager::_pf_manager = 0;


PagedFileManager* PagedFileManager::instance()
{
    if(!_pf_manager)
        _pf_manager = new PagedFileManager();

    return _pf_manager;
}


PagedFileManager::PagedFileManager()
{
}


PagedFileManager::~PagedFileManager()
{
}



RC PagedFileManager::createFile(const string &fileName)
{
   RC rcode = rc::success;
   const char* cfname = fileName.c_str();

   if (existsFile (cfname)) {
      rcode = rc::file_already_exists;
   } else {
      // creates/overwrites a new binary read/write file
      FILE* new_file = fopen (cfname, "wb+");
      if (new_file) {
         fclose(new_file);
      } else {
         rcode = rc::file_create_error;
      }
   } 
   RC_MSG (rcode, " [filename: \"%s\"]\n", cfname);
   return rcode;
}


RC PagedFileManager::destroyFile(const string &fileName)
{
   const char* cfname = fileName.c_str();
   if (remove (cfname) != 0) {
      RC_MSG (rc::file_delete_error, " [filename: \"%s\"]\n", cfname);
      return rc::file_delete_error;
   } 
   return rc::success;
}

//PRE: file with fileName must already exist
//PRE: fileHandle must not have another file open (aka it must be free)
RC PagedFileManager::openFile(const string &fileName, FileHandle &fileHandle)
{
   const char* cfname = fileName.c_str();
   RC rcode = rc::success;
   if (!existsFile (cfname)) {
      rcode = rc::file_does_not_exist;
   } else if (fileHandle.isInUse()) {
      rcode = rc::file_handle_in_use;
   } else {
      // opens existsing file in binary read/write mode 
      FILE* myfile = fopen (cfname, "rb+");
      if (myfile) {
         fileHandle.setFile(myfile);
      } else {
         rcode = rc::file_open_error;
      }
   }
   RC_MSG (rcode, " [filename = \"%s\"]\n", cfname);
   return rcode;
}


RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
    return -1;
}


FileHandle::FileHandle() : _file (NULL)
{
    
    readPageCounter = 0;
    writePageCounter = 0;
    appendPageCounter = 0;
}


FileHandle::~FileHandle()
{
}


inline bool FileHandle::isFree() {
   return _file == NULL;
}


inline bool FileHandle::isInUse() {
   return _file != NULL;
}


inline void FileHandle::setFile(FILE* file) {
   _file = file;
}


RC FileHandle::readPage(PageNum pageNum, void *data)
{
    return -1;
}


RC FileHandle::writePage(PageNum pageNum, const void *data)
{
    return -1;
}


RC FileHandle::appendPage(const void *data)
{
    return -1;
}


unsigned FileHandle::getNumberOfPages()
{
    return -1;
}


RC FileHandle::collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount)
{
    return -1;
}

//
// PUBLIC NONMEMBER FUNCTION DEFINITIONS 
//

void veprintf (const char* format, va_list args) {
   assert (format != NULL);
   fflush (NULL);
   vfprintf (stderr, format, args);
   fflush (NULL);
}

void eprintf (const char* format, ...) {
   va_list args;
   va_start (args, format);
   veprintf (format, args);
   va_end (args);
}

void syseprintf (const char* object) {
   eprintf ("%s: %s\n", object, strerror (errno));
}

void rcprintf(int rc) {
   if (rc) eprintf ("rc #%d: %s\n", rc, rc_msgs.at(rc).c_str());
}
