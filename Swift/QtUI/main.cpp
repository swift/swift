/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options.hpp>


#include <QApplication>

#include "QtSwift.h"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	boost::program_options::options_description desc = Swift::QtSwift::getOptionsDescription();
	boost::program_options::variables_map vm;	
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	Swift::QtSwift swift(vm);
	return app.exec();
}
