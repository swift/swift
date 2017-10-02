/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Config/swiften-config.h>

#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/version.hpp>

#include <Swiften/Base/Path.h>
#include <Swiften/Base/Paths.h>
#include <Swiften/Base/Platform.h>
#include <Swiften/Version.h>

using namespace Swift;

static void printFlags(const std::vector<std::string>& flags) {
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
    for(auto & i : libs) {
        if (inPlace) {
            std::string lib = i;
            boost::replace_all(lib, "#", pathToString(topSourcePath));
            i = lib;
        }
        else {
            std::string lib = i;
            boost::replace_all(lib, "#", pathToString(topInstallPath / "lib"));
            boost::erase_all(lib, "/Swiften");
            i = lib;
        }
    }
    for(auto & i : cflags) {
        if (inPlace) {
            std::string cflag = i;
            boost::replace_all(cflag, "#", pathToString(topSourcePath));
            i = cflag;
        }
        else {
            std::string cflag = i;
            boost::replace_all(cflag, "#", pathToString(topInstallPath / "include"));
            i = cflag;
        }
    }


    // Print the requested variable
    if (vm.count("libs") > 0) {
        printFlags(libs);
    }
    if (vm.count("cflags") > 0) {
        printFlags(cflags);
    }
    return 0;
}
