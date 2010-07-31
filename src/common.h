#ifndef CPPVERIFY_COMMON_H__
#define CPPVERIFY_COMMON_H__

// google
#include <glog/logging.h>

// std
#include <ctime>
#include <string>
#include <vector>
#include <map>

// defines
#define CACHE_DIR ".cache"
#define CACHE_FILE "cppverify"

namespace cppverify {

typedef struct Warning
{
	uint32_t line;
	std::string msg;
} warning_t;

typedef std::string file_t;
typedef std::vector<file_t> files_t;

typedef std::map<std::string, time_t> cache_t;

typedef std::vector<warning_t> warnings_t;

typedef std::pair<file_t,warnings_t> result_t;

typedef std::vector<result_t> results_t;

}

#endif // CPPVERIFY_COMMON_H__
