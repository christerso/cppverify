#ifndef FILELOADER_H_INCL__
#define FILELOADER_H_INCL__

// Own
#include "common.h"

// Boost Includes
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

// Standard C++ includes
#include <fstream>

namespace cppverify {

class FileLoader
{
public:
	void run_scan(std::vector<std::string>& file_paths, files_t& file_list, bool use_cache = true);
	/**
	 * Loading a file fills the cache array containing the file modified timestamp and the filename
	 * 
	 * @param path
	 * 
	 * @param filename The name of the file
	 * @param useCache
	 * 
	 * @return True or false
	 */
    bool scan_dirs(const boost::filesystem::path& dir_path, files_t& file_list, bool use_cache = true);
private:
    void cache_file();
    void save_cache(const files_t& file_list);
private: 
    cache_t _file_cache;
};
}
#endif // FILELOADER_H_INCL__

