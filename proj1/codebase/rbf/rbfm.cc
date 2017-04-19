#include "rbfm.h"

RecordBasedFileManager* RecordBasedFileManager::_rbf_manager = 0;

RecordBasedFileManager* RecordBasedFileManager::instance()
{
    if(!_rbf_manager)
        _rbf_manager = new RecordBasedFileManager();

    return _rbf_manager;
}

RecordBasedFileManager::RecordBasedFileManager() {
    _pfm = PagedFileManager::instance();
}

RecordBasedFileManager::~RecordBasedFileManager()
{
}

RC RecordBasedFileManager::createFile(const string &fileName) 
{
    _pfm->createFile(fileName);
    return -1;
}

RC RecordBasedFileManager::destroyFile(const string &fileName) 
{
    _pfm->destroyFile(fileName);
    return -1;
}

RC RecordBasedFileManager::openFile(const string &fileName, 
                                    FileHandle &fileHandle) 
{
    _pfm->openFile(fileName, fileHandle);
    return -1;
}

RC RecordBasedFileManager::closeFile(FileHandle &fileHandle) 
{
    _pfm->closeFile(fileHandle);
    return -1;
}

RC RecordBasedFileManager::insertRecord(FileHandle &fileHandle, 
 const vector<Attribute> &recordDescriptor, const void *data, RID &rid) {
    return -1;
}

RC RecordBasedFileManager::readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data) {
    return -1;
}

RC RecordBasedFileManager::printRecord(const vector<Attribute> &recordDescriptor, const void *data) {
    return -1;
}
