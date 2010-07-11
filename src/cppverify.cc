// std
#include <iostream>

// boost
#include <boost/foreach.hpp>

#include "common.h"
#include "check.h"

// own
#include "fileloader.h"

using namespace cppverify;

int main(int argc, char** argv)
{
	int retval = 0;

	// Init google logging
	google::InitGoogleLogging(argv[0]);

	files_t filelist;
	FileLoader fl;
	boost::filesystem::path my_path( "./" );
	fl.scan_dirs(my_path, filelist);
	// TODO Get files to check
	files_t files; // Change to shared_ptr on vector?

	// Loop over all files and check them for warnings/errors
	p_results_t results( new results_t );
	BOOST_FOREACH( file_t file, files ) {
		p_warnings_t warnings = check( file );

		if ( !warnings->empty() ) {
			results->push_back( p_result_t( new result_t( file, warnings ) ) );
		}
	}

	// Present the result
	if ( !results->empty() ) {
		// TODO Present/Generate result

		// TODO if retval should be none zero when any warning are found, add it here
	} else {
		// TODO print Everything is fine!
		// XXX Should an empty xml file be generated in this case?
	}

	return retval;
}
