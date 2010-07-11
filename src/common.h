#ifndef CPPVERIFY_COMMON_H__
#define CPPVERIFY_COMMON_H__

// std
#include <vector>

// boost
#include <boost/shared_ptr.hpp>

namespace cppverify {

class Warning
{
};

typedef std::string file_t;
typedef boost::shared_ptr<file_t> p_file_t;

typedef std::vector<p_file_t> files_t;
typedef boost::shared_ptr<files_t> p_files_t;

typedef Warning warning_t;
typedef boost::shared_ptr<warning_t> p_warning_t;

typedef std::vector<p_warning_t> warnings_t;
typedef boost::shared_ptr<warnings_t> p_warnings_t;

typedef std::pair<p_file_t,p_warnings_t> result_t;
typedef boost::shared_ptr<result_t> p_result_t;

typedef std::vector<p_result_t> results_t;
typedef boost::shared_ptr<results_t> p_results_t;

}

#endif // CPPVERIFY_COMMON_H__
