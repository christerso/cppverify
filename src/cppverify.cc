// own
#include "common.h"
#include "fileloader.h"

#include "check.h"
extern "C" {
#include "cheaders.h"
}
// boost
#include <exception>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

// c++ tr1
#include <tr1/unordered_map>

// std
#include <iostream>
#include <cctype>
// List of TODOs:
//
//	Make paths full path (optional)
//   Add Depth of scan


namespace po = boost::program_options;

namespace cppverify {
class CppVerify {
public:
	CppVerify( void );
	int setup_program_options( int argc, char** argv );
	int check_program_options( void );
	void find_files( void );
	void check_files( void );
	void check_style( void );
	int show_result( void );
private:
	results_t results;
	FileLoader fl;
	po::options_description opt_desc;
	po::variables_map vm;
	utable_t _inc_table;
};
}

using namespace cppverify;

int main(int argc, char** argv)
{
	//Init google logging
	google::InitGoogleLogging(argv[0]);

	int retval = 0;
	CppVerify cv;

	retval = cv.setup_program_options( argc, argv );

	// check options
	if ( retval != 0 ) {
		goto main_exit;
	}
	// Setup include table, defaulting to C99
	cv.check_style();

	// Find all files to check
	cv.find_files();

	// check files
	cv.check_files();

	// Present the result
	retval = cv.show_result();

main_exit:
	google::ShutdownGoogleLogging();
	return retval;
}

CppVerify::CppVerify( void ) :
		results(),
		fl(),
		opt_desc("Allowed options"),
		vm()
{
}

/**
 * Setup the option parser
 *
 * @param argc Number of arguments to cppverify
 * @param argv Array of char pointers with all arguments to cppverify.
 * @param opt_desc The option description.
 * @param vm The parsed options.
 *
 * @return 0 if successfull.
 */
int CppVerify::setup_program_options( int argc, char** argv )
{
	int retval = 0;

	// Preparing boost command line options
	opt_desc.add_options()
	("help,h", "show help on commands")
	("use-cache,c", "cache the files to scan")
	("c-style,s", po::value<std::string>(), "C style to scan for")
	("include-path,I", po::value<std::vector<std::string> >(), "paths to scan for files");
	po::positional_options_description p;
	p.add("include-path", -1);

	try {
		po::store(po::command_line_parser(argc, argv).options(opt_desc).positional(p).run(), vm);
		po::notify(vm);
	} catch (std::exception& x) {
		std::cerr << x.what() << std::endl;
		retval = -1;
	}

	if ( retval == 0 ) {
		retval = check_program_options();
	}

	return retval;
}

int CppVerify::check_program_options( void )
{
	int retval = 0;

	if ( vm.count("help") ) {
		std::cout << opt_desc << std::endl;
		retval = -1;
	}

	return retval;
}

/**
 * Finds all the files to check.
 */
void CppVerify::find_files( void )
{
	if ( vm.count( "include-path" ) ) {
		std::vector<std::string> res;
		std::vector<std::string> composed_vec;
		bool use_cache = false;

		if (vm.count("use-cache")) {
			use_cache = true;
		}

		res = vm["include-path"].as<std::vector<std::string> >();
		for (size_t i = 0; i < res.size(); i++) {
			// Composing full path from shortened paths (might need some improvement)
			char pathsz[256];
			std::string pathstr;
			strncpy(pathsz, res[i].c_str(), res[i].length());
			if ('.' == res[i].c_str()[0]) {
				getcwd(pathsz, 256);
				pathstr = pathsz;
			} else {
				pathstr = res[i].c_str();
			}
			composed_vec.push_back(pathstr);
		}
		fl.run_scan(composed_vec, use_cache);
	} else {
		DLOG(INFO) << "No directories to check.";
	}

	return;
}

/**
 * Loops over all the files and checks each file.
 */
void CppVerify::check_files( void )
{
	// Loop over all files and check them for warnings/errors
	BOOST_FOREACH( file_t file, fl.get_file_list() ) {
		warnings_t warnings;

		// TODO send vm to check, is this needed?
		if (!check( file, warnings, _inc_table )) {
			fl.remove_from_cache(file);
		}

		if ( !warnings.empty() ) {
			results.push_back( result_t( file, warnings ) );
		}
	}
	if (vm.count("use-cache")) {
		fl.save_cache();
	}
	return;
}

/**
 * Checks which C dialect to scan for
 */
void CppVerify::check_style( void )
{
	cstyles_t style_t;
	if (vm.count("c-style")) {
		std::string style;
		style = vm["c-style"].as<std::string> ();
		if (!style.compare("99")) {
			LOG(INFO) << "Scanning conforms to C99";
			style_t = C99;
		} else if (!style.compare("95") || !style.compare("C95")) {
			LOG(INFO) << "Scanning conforms to C94-C95";
			style_t = C95;
		} else if (!style.compare("89") || !style.compare("90")) {
			LOG(INFO) << "Scanning conforms to C89-C90";
			style_t = C89;
		}
	} else {
		style_t = C99; // Default style if none given
	}

	switch (style_t) {
	case C99:
		for (unsigned int i = 0; i < sizeof(c99_headers)
		        / sizeof(c99_headers[0]); i++) {
			_inc_table.insert(std::make_pair(c99_headers[i][0],
			                                 c99_headers[i][1]));
		}
	case C95:
		for (unsigned int i = 0; i < sizeof(c94_95_headers)
		        / sizeof(c94_95_headers[0]); i++) {
			_inc_table.insert(std::make_pair(c94_95_headers[i][0],
			                                 c94_95_headers[i][1]));
		}
	case C89:
		for (unsigned int i = 0; i < sizeof(c89_90_headers)
		        / sizeof(c89_90_headers[0]); i++) {
			_inc_table.insert(std::make_pair(c89_90_headers[i][0],
			                                 c89_90_headers[i][1]));
		}
	}
}
/**
 * Shows the result of the check(s) to the user.
 */
int CppVerify::show_result( void )
{
	int retval = 0;

	if ( !results.empty() ) {
		BOOST_FOREACH( result_t result, results ) {
			BOOST_FOREACH( warning_t warning, result.second ) {
				std::cout << result.first << ":" << warning.line << " - " << warning.msg << std::endl;
			}
		}
		// TODO Present/Generate result

		// TODO if retval should be none zero when any warning are found, add it here
		if ( false ) {
			retval = 1;
		}
	} else {
		// TODO Should be possible to disable this.
		//std::cout << "No errors" << std::endl;
		// XXX Should an empty xml file be generated in this case?
	}

	return retval;
}
