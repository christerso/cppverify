// Own includes
#include "fileloader.h"

// Boost includes
#include <boost/filesystem/operations.hpp>
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
// Standard C++
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <map>

// C headers
#include <sys/types.h>
#include <pwd.h>

// Google
#include <glog/logging.h>

using namespace cppverify;

FileLoader::FileLoader():
		_files_scanned(0)
{
	prepare_associations();
}

FileLoader::~FileLoader()
{

}

void FileLoader::prepare_associations()
{
	char buf[4];
	const char* ext[] = { "cc", "cpp", "hpp", "h", "hh" };
	LOG(INFO) << "Adding extensions:";
	for (unsigned int i = 0; i < sizeof(ext) / sizeof(ext[0]); i++ ) {
		std::string val;
		strncpy(buf, ext[i], sizeof(buf));
		val = buf;
		LOG(INFO) << val;
		_assoc.insert(val);
	}
}

void FileLoader::run_scan(std::vector<std::string>& file_paths, bool use_cache)
{
	if (use_cache) {
		load_cache();
	}
	for ( size_t i = 0; i < file_paths.size(); i++ ) {
		boost::filesystem::path scan_path(file_paths[i]);
		scan_dirs(scan_path, use_cache);
	}
	// Now save the cache
	DLOG(INFO) << "Number of files cached:" << boost::lexical_cast<std::string>(_file_cache.size());
	save_cache();
}

// Try to get homedir from the passwdfile.
// If that fails look for the HOME environment variable
// If that also fails take the current directory.
void FileLoader::get_homedir(std::string& homedir)
{
	struct passwd *pwd;
	pwd = getpwuid(getuid());
	homedir = pwd->pw_dir;
	if (!homedir.compare("") || pwd == NULL) {
		homedir = getenv("HOME");
		if (!homedir.compare("")) {
			homedir = ".";
		}
	}
}
void FileLoader::remove_from_cache(std::string& filename)
{
	DLOG(INFO) << "Removing " << filename << " from cache, as it contains C style header";
	_file_cache.erase(filename);
}

files_t& FileLoader::get_file_list()
{
	return _file_list;
}

void FileLoader::load_cache()
{
	std::string homedir;
	get_homedir(homedir);
	std::string cachepath(homedir);
	cachepath.append("/");
	cachepath.append(CACHE_DIR);
	cachepath.append("/");
	cachepath.append(CACHE_FILE);

	std::stringstream stream_conv;
	std::time_t tt;
	std::string path;
	std::string time;
	std::string line;
	std::string::size_type st;
	std::ifstream infile(cachepath.c_str());
	if (infile.is_open()) {
		while (!infile.eof()) {
			std::getline(infile, line);
			st = line.rfind(":");
			if (st != std::string::npos) {
				path = line.substr(0, st);
				time = line.substr(st + 1);
				DLOG(INFO) << "Loaded cache value: " << path << " : " << time;
				// Convert time back into t_time format
				stream_conv << time;
				stream_conv >> tt;
				stream_conv.clear();
				stream_conv.str("");
				if (!_file_cache.insert(std::make_pair(path, tt)).second) {
					// And this should never happen as these values are loaded first, but who knows
					DLOG(WARNING) << "Value already existed in cache, skipping add.";
				}
			}
		}
	}
}

void FileLoader::save_cache()
{
	if (_file_cache.empty()) {
		return;
	}
	std::string userhome;
	get_homedir(userhome);
	std::string cachepathstr = userhome;
	std::string cachefilestr = userhome;
	cachefilestr.append("/");
	cachefilestr.append(CACHE_DIR);
	cachepathstr = cachefilestr;
	cachefilestr.append("/");
	cachefilestr.append(CACHE_FILE);
	boost::filesystem::path cachepath(cachepathstr);
	if (!boost::filesystem::exists(cachepath)) {
		try {
			boost::filesystem::create_directory(cachepath);
		} catch (boost::exception& x) {
			LOG(ERROR) << "Unable to create:" << cachepathstr;
		}
	} else {
		std::ofstream outfile(cachefilestr.c_str());
		cache_t::const_iterator it_pos = _file_cache.begin(), it_end = _file_cache.end();
		LOG(INFO) << "Writing cache...";
		while (it_pos != it_end) {
			outfile << it_pos->first << ":" << it_pos->second << std::endl;
			++it_pos;
		}
		outfile.close();
	}
}

bool FileLoader::scan_dirs(const boost::filesystem::path& dir_path, bool use_cache)
{
	if ( !boost::filesystem::exists(dir_path) || !boost::filesystem::is_directory(dir_path) ) {
		std::cerr << "Invalid path given:\n" << dir_path.string() << std::endl;
		return false;
	}
	std::string full_name;
	boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
	for ( boost::filesystem::directory_iterator itr(dir_path); itr != end_itr; ++itr ) {
		try {
			if ( is_directory(itr->status()) ) {
				// For each directory, call ourselves and repeat the scan
				scan_dirs( itr->path(), use_cache );
			} else {
				full_name = itr->path().string();
				++_files_scanned;
				if (!check_extensions(full_name)) {
					continue;
				}
				if ( use_cache ) {
					std::time_t time_modified = boost::filesystem::last_write_time(full_name);
					if (_file_cache.insert(std::make_pair(full_name, time_modified)).second) {
						// Insert succeeded
						LOG(INFO) << "Added: " << full_name;
						_file_list.push_back(full_name);
					} else {
						// we already have this value in the cache, replace the value in this key if timestamps differ
						if ( _file_cache[full_name] != time_modified ) {
							LOG(INFO) << "Updating: " << full_name;
							_file_cache[full_name] = time_modified;
							_file_list.push_back(full_name);
						} else {
							LOG(INFO) << "Not updating: " << full_name;
						}
					}
				} else {
					// No cache always append
					LOG(INFO) << "(NOCACHE) Added: " << full_name;
					_file_list.push_back(full_name);
				}
			}
		} catch ( boost::exception& x ) {
			DLOG(ERROR) << "Permission Denied";
		}
	}
	return true;
}

bool FileLoader::check_extensions(const std::string& file_name)
{
	// Reverse scan for first dot
	std::string::size_type st;
	st = file_name.rfind(".");
	std::string compval;
	if (st != std::string::npos) {
		compval = file_name.substr(st + 1);
		if (_assoc.find(compval) != _assoc.end()) {
			return true;
		}
	}
	return false;
}

int FileLoader::get_files_scanned()
{
	return _files_scanned;
}

