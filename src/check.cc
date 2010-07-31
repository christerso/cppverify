#include "check.h"

#include "cheaders.h"

// boost
#include <boost/regex.hpp>

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

	std::cout << "Checking file: " << file << std::endl;
	while( getline( file_stream, line ).good() ) {
		line_nr += 1;

		if( boost::regex_match( line, what, expr, boost::match_extra ) ) {
			if( what.size() == 2 ) {
				check_header( line_nr, what[1], warnings );
				//std::cout << "Line " << line_nr << ", header: \"" << what[1] << "\"" << std::endl;
			} else {
				// shouldn't happen
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
			std::cout << header << ' ' << c89_90_headers[i][0] << std::endl;
			warnings.push_back( warning_t( line_nr, header ) );
			return;
		}
	}
	for( int i = 0; i < 3; i++ ) {
		if( header.compare( c94_95_headers[i][0] ) == 0 ) {
			std::cout << header << ' ' << c94_95_headers[i][0] << std::endl;
			warnings.push_back( warning_t( line_nr, header ) );
			return;
		}
	}
	for( int i = 0; i < 6; i++ ) {
		if( header.compare( c99_headers[i][0] ) == 0 ) {
			std::cout << header << ' ' << c99_headers[i][0] << std::endl;
			warnings.push_back( warning_t( line_nr, header ) );
			return;
		}
	}
	return;
}
