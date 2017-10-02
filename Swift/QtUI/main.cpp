/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cstdlib>
#include <iostream>
#include <locale>
#include <memory>
#include <sstream>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/version.hpp>

#include <QApplication>
#include <QLocale>
#include <QStringList>
#include <QTextCodec>
#include <QTranslator>

#include <Swiften/Base/Path.h>

#include <Swift/Controllers/ApplicationInfo.h>
#include <Swift/Controllers/BuildVersion.h>
#include <Swift/Controllers/Translator.h>

#include <SwifTools/Application/PlatformApplicationPathProvider.h>
#include <SwifTools/CrashReporter.h>

#include <Swift/QtUI/QtSwift.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtTranslator.h>

int main(int argc, char* argv[]) {
    Swift::PlatformApplicationPathProvider applicationPathProvider(SWIFT_APPLICATION_NAME);

    // Set crash report prefix to include date and version.
    std::stringstream prefix;

    // This date_facet will be cleaned up by the stringstream.
    auto outputFacet = new boost::gregorian::date_facet();
    outputFacet->format("%Y-%m-%d");
    prefix.imbue(std::locale(std::locale::classic(), outputFacet));

    prefix << buildVersion << "_" << boost::gregorian::date(boost::gregorian::day_clock::local_day()) << "_";

    Swift::CrashReporter crashReporter(applicationPathProvider.getDataDir() / "crashes", prefix.str());

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
        std::string language;
        if (vm.count("language") > 0) {
            try {
                language = vm["language"].as<std::string>();
            } catch (const boost::bad_any_cast&) {
            }
        }
        if (!language.empty()) {
            qtTranslator.load(QString(SWIFT_APPLICATION_NAME).toLower() + "_" + P2QSTRING(language), P2QSTRING(Swift::pathToString(someTranslationPath.parent_path())));
        }
        else {
            qtTranslator.load(QLocale::system(), QString(SWIFT_APPLICATION_NAME).toLower(), "_", P2QSTRING(Swift::pathToString(someTranslationPath)));
        }
#else
        //std::cout << "Loading " << std::string(QLocale::system().name().toUtf8()) << std::endl;
        qtTranslator.load(QString(SWIFT_APPLICATION_NAME).toLower() + "_" + QLocale::system().name(), P2QSTRING(Swift::pathToString(someTranslationPath)));
#endif
    }

    QApplication app(argc, argv);

    app.installTranslator(&qtTranslator);
    QtTranslator swiftTranslator;
    Swift::Translator::setInstance(&swiftTranslator);

#if QT_VERSION < 0x050501
    /*  According to Qt documenation, Qt is suppsoed to set the applications layout
     *  direction based on the translatable QT_LAYOUT_DIRECTION string. There is a
     *  bug in Qt prior version 5.5.1, i.e. QTBUG-43447, thus we set the layout
     *  direction manually based on the tranlsated QT_LAYOUT_DIRECTION string.
     */
    app.setLayoutDirection(QGuiApplication::tr("QT_LAYOUT_DIRECTION") == QLatin1String("RTL") ? Qt::RightToLeft : Qt::LeftToRight);
#endif

    Swift::QtSwift swift(vm);
    int result = app.exec();

    Swift::Translator::setInstance(nullptr);

    return result;
}
