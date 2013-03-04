/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options.hpp>
#include <boost/version.hpp>
#include <iostream>
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QLocale>
#include <QStringList>

#include <Swift/Controllers/Translator.h>
#include <Swift/Controllers/ApplicationInfo.h>
#include <Swift/Controllers/BuildVersion.h>
#include <SwifTools/Application/PlatformApplicationPathProvider.h>
#include <SwifTools/CrashReporter.h>
#include <stdlib.h>

#include "QtSwift.h"
#include "QtTranslator.h"
#include "QtSwiftUtil.h"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);

	Swift::PlatformApplicationPathProvider applicationPathProvider(SWIFT_APPLICATION_NAME);

	Swift::CrashReporter crashReporter(applicationPathProvider.getDataDir() / "crashes");

#if QT_VERSION < 0x050000
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

	// Parse program options
	boost::program_options::options_description desc = Swift::QtSwift::getOptionsDescription();
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
		std::cout << SWIFT_APPLICATION_NAME << " is an instant messaging client for the XMPP network." << std::endl;
		std::cout << std::endl;
		std::cout << "Usage: " << argv[0] << " [OPTIONS]..." << std::endl;
		std::cout << std::endl;
		std::cout << desc << std::endl;
		return 1;
	}
	if (vm.count("version") > 0) {
		std::cout << SWIFT_APPLICATION_NAME << " " << buildVersion << std::endl;
		return 0;
	}

	// Translation
#if QT_VERSION < 0x050000
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
#endif
	boost::filesystem::path someTranslationPath = applicationPathProvider.getResourcePath("/translations/swift_en.qm");

	QTranslator qtTranslator;
	if (!someTranslationPath.empty()) {
#if QT_VERSION >= 0x040800
		if (vm.count("language") > 0) {
			qtTranslator.load(QString(SWIFT_APPLICATION_NAME).toLower() + "_" + P2QSTRING(vm["language"].as<std::string>()), someTranslationPath.parent_path().string().c_str());
		}
		else {
			qtTranslator.load(QLocale::system(), QString(SWIFT_APPLICATION_NAME).toLower(), "_", someTranslationPath.parent_path().string().c_str());
		}
#else
		//std::cout << "Loading " << std::string(QLocale::system().name().toUtf8()) << std::endl;
		qtTranslator.load(QString(SWIFT_APPLICATION_NAME).toLower() + "_" + QLocale::system().name(), someTranslationPath.parent_path().string().c_str());
#endif
	}
	app.installTranslator(&qtTranslator);
	QtTranslator swiftTranslator;
	Swift::Translator::setInstance(&swiftTranslator);


	Swift::QtSwift swift(vm);
	int result = app.exec();

	Swift::Translator::setInstance(NULL);

	return result;
}
