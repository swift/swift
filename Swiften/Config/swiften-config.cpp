/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options.hpp>
#include <boost/version.hpp>

#include <Swiften/Base/String.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Platform.h>
#include <Swiften/Base/Paths.h>

#include "swiften-config.h"

using namespace Swift;

void printFlags(const std::vector<String>& flags) {
	for (size_t i = 0; i < flags.size(); ++i) {
		if (i > 0) {
			std::cout << " ";
		}
		std::cout << flags[i];
	}
	std::cout << std::endl;
}

int main(int argc, char* argv[]) {
	boost::program_options::options_description desc;
	desc.add_options()
		("help", "Show this help message")
		("libs", "List the library flags")
		("cflags", "List the compiler & preprocessor flags")
	;
	boost::program_options::variables_map vm;
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	} catch (boost::program_options::unknown_option option) {
#if BOOST_VERSION >= 104200
		std::cout << "Ignoring unknown option " << option.get_option_name() << " but continuing." <<  std::endl;
#else
		std::cout << "Error: " << option.what() << " (continuing)" <<  std::endl;
#endif
	}
	boost::program_options::notify(vm);

	if (vm.count("help") > 0) {
		std::cout << desc << "\n";
		return 0;
	}

	// Read in all variables
	std::vector<String> libs;
	for (size_t i = 0; LIBFLAGS[i]; ++i) {
		libs.push_back(LIBFLAGS[i]);
	}
	std::vector<String> cflags;
	for (size_t i = 0; CPPFLAGS[i]; ++i) {
		cflags.push_back(CPPFLAGS[i]);
	}

	// Detect whether we're running in-place or not
	boost::filesystem::path executablePath = Paths::getExecutablePath();
	boost::filesystem::path topPath = executablePath / ".." / "..";
	bool inPlace = true;	

	// Replace "#" variables with the correct path
	for(size_t i = 0; i < libs.size(); ++i) {
		if (inPlace) {
			String lib = libs[i];
			lib.replaceAll('#', topPath.string());
			libs[i] = lib;
		}
		else {
			// TODO
		}
	}
	for(size_t i = 0; i < cflags.size(); ++i) {
		if (inPlace) {
			String cflag = cflags[i];
			cflag.replaceAll('#', topPath.string());
			cflags[i] = cflag;
		}
		else {
			// TODO
		}
	}


	// Print the requested variable
	if (vm.count("libs") > 0) {
		printFlags(libs);
	}
	else if (vm.count("cflags") > 0) {
		printFlags(cflags);
	}
	return 0;
}
