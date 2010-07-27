// own
#include "cheaders.h"
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

using namespace cppverify;

int main(int argc, char** argv)
{
	int retval = 0;
	bool use_cache = false;
	//Init google logging
	google::InitGoogleLogging(argv[0]);

	FileLoader fl;
	// Preparing boost command line options
	po::options_description desc("Allowed options");
	desc.add_options()
	("help,h", "show help on commands")
	("use-cache,c", "cache the files to scan")
	("c-style,s", po::value<std::string>(), "C style to scan for")
	("include-path,I", po::value<std::vector<std::string> >(), "paths to scan for files");
	po::positional_options_description p;
	p.add("include-path", -1);
	po::variables_map vm;
	try {
		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
		po::notify(vm);
	} catch (std::exception& x) {
		std::cerr << x.what() << std::endl;
		return retval;
	}

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return retval;
	}

	if (vm.count("use-cache")) {
		use_cache = true;
	}

	if (vm.count("include-path")) {
		std::cout << "Including paths:" << std::endl;
		std::vector<std::string> res;
		boost::filesystem::path cpath;
		std::vector<std::string> composed_vec;
		res = vm["include-path"].as<std::vector<std::string> >();
		for (size_t i = 0; i < res.size(); i++) {
			// Composing full path from shortened paths
			cpath = boost::filesystem::complete(res[i]);
			std::cout << cpath.string() << std::endl;
			composed_vec.push_back(cpath.string());
		}
		fl.run_scan(composed_vec, use_cache);
	}

	// Note reference used
	files_t& files = fl.get_file_list();

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

	google::ShutdownGoogleLogging();
	return retval;
}
