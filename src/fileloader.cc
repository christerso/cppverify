// Own includes
#include "fileloader.h"

// Boost includes
#include <boost/filesystem/operations.hpp>
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>

// Standard C++
#include <iostream>
#include <fstream>

using namespace cppverify;

void FileLoader::run_scan(std::vector<std::string>& file_paths, files_t& file_list, bool use_cache)
{
	for ( size_t i = 0; i < file_paths.size(); i++ ) {
		boost::filesystem::path scan_path(file_paths[i]);
		scan_dirs(scan_path, file_list, use_cache);
	}
	// Now save the cache
	DLOG(INFO) << "Number of files found:" << file_list.size();
	save_cache(file_list);
}

void FileLoader::save_cache(const files_t& file_list)
{
	// boost::filesystem::path cache(CACHE_PATH);
	std::string fullpath(CACHE_PATH);
	fullpath.append("/");
	fullpath.append(CACHE_FILE);
	// try {
	// 	if ( !boost::filesystem::exists(cache) ) {
	// 		// Create the file path if it does not exist
	// 		// boost::filesystem::create_directory(cache);
	// 	}
	// } catch (boost::exception& x) {
	// 	LOG(WARNING) << "Unable to save cachefile";
	// 	return;
	// }
	std::ofstream outfile(fullpath.c_str());
	files_t::const_iterator it_pos = file_list.begin(), it_end = file_list.end();
	LOG(INFO) << "Writing cache...";
	while (it_pos != it_end) {
		outfile << it_pos->c_str() << std::endl;
		++it_pos;
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

