#ifndef _pfm_h_
#define _pfm_h_

typedef unsigned PageNum;
typedef int RC;
typedef char byte;

#define PAGE_SIZE 4096
#include <string>
#include <climits>

#define DEBUG
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
    friend class PagedFileManager;

    // variables to keep the counter for each operation
    unsigned readPageCounter;
    unsigned writePageCounter;
    unsigned appendPageCounter;
    
    FileHandle();       // Default constructor
    ~FileHandle();      // Destructor

    inline bool isInUse();       // Check if this FileHandle is in use 
                                 //   (opposite of isFree()) 
  
    inline bool isFree();        // Check if this FileHandle is free   
                                 //   (opposite of isInUse()) 

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

private:
    FILE* _file;
}; 


// PUBLIC HELPER FUNCTIONS
// error messaging
void veprintf (const char* format, va_list args);
void eprintf (const char* format, ...);
void syseprintf (const char* object);

#define ERR_MSG(...) do { \
    eprintf("%s: %s %d: ", __FILE__, __func__, __LINE__); \
    eprintf(__VA_ARGS__); } while(0)


#define DEBUG    // comment this line to hide CERR logs and test 
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


#ifndef DEBUG
    #define DEBUG_LOG(...)  do{}while(0)
    #define DEBUG_TEST(...) do{}while(0)
#else
    #define DEBUG_LOG(...) do { \
        eprintf("(DB) %s: %s %d: ", __FILE__, \
                __func__, __LINE__); \
        eprintf(__VA_ARGS__); } while(0)
    #define DEBUG_TEST(STMTS) do { \
        DEBUG_LOG("---- BEGIN CERR_TEST ----\n"); \
        STMTS; \
        DEBUG_LOG("----- END CERR_TEST ----\n"); } while(0)
#endif


#endif
