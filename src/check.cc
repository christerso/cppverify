#include "check.h"

// boost
#include <boost/regex.hpp>

// glog
#include <glog/logging.h>

// std
#include <sstream>
#include <fstream>

namespace cppverify {

const std::string _inc_match("^\\s*#include\\s*(<([^\"'<>|\\b]+)>|\"([^\"'<>|\\b]+)\")");
// const std::string _inc_match( "^#include [<\"]([\\w\\./]+)[>\"]" );

void check_header( uint32_t line_nr, const std::string header, warnings_t& warnings, utable_t& inc_table );
std::string create_wrong_header_msg( const std::string& header, const char* cpp_header );
}

using namespace cppverify;

void cppverify::check( const file_t& file, warnings_t& warnings, utable_t& inc_table )
{
	boost::regex expr( _inc_match );
	boost::smatch what;
	std::string line;
	uint32_t line_nr = 0;

	std::ifstream file_stream( file.c_str(), std::ifstream::in );

	DLOG(INFO) << "Checking file: " << file;
	while ( getline( file_stream, line ).good() ) {
		line_nr += 1;

		if ( boost::regex_match( line, what, expr, boost::match_extra ) ) {
			if ( what.size() == 4 ) {
				if (!what[2].compare("")) {
					DLOG(ERROR) << "Found system header '" << what[3] << "' on line " << line_nr;
					check_header( line_nr, what[3], warnings, inc_table );
				} else {
					// what[2] matches when the user have used " for the system header
					// this should also be a warning, as the user should use <> for system headers
					DLOG(ERROR) << "Found system header '" << what[2] << "' on line " << line_nr;
					check_header( line_nr, what[2], warnings, inc_table );
				}
			} else {
				LOG(ERROR) << "Got a match that not size 4, this should happen!";
				std::abort();
			}
		}
	}
	return;
}

void cppverify::check_header( uint32_t line_nr, const std::string header, warnings_t& warnings, utable_t& inc_table )
{
	utable_t::const_iterator cit;
	cit = inc_table.find(header);
	if (cit != inc_table.end()) {
		LOG(INFO) << header << " matched " << cit->first;
		warnings.push_back( warning_t( line_nr, create_wrong_header_msg( header, cit->second.c_str() ) ) );
	}
	return;
}

std::string cppverify::create_wrong_header_msg( const std::string& header, const char* cpp_header )
{
	std::ostringstream oss;
	oss << "Change include of '" << header << "' to '" << cpp_header << "'";
	return oss.str();
}
