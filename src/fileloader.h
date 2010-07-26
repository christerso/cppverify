#ifndef FILELOADER_H_INCL__
#define FILELOADER_H_INCL__

// Own
#include "common.h"

// Boost Includes
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

namespace cppverify {

class FileLoader
{
public:
	void run_scan(std::vector<std::string>& file_paths, bool use_cache = true);
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
    bool scan_dirs(const boost::filesystem::path& dir_path, bool use_cache = true);
	files_t& get_file_list();
private:
    void get_homedir(std::string& homedir);
    void load_cache();
    void save_cache();
private: 
	files_t _file_list; // Complete filelist
    cache_t _file_cache;
};
}
#endif // FILELOADER_H_INCL__

