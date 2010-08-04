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

// C headers
#include <sys/time.h>

// List of TODOs:
//
//  Add Depth of scan
//	Add filesize limit to cachefile


namespace po = boost::program_options;

namespace cppverify {
class CppVerify {
public:
	CppVerify( void );
	int setup_program_options( int argc, char** argv );

	bool timed_run( void );
	int check_program_options( void );
	void find_files( void );
	void check_files( void );
	void check_style( void );
	void check_clear_cache();
	int show_result( void );
	int get_filelist_length();
	int get_total_files();
	void check_output_xml();
private:
	results_t _results;
	FileLoader _fl;
	po::options_description _opt_desc;
	po::variables_map _vm;
	utable_t _inc_table;
};
}

using namespace cppverify;

int main(int argc, char** argv)
{
	// Timing setup
	timeval t1, t2;
	double elapsed_time;

	//Init google logging
	google::InitGoogleLogging(argv[0]);

	int retval = 0;
	CppVerify cv;

	retval = cv.setup_program_options( argc, argv );

	if (cv.timed_run()) {
		gettimeofday(&t1, NULL);
	}
	// check options
	if ( retval != 0 ) {
		goto main_exit;
	}
	// Setup include table, defaulting to C99
	cv.check_style();

	// Check if we should remove the cache
	cv.check_clear_cache();

	// Find all files to check
	cv.find_files();

	// check files
	cv.check_files();

	// Present the result
	retval = cv.show_result();

main_exit:
	google::ShutdownGoogleLogging();
	if (cv.timed_run()) {
		gettimeofday(&t2, NULL);
		elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
		elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
		std::cout << "Scanned " << cv.get_filelist_length() << " files, out of " << cv.get_total_files() << " in " << elapsed_time << " ms." << std::endl;
	}
	return retval;
}

CppVerify::CppVerify( void ) :
		_results(),
		_fl(),
		_opt_desc("Allowed options"),
		_vm()
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
	_opt_desc.add_options()
	("help,h", "show help on commands")
	("cache,c", "cache the files to scan")
	("remove-cache,r", "remove cache file")
	("style,s", po::value<std::string>(), "C style to scan for, default C99")
	("stats,S", "show stats for the run")
	("xml-output,x", "output XML file with scan results")
	("include-path,I", po::value<std::vector<std::string> >(), "paths to scan for files");
	po::positional_options_description p;
	p.add("include-path", -1);

	try {
		po::store(po::command_line_parser(argc, argv).options(_opt_desc).positional(p).run(), _vm);
		po::notify(_vm);
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

	if ( _vm.count("help") ) {
		std::cout << _opt_desc << std::endl;
		retval = -1;
	}

	return retval;
}

/**
 * Finds all the files to check.
 */
void CppVerify::find_files( void )
{
	if ( _vm.count( "include-path" ) ) {
		std::vector<std::string> res;
		std::vector<std::string> composed_vec;
		bool use_cache = false;

		if (_vm.count("cache")) {
			use_cache = true;
		}

		res = _vm["include-path"].as<std::vector<std::string> >();
		for (size_t i = 0; i < res.size(); i++) {
			// Composing full path from shortened paths (might need some improvement)
			char pathsz[256];
			std::string pathstr;
			strncpy(pathsz, res[i].c_str(), res[i].length());
			if ('.' == res[i].c_str()[0]) {
				char* ret = getcwd(pathsz, 256);
				if (ret == NULL) {
					LOG(ERROR) << "Unable to get path";
				}
				pathstr = pathsz;
			} else {
				pathstr = res[i].c_str();
			}
			composed_vec.push_back(pathstr);

		}
		_fl.run_scan(composed_vec, use_cache);
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
	BOOST_FOREACH( file_t file, _fl.get_file_list() ) {
		warnings_t warnings;

		// TODO send vm to check, is this needed?
		if (!check( file, warnings, _inc_table )) {
			_fl.remove_from_cache(file);
		}

		if ( !warnings.empty() ) {
			_results.push_back( result_t( file, warnings ) );
		}
	}
	if (_vm.count("cache")) {
		_fl.save_cache();
	}
	return;
}

bool CppVerify::timed_run( void )
{
	if (_vm.count("stats")) {
		return true;
	}
	return false;
}

/**
 * Checks which C dialect to scan for
 */
void CppVerify::check_style( void )
{
	cstyles_t style_t = C99;
	if (_vm.count("style")) {
		std::string style;
		style = _vm["style"].as<std::string> ();
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


void CppVerify::check_clear_cache( void )
{
	if (_vm.count("remove-cache")) {
		_fl.clear_cache();
	}
}

/**
 * Shows the result of the check(s) to the user.
 */
int CppVerify::show_result( void )
{
	int retval = 0;

	if ( !_results.empty() ) {
		BOOST_FOREACH( result_t result, _results ) {
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

int CppVerify::get_filelist_length()
{
	return _fl.get_file_list().size();
}

int CppVerify::get_total_files()
{
	return _fl.get_files_scanned();
}

void CppVerify::check_output_xml()
{
	if (_vm.count("output-xml")) {
		files_t files = _fl.get_file_list();

	}
}
