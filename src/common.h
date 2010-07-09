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

typedef boost::shared_ptr<std::string> file_t;
typedef boost::shared_ptr<Warning> warning_t;
typedef boost::shared_ptr<std::vector<warning_t>> warnings_t;
typedef std::pair<file_t,warnings_t> result_t;

}

#endif // CPPVERIFY_COMMON_H__
