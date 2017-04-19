#include <vector>

#include <cmath> // ceil
#include <assert.h> // assert
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // stat

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
        file_handle_empty,
        page_does_not_exist,
        incomplete_page_read,
        incomplete_page_write,
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
   "error: file handle empty",
   "error: page does not exist",
   "error: incomplete page read",
   "error: incomplete page write",
   "last return code"
};

void assertRc() {
   assert(rc_msgs.size() == rc::last_rc + 1);
}

#define RC_MSG(RCODE, ...) do {\
       eprintf("%s: %s %d: rc#%d: %s ", __FILE__, __func__, \
               __LINE__, RCODE, rc_msgs.at(RCODE).c_str()); \
       eprintf (__VA_ARGS__); } while(0)

//
// PRIVATE HELPER FUNCTIONS
//

bool existsFile(const char* cfname);

inline int pageBeginPos(PageNum pageNum);
inline int pageEndPos(PageNum pageNum);

void rcprintf(int rc);

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


bool existsFile(const char* cfname) {
   struct stat buffer;
   return (stat (cfname, &buffer) == 0);
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

// WARNING: user must close/free all associated FileHandles
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
//PRE: fileHandle must be empty
RC PagedFileManager::openFile(const string &fileName, 
                              FileHandle &fileHandle)
{
   if (!fileHandle.isEmpty()) {
      RC_MSG (rc::file_handle_in_use, "\n");
      return rc::file_handle_in_use;
   } 
   const char* cfname = fileName.c_str();
   struct stat buffer;
   // Check if file exists
   if (stat (cfname, &buffer) != 0) {
      RC_MSG (rc::file_does_not_exist, 
              " [filename: \"%s\"]\n", cfname);
      return rc::file_does_not_exist;
   } 
   // Open existing file in binary read/write mode 
   FILE* fstrm = fopen (cfname, "rb+");
   if (fstrm) {
      // Set the file to the FileHandle
      fileHandle._fstream = fstrm;
      fileHandle._page_count = buffer.st_size / PAGE_SIZE; 
   } else {
      RC_MSG (rc::file_open_error, " [filename: \"%s\"]\n", cfname);
      return rc::file_open_error;
   }
   return rc::success;
}

//need to flush all pages to disk
RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
    if (fileHandle.isEmpty()) {
       RC_MSG (rc::file_handle_empty, "\n");
       return rc::file_handle_empty;
    } 
    if (fclose (fileHandle._fstream)) {
       RC_MSG (rc::file_close_error, "\n");
       return rc::file_close_error;
    }
    fileHandle._fstream = NULL;
    fileHandle._page_count = 0; 
    return rc::success;
}


FileHandle::FileHandle() : 
    _fstream (NULL), _page_count (0) 
{
    readPageCounter = 0;
    writePageCounter = 0;
    appendPageCounter = 0;
}


FileHandle::~FileHandle()
{
}


inline bool FileHandle::isEmpty() {
   return _fstream == NULL;
}

inline int pageBeginPos(PageNum pageNum) {
   return pageNum * PAGE_SIZE;
} 

inline int pageEndPos(PageNum pageNum) {
   return pageBeginPos(pageNum) + PAGE_SIZE - 1;
}

// Reads from the fstream into data memory 
// PRE: data must be of size PAGE_SIZE 
// WARNING: no data size check, no NULL check
RC FileHandle::readPage(PageNum pageNum, void *data)
{
   DEBUG_TEST(
      if (data == NULL) {
         DEBUG_LOG("error: void* data == NULL\n"); 
      }
      if (isEmpty()) {
        RC_MSG(rc::file_handle_empty, "\n");
        return rc::file_handle_empty;
      }
      if (pageNum >= _page_count ) {
         RC_MSG(rc::page_does_not_exist, "[pageNum: %d]\n", pageNum);
         return rc::page_does_not_exist;
      }
   );
   
   // unchecked fseek
   fseek (_fstream, pageBeginPos (pageNum), SEEK_SET);
   int fread_rc = fread (data, 1, PAGE_SIZE, _fstream);

   DEBUG_TEST(
      if (!ferror (_fstream)) {
         RC_MSG(rc::file_read_error, "\n");
         return rc::file_read_error;
      }
      if (fread_rc != PAGE_SIZE) {
         RC_MSG(rc::incomplete_page_read, "\n");
         return rc::incomplete_page_read;
      }
   );
 
   ++readPageCounter; 
   return rc::success;    
}


// Writes the data memory into the file
RC FileHandle::writePage(PageNum pageNum, const void *data)
{
   DEBUG_TEST(
      if (data == NULL) {
         DEBUG_LOG("error: void* data == NULL\n"); 
      }
      if (isEmpty()) {
         RC_MSG(rc::file_handle_empty, "\n");
         return rc::file_handle_empty;
      }
      if (pageNum >= _page_count ) {
         RC_MSG(rc::page_does_not_exist, "[pageNum: %d]\n", pageNum);
         return rc::page_does_not_exist;
      }
   );
   
   // unchecked fseek
   fseek (_fstream, pageBeginPos (pageNum), SEEK_SET);
   if (fwrite (data, 1, PAGE_SIZE, _fstream) == PAGE_SIZE) {
      fflush (_fstream);
   } else {
      RC_MSG(rc::file_write_error, "[pageNum: %d]\n", pageNum);
      return rc::file_write_error;
   }

   ++writePageCounter; 
   return rc::success;
}


RC FileHandle::appendPage(const void *data)
{
   fseek (_fstream, pageBeginPos(_page_count), SEEK_SET);
   ++_page_count;

   if (fwrite (data, 1, PAGE_SIZE, _fstream) == PAGE_SIZE) {
      fflush (_fstream);
   } else {
      RC_MSG(rc::incomplete_page_write, "\n");
      return rc::incomplete_page_write;
   }

   ++appendPageCounter; 
   return rc::success;
}


unsigned FileHandle::getNumberOfPages()
{
   return _page_count;
}


RC FileHandle::collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount)
{
    readPageCount = readPageCounter;
    writePageCount = writePageCounter;
    appendPageCount = appendPageCounter;
    return rc::success;
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
