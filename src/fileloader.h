#ifndef FILELOADER_H_INCL__
#define FILELOADER_H_INCL__

// own
#include "common.h"

// boost includes
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

// std c++ includes
#include <set>

namespace cppverify {

class FileLoader {
public:
	FileLoader();
	~FileLoader();
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
	void remove_from_cache(std::string& filename);
	files_t& get_file_list();
	void save_cache();
private:
	void get_homedir(std::string& homedir);
	void load_cache();

	void prepare_associations();
	bool check_extensions(const std::string& file_name);
private:
	files_t _file_list; // Complete filelist
	cache_t _file_cache;
	std::set<std::string> _assoc;
};
}
#endif // FILELOADER_H_INCL__

