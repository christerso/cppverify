#ifndef CPPVERIFY_CHECK_H__
#define CPPVERIFY_CHECK_H__

#include "common.h"
// c++ tr1
#include <tr1/unordered_map>

namespace cppverify {

void check( const file_t& file, warnings_t& warnings, utable_t& inc_table ); // TODO Add options object

}

#endif // CPPVERIFY_CHECK_H__
