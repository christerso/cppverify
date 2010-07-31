#ifndef CPPVERIFY_COMMON_H__
#define CPPVERIFY_COMMON_H__

// boost
#include <boost/shared_ptr.hpp>

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

// defines
#define CACHE_PATH ".cache"
#define CACHE_FILE "cppverify"

namespace cppverify {

class Warning
{
};

typedef std::string file_t;
typedef std::vector<file_t> files_t;

typedef std::map<std::string, time_t> cache_t;

typedef Warning warning_t;
typedef boost::shared_ptr<warning_t> p_warning_t;

typedef std::vector<p_warning_t> warnings_t;
typedef boost::shared_ptr<warnings_t> p_warnings_t;

typedef std::pair<file_t,p_warnings_t> result_t;
typedef boost::shared_ptr<result_t> p_result_t;

typedef std::vector<p_result_t> results_t;
typedef boost::shared_ptr<results_t> p_results_t;

}

#endif // CPPVERIFY_COMMON_H__
