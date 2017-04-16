#ifndef _pfm_h_
#define _pfm_h_

typedef unsigned PageNum;
typedef int RC;
typedef char byte;

#define PAGE_SIZE 4096
#include <string>
#include <climits>
using namespace std;

class FileHandle;

class PagedFileManager
{
public:
    
    static PagedFileManager* instance();      // Access to the 
                                              // _pf_manager instance

   
    RC createFile  (const string &fileName);  // Create a new file
    RC destroyFile (const string &fileName);  // Destroy a file
    RC openFile    (const string &fileName, 
                    FileHandle &fileHandle);  // Open a file
    RC closeFile   (FileHandle &fileHandle);  // Close a file

protected:
    PagedFileManager();                       // Constructor
    ~PagedFileManager();                      // Destructor

private:
    static PagedFileManager *_pf_manager;
};


class FileHandle
{
public:
    // variables to keep the counter for each operation
    unsigned readPageCounter;
    unsigned writePageCounter;
    unsigned appendPageCounter;
    
    FileHandle();       // Default constructor
    ~FileHandle();      // Destructor

    // Get a specific page
    RC readPage(PageNum pageNum, void *data);

    // Write a specific page
    RC writePage(PageNum pageNum, const void *data);  

    // Append a specific page
    RC appendPage(const void *data);                  

    // Get the number of pages in the file
    unsigned getNumberOfPages();

    // Put the current counter values into variables
    RC collectCounterValues(unsigned &readPageCount, 
                            unsigned &writePageCount, 
                            unsigned &appendPageCount);  
}; 

#define DEBUG    // delete this line to hide CERR logs and test 
                 // can also compile with 
                 //    "gmake XCFLAGS=-UDEBUG" to undefine
                 // or "gmake XCFLagS=-DDEBUG" to define

//CERR_LOG usage:
//   CERR_LOG(char* format, ...);
//   example: CERR_LOG("%s drank %d beers\n", name, num);

//CERR_TEST usage:
//   example: CERR_TEST(int x = 1; cerr << x << endl; 
//                      x = myfunc(x); cerr << x << endl;);
//   note: be careful of introducing side-effects!!
//         try not to introduce permanent changes with this

void veprintf (const char* format, va_list args);
void eprintf (const char* format, ...);
void syseprintf (const char* object);

#ifndef DEBUG
    #define CERR_LOG(...)  do{}while(0)
    #define CERR_TEST(...) do{while(0)
#else
    #define CERR_LOG(...) do { \
        fprintf(stderr, "%s: %s %d: ", __FILE__, \
                __func__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); } while(0)
    #define CERR_TEST(STMTS) do { \
        CERR_LOG("---- BEGIN CERR_TEST ----\n"); \
        STMTS; \
        CERR_LOG("----- END CERR_TEST ----\n"); } while(0)
#endif


#endif
