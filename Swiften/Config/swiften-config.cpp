/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options.hpp>
#include <boost/version.hpp>
#include <boost/filesystem.hpp>
#include <string>

#include <Swiften/Base/Platform.h>
#include <Swiften/Base/Paths.h>
#include <Swiften/Version.h>

#include "swiften-config.h"

using namespace Swift;

void printFlags(const std::vector<std::string>& flags) {
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
		("version", "Show version information")
		("libs", "List the library flags")
		("cflags", "List the compiler & preprocessor flags")
	;
	boost::program_options::variables_map vm;
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);
	}
	catch (const boost::program_options::unknown_option& option) {
#if BOOST_VERSION >= 104200
		std::cout << "Ignoring unknown option " << option.get_option_name() << std::endl;
#else
		std::cout << "Error: " << option.what() << std::endl;
#endif
	}
	catch (const boost::program_options::error& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return -1;
	}

	if (vm.count("help") > 0) {
		std::cout << "swiften-config outputs the Swiften build options." << std::endl;
		std::cout << std::endl;
		std::cout << "Usage: swiften-config [OPTIONS]..." << std::endl;
		std::cout << std::endl;
		std::cout << desc << "\n";
		return 0;
	}
	if (vm.count("version") > 0) {
		std::cout << "swiften-config " << SWIFTEN_VERSION_STRING << std::endl;
		return 0;
	}

	// Read in all variables
	std::vector<std::string> libs;
	for (size_t i = 0; LIBFLAGS[i]; ++i) {
		libs.push_back(LIBFLAGS[i]);
	}
	std::vector<std::string> cflags;
	for (size_t i = 0; CPPFLAGS[i]; ++i) {
		cflags.push_back(CPPFLAGS[i]);
	}

	// Detect whether we're running in-place or not
	boost::filesystem::path executablePath = Paths::getExecutablePath();
	boost::filesystem::path topSourcePath = executablePath / ".." / "..";
	boost::filesystem::path topInstallPath = executablePath / "..";
	bool inPlace = !boost::filesystem::exists(topInstallPath / "include" / "Swiften" / "Swiften.h");

	// Replace "#" variables with the correct path
	for(size_t i = 0; i < libs.size(); ++i) {
		if (inPlace) {
			std::string lib = libs[i];
			boost::replace_all(lib, "#", topSourcePath.string());
			libs[i] = lib;
		}
		else {
			std::string lib = libs[i];
			boost::replace_all(lib, "#", (topInstallPath / "lib").string());
			boost::erase_all(lib, "/Swiften");
			libs[i] = lib;
		}
	}
	for(size_t i = 0; i < cflags.size(); ++i) {
		if (inPlace) {
			std::string cflag = cflags[i];
			boost::replace_all(cflag, "#", topSourcePath.string());
			cflags[i] = cflag;
		}
		else {
			std::string cflag = cflags[i];
			boost::replace_all(cflag, "#", (topInstallPath / "include").string());
			cflags[i] = cflag;
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
