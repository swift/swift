/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options.hpp>
#include <boost/version.hpp>
#include <iostream>

#include "Swiftob.h"

int main(int argc, char* argv[]) {
	boost::program_options::options_description desc = Swiftob::getOptionsDescription();
	boost::program_options::variables_map vm;
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	} catch (const boost::program_options::unknown_option& option) {
#if BOOST_VERSION >= 104200
		std::cout << "Ignoring unknown option " << option.get_option_name() << " but continuing." <<  std::endl;
#else
		std::cout << "Error: " << option.what() << " (continuing)" <<  std::endl;
#endif
	}
	boost::program_options::notify(vm);
	if (vm.count("help") > 0) {
		std::cout << desc << "\n";
		return 1;
	}

	Swiftob bot(vm);
	int result = bot.exec();

	return result;
}
