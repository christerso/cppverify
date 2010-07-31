#include "check.h"

// boost
#include <boost/regex.hpp>

// std
#include <iostream>
#include <fstream>

using namespace cppverify;

const std::string _inc_match( "^#include [<\"](\\w+)[>\"]" );

p_warnings_t cppverify::check( const file_t& file )
{
	p_warnings_t warnings( new warnings_t );
	boost::regex expr( _inc_match );
	boost::smatch what;
	std::string line;
	uint32_t line_nr = 0;

	std::ifstream file_stream( const_cast<file_t&>(file), std::ifstream::in );

	std::cout << "Checking file: " << file << std::endl;
	while( getline( file_stream, line ).good() ) {
		line_nr += 1;

		if( boost::regex_match( line, what, expr, boost::match_extra ) ) {
			if( what.size() == 2 ) {
				std::cout << "Line " << line_nr << ", header: \"" << what[1] << "\"" << std::endl;
			} else {
				// should happen
			}
		}
	}

	return warnings;
}
