// std
#include <iostream>

#include "common.h"
#include "check.h"

using namespace cppverify;

int main(int argc, char** argv)
{
	int retval = 0;

	// TODO Init options

	// TODO Get files to check
	std::vector<file_t> files; // Change to shared_ptr on vector?

	// Loop over all files and check them for warnings/errors
	std::vector<result_t> results;
	for ( std::vector<file_t>::iterator iter = files.begin(); iter != files.end(); iter++ ) {
		warnings_t warnings = check( *iter );

		if ( !warnings->empty() ) {
			results.push_back( result_t( *iter, warnings ) );
		}
	}

	// Present the result
	if ( !results.empty() ) {
		// TODO Present/Generate result

		// TODO if retval should be none zero when any warning are found, add it here
	} else {
		// TODO print Everything is fine!
		// XXX Should an empty xml file be generated in this case?
	}

	return retval;
}
