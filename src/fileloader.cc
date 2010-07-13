// Own includes
#include "fileloader.h"

// Boost includes
#include <boost/filesystem/operations.hpp>
#include <boost/exception/all.hpp>
// Standard C++
#include <string>
#include <ctime>
#include <vector>
#include <map>

// Google
#include <glog/logging.h>

using namespace cppverify;

void FileLoader::run_scan(std::vector<std::string>& file_paths, files_t& file_list, bool use_cache)
{
	for ( size_t i = 0; i < file_paths.size(); i++ ) {
		boost::filesystem::path scan_path(file_paths[i]);
		scan_dirs(scan_path, file_list, use_cache);
	}
}

bool FileLoader::scan_dirs(const boost::filesystem::path& dir_path, files_t& file_list, bool use_cache)
{
	if ( !boost::filesystem::exists(dir_path) ) {
		return false;
	}
	boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
	for ( boost::filesystem::directory_iterator itr(dir_path); itr != end_itr; ++itr ) {
		try {
			if ( is_directory(itr->status()) ) {
				// For each directory, call ourselves and repeat the scan
				scan_dirs( itr->path(), file_list, use_cache );
			} else {
				std::string full_name = itr->path().string();
				if ( use_cache ) {
					std::time_t time_modified = boost::filesystem::last_write_time(full_name);
					if (_file_cache.insert(std::map<std::string, time_t>::value_type(full_name, time_modified)).second) {
						// Insert succeeded
						LOG(INFO) << "Added: " << full_name;
					} else {
						// we already have this value in the cache, replace the value in this key if timestamps differ
						LOG(INFO) << "Found: " << full_name;
						LOG(INFO) << "Checking time modified";
						if ( _file_cache[full_name] != time_modified ) {
							_file_cache[full_name] = time_modified;
							file_list.push_back(full_name);
						}
					}
				} else {
					// No cache always append
					LOG(INFO) << "(NOCACHE) Added: " << full_name;
					file_list.push_back(full_name);
				}
			}
		} catch ( boost::exception& x ) {
			DLOG(ERROR) << "Permission Denied";
		}
	}
	return true;
}

