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
        file_delete_error,
        file_open_error,
        file_read_error, 
        file_write_error, 
    };
}

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
   struct stat buffer;
   const char* cfname = fileName.c_str();

   if (stat (cfname, &buffer) == 0) {
      eprintf ("error: file with name \"%s\" already exists\n", cfname);
      return rc::file_already_exists;
   } else {
      FILE* new_file = fopen (cfname, "ab");
      if (new_file) {
         fclose(new_file);
      } else {
         eprintf("error: could not create file \"%s\"\n", cfname);
         return rc::file_open_error;
      }
   } 
   return rc::success;
}


RC PagedFileManager::destroyFile(const string &fileName)
{
   const char* cfname = fileName.c_str();
   if (remove (cfname) != 0) {
      eprintf ("Error deleting file %s\n", cfname);
      return rc::file_delete_error;
   } 
   return rc::success;
}


RC PagedFileManager::openFile(const string &fileName, FileHandle &fileHandle)
{
    return -1;
}


RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
    return -1;
}


FileHandle::FileHandle()
{
    readPageCounter = 0;
    writePageCounter = 0;
    appendPageCounter = 0;
}


FileHandle::~FileHandle()
{
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

void veprintf (const char* format, va_list args) {
   assert (format != NULL);
   fflush (NULL);
   vfprintf (stderr, format, args);
   fflush (NULL);
}

void eprintf (const char* format, ...) {
   fprintf (stderr, "proj1: ");
   va_list args;
   va_start (args, format);
   veprintf (format, args);
   va_end (args);
}

void syseprintf (const char* object) {
   eprintf ("%s: %s\n", object, strerror (errno));
}

