// own
#include "common.h"
#include "fileloader.h"
#include "check.h"

// boost
#include <exception>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

// std
#include <iostream>
#include <cctype>
// List of TODOs:
//
//	Make paths full path (optional)
//   Add Depth of scan

namespace po = boost::program_options;

namespace cppverify {
int setup_program_options( int argc, char** argv, po::options_description& opt_desc, po::variables_map& vm );
void find_files( po::variables_map& vm, FileLoader& fl );
void check_files( po::variables_map& vm, FileLoader& fl, results_t& results );
int show_result( po::variables_map& vm, const results_t& results );
}

using namespace cppverify;

int main(int argc, char** argv)
{
	//Init google logging
	google::InitGoogleLogging(argv[0]);

	int retval = 0;
	FileLoader fl;
	results_t results;

	// TODO refactor this out
	po::variables_map vm;
	po::options_description opt_desc("Allowed options");

	retval = setup_program_options( argc, argv, opt_desc, vm );

	// check options
	if( retval != 0 ) {
		goto main_exit;
	} else if( vm.count("help") ) {
		std::cout << opt_desc << std::endl;
		goto main_exit;
	}

	// Find all files to check
	find_files( vm, fl );

	// check files
	check_files( vm, fl, results );

	// Present the result
	retval = show_result( vm, results );

main_exit:
	google::ShutdownGoogleLogging();
	return retval;
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
int cppverify::setup_program_options( int argc, char** argv, po::options_description& opt_desc, po::variables_map& vm )
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

	return retval;
}

/**
 * Finds all the files to check.
 *
 * @param vm Parsed options to cppverify.
 * @param fl File loader.
 */
void cppverify::find_files( po::variables_map& vm, FileLoader& fl )
{
	if( vm.count( "include-path" ) ) {
		std::vector<std::string> res;
		boost::filesystem::path cpath;
		std::vector<std::string> composed_vec;
		bool use_cache = false;

		if (vm.count("use-cache")) {
			use_cache = true;
		}

		res = vm["include-path"].as<std::vector<std::string> >();
		for (size_t i = 0; i < res.size(); i++) {
			// Composing full path from shortened paths (might need some improvement)
			cpath = boost::filesystem::complete(res[i]);
			composed_vec.push_back(cpath.string());
		}
		fl.run_scan(composed_vec, use_cache);
	} else {
		DLOG(INFO) << "No directories to check.";
	}

	return;
}

/**
 * Loops over all the files and checks each file.
 *
 * @param vm Options to cppverify.
 * @param fl The file loader, contains all files to check.
 * @param results The results from the checks (out)
 */
void cppverify::check_files( po::variables_map& vm, FileLoader& fl, results_t& results )
{
	// Loop over all files and check them for warnings/errors
	BOOST_FOREACH( file_t file, fl.get_file_list() ) {
		warnings_t warnings;

		// TODO send vm to check, is this needed?
		check( file, warnings );

		if ( !warnings.empty() ) {
			results.push_back( result_t( file, warnings ) );
		}
	}

	return;
}

/**
 * Shows the result of the check(s) to the user.
 *
 * @param vm Options to cppverify.
 * @param results The results from the check(s).
 *
 * @return 0 if everything was successful, 1 if any warnings are in the
 * results and the user has activated this in the options.
 */
int cppverify::show_result( po::variables_map& vm, const results_t& results )
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
		if( false ) {
			retval = 1;
		}
	} else {
		// TODO Should be possible to disable this.
		std::cout << "No errors" << std::endl;
		// XXX Should an empty xml file be generated in this case?
	}

	return retval;
}
