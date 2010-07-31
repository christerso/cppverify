#include "check.h"

#include "cheaders.h"

// boost
#include <boost/regex.hpp>

// glog
#include <glog/logging.h>

// std
#include <iostream>
#include <fstream>




namespace cppverify {
const std::string _inc_match( "^#include [<\"]([\\w\\./]+)[>\"]" );

void check_header( uint32_t line_nr, const std::string header, warnings_t& warnings );
}

using namespace cppverify;

void cppverify::check( const file_t& file, warnings_t& warnings )
{
	boost::regex expr( _inc_match );
	boost::smatch what;
	std::string line;
	uint32_t line_nr = 0;

	std::ifstream file_stream( file.c_str(), std::ifstream::in );

	DLOG(INFO) << "Checking file: " << file;
	while( getline( file_stream, line ).good() ) {
		line_nr += 1;

		if( boost::regex_match( line, what, expr, boost::match_extra ) ) {
			if( what.size() == 2 ) {
				DLOG(INFO) << "Found header '" << what[1] << "' on line " << line_nr;
				check_header( line_nr, what[1], warnings );
			} else {
				LOG(ERROR) << "Got a match that not size 2, this should happen!";
				std::abort();
			}
		}
	}

	return;
}

void cppverify::check_header( uint32_t line_nr, const std::string header, warnings_t& warnings )
{
	// Don't like have three different for loops, find a better way
	for( int i = 0; i < 15; i++ ) {
		if( header.compare( c89_90_headers[i][0] ) == 0 ) {
			DLOG(INFO) << header << " matched " << c89_90_headers[i][0];
			warnings.push_back( warning_t( line_nr, header ) );
			return;
		}
	}
	for( int i = 0; i < 3; i++ ) {
		if( header.compare( c94_95_headers[i][0] ) == 0 ) {
			DLOG(INFO) << header << " matched " << c94_95_headers[i][0];
			warnings.push_back( warning_t( line_nr, header ) );
			return;
		}
	}
	for( int i = 0; i < 6; i++ ) {
		if( header.compare( c99_headers[i][0] ) == 0 ) {
			DLOG(INFO) << header << " matched " << c99_headers[i][0];
			warnings.push_back( warning_t( line_nr, header ) );
			return;
		}
	}
	return;
}
