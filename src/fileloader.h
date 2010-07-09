#ifndef FILELOADER_H_INCL__
#define FILELOADER_H_INCL__

// Standard C++ includes
#include <string>
#include <fstream>

// Boost includes
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp> 
#include <boost/filesystem/operations.hpp>

class CFileLoader
{
public:
    /**
     * Loading a file fills the cache array containing the file modified timestamp and the filename
     * 
     * @param filename The name of the file
     * 
     * @return True or false
     */
    bool loadFile(const std::string& path, const std::string& filename, bool useCache);
private:
    void cacheFile();
    void saveCache();
private:
    std::ifstream _fs;
};

#endif // FILELOADER_H_INCL__

