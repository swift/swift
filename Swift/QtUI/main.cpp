/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options.hpp>

#include <iostream>

#include <QApplication>

#include "QtSwift.h"

int main(int argc, char* argv[]) {
	boost::program_options::options_description desc = Swift::QtSwift::getOptionsDescription();
	boost::program_options::variables_map vm;
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	} catch (boost::program_options::unknown_option option) {
		std::cout << "Ignoring unknown option " << option.get_option_name() << " but continuing." <<  std::endl;
	}
	boost::program_options::notify(vm);
	if (vm.count("help") > 0) {
		std::cout << desc << "\n";
		return 1;
	}
	QApplication app(argc, argv);
	Swift::QtSwift swift(vm);
	return app.exec();
}
