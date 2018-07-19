/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtSwift.h>

#include <map>
#include <string>

#include <boost/bind.hpp>

#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QMap>
#include <QMessageBox>
#include <qdebug.h>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/Path.h>
#include <Swiften/Base/Paths.h>
#include <Swiften/Base/Platform.h>
#include <Swiften/Base/String.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/TLS/TLSContextFactory.h>

#include <SwifTools/Application/PlatformApplicationPathProvider.h>
#include <SwifTools/AutoUpdater/AutoUpdater.h>
#include <SwifTools/AutoUpdater/PlatformAutoUpdaterFactory.h>
#include <SwifTools/EmojiMapper.h>

#include <Swift/Controllers/ApplicationInfo.h>
#include <Swift/Controllers/BuildVersion.h>
#include <Swift/Controllers/AccountController.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/Settings/SettingsProviderHierachy.h>
#include <Swift/Controllers/Settings/XMLSettingsProvider.h>
#include <Swift/Controllers/StatusCache.h>
#include <Swift/Controllers/Storages/CertificateFileStorageFactory.h>
#include <Swift/Controllers/Storages/FileStoragesFactory.h>

#include <Swift/QtUI/QtChatTabs.h>
#include <Swift/QtUI/QtChatWindowFactory.h>
#include <Swift/QtUI/QtLoginWindow.h>
#include <Swift/QtUI/QtSingleWindow.h>
#include <Swift/QtUI/QtSoundPlayer.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtSystemTray.h>
#include <Swift/QtUI/QtUIFactory.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/QtUI/SwiftUpdateFeeds.h>

#if defined(SWIFTEN_PLATFORM_WINDOWS)
#include <Swift/QtUI/WindowsNotifier.h>
#elif defined(HAVE_GROWL)
#include <SwifTools/Notifier/GrowlNotifier.h>
#elif defined(SWIFTEN_PLATFORM_LINUX)
#include <Swift/QtUI/FreeDesktopNotifier.h>
#elif defined(SWIFTEN_PLATFORM_MACOSX)
#include <SwifTools/Notifier/NotificationCenterNotifier.h>
#else
#include <SwifTools/Notifier/NullNotifier.h>
#endif

#if defined(SWIFTEN_PLATFORM_MACOSX)
#include <SwifTools/Dock/MacOSXDock.h>
#else
#include <SwifTools/Dock/NullDock.h>
#endif

#if defined(SWIFTEN_PLATFORM_MACOSX)
#include <Swift/QtUI/QtURIHandler.h>
#elif defined(SWIFTEN_PLATFORM_WIN32)
#include <SwifTools/URIHandler/NullURIHandler.h>
#else
#include <Swift/QtUI/QtDBUSURIHandler.h>
#endif

#if defined(SWIFTEN_PLATFORM_MACOSX)
#include <Swift/QtUI/CocoaUIHelpers.h>
#endif

namespace Swift{

po::options_description QtSwift::getOptionsDescription() {
    po::options_description result("Options");
    result.add_options()
        ("debug", "Turn on debug logging")
        ("help", "Show this help message")
        ("version", "Show version information")
        ("no-tabs", "Don't manage chat windows in tabs (unsupported)")
        ("latency-debug", "Use latency debugging (unsupported)")
        ("enable-jid-adhocs", "Enable AdHoc commands to custom JIDs.")
#if QT_VERSION >= 0x040800
        ("language", po::value<std::string>(), "Use a specific language, instead of the system-wide one")
#endif
        ("logfile", po::value<std::string>()->implicit_value(""), "Save all logging information to a file")
        ("enable-future", "Enable future features (unsupported). This will persist across restarts")
        ("disable-future", "Disable future features. This will persist across restarts")
        ;
    return result;
}

XMLSettingsProvider* QtSwift::loadSettingsFile(const QString& fileName) {
    QFile configFile(fileName);
    if (configFile.exists() && configFile.open(QIODevice::ReadOnly)) {
        QString xmlString;
        while (!configFile.atEnd()) {
            QByteArray line = configFile.readLine();
            xmlString += line + "\n";
        }
        return new XMLSettingsProvider(Q2PSTRING(xmlString));
    }
    return new XMLSettingsProvider("");
}

void QtSwift::loadEmoticonsFile(const QString& fileName, std::map<std::string, std::string>& emoticons)  {
    QFile file(fileName);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line.replace("\n", "");
            line.replace("\r", "");
            QStringList tokens = line.split(" ");
            if (tokens.size() == 2) {
                QString emoticonFile = tokens[1];
                if (!emoticonFile.startsWith(":/") && !emoticonFile.startsWith("qrc:/")) {
                    emoticonFile = "file://" + emoticonFile;
                }
                emoticons[Q2PSTRING(tokens[0])] = Q2PSTRING(emoticonFile);
            }
        }
    }
}

const std::string& QtSwift::updateChannelToFeed(const std::string& channel) {
    static const std::string invalidChannel;
    if (channel == UpdateFeeds::StableChannel) {
        return UpdateFeeds::StableAppcastFeed;
    }
    else if (channel == UpdateFeeds::TestingChannel) {
        return UpdateFeeds::TestingAppcastFeed;
    }
    else if (channel == UpdateFeeds::DevelopmentChannel) {
        return UpdateFeeds::DevelopmentAppcastFeed;
    }
    else {
        return invalidChannel;
    }
}

QtSwift::QtSwift(const po::variables_map& options) : networkFactories_(&clientMainThreadCaller_), autoUpdater_(nullptr), idleDetector_(&idleQuerier_, networkFactories_.getTimerFactory(), 1000) {
    QCoreApplication::setApplicationName(SWIFT_APPLICATION_NAME);
    QCoreApplication::setOrganizationName(SWIFT_ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(SWIFT_ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationVersion(buildVersion);

    qtSettings_ = new QtSettingsProvider();
    xmlSettings_ = loadSettingsFile(P2QSTRING(pathToString(Paths::getExecutablePath() / "system-settings.xml")));
    settingsHierarchy_ = new SettingsProviderHierachy();
    settingsHierarchy_->addProviderToTopOfStack(xmlSettings_);
    settingsHierarchy_->addProviderToTopOfStack(qtSettings_);

    networkFactories_.getTLSContextFactory()->setDisconnectOnCardRemoval(settingsHierarchy_->getSetting(SettingConstants::DISCONNECT_ON_CARD_REMOVAL));

    loadEmoticonsFile(":/emoticons/emoticons.txt", emoticons_);
    loadEmoticonsFile(P2QSTRING(pathToString(Paths::getExecutablePath() / "emoticons.txt")), emoticons_);

    splitter_ = new QtSingleWindow(qtSettings_);
    connect(splitter_, SIGNAL(wantsToAddAccount()), this, SLOT(handleWantsToAddAccount()));

    if (options.count("debug")) {
        Log::setLogLevel(Swift::Log::debug);
    }

    if (options.count("enable-future")) {
        settingsHierarchy_->storeSetting(SettingConstants::FUTURE, true);
    }

    if (options.count("disable-future")) {
        settingsHierarchy_->storeSetting(SettingConstants::FUTURE, false);
    }

    if (options.count("logfile")) {
        try {
            std::string fileName = options["logfile"].as<std::string>();
            Log::setLogFile(fileName);
        }
        catch (...) {
            SWIFT_LOG(error) << "Error while retrieving the specified log file name from the command line" << std::endl;
        }
    }
    //TODO this old option can be purged
    useDelayForLatency_ = options.count("latency-debug") > 0;

    // Load fonts
    std::vector<std::string> fontNames = {
        "themes/Default/Lato2OFL/Lato-Black.ttf",
        "themes/Default/Lato2OFL/Lato-BlackItalic.ttf",
        "themes/Default/Lato2OFL/Lato-Bold.ttf",
        "themes/Default/Lato2OFL/Lato-BoldItalic.ttf",
        "themes/Default/Lato2OFL/Lato-Hairline.ttf",
        "themes/Default/Lato2OFL/Lato-HairlineItalic.ttf",
        "themes/Default/Lato2OFL/Lato-Heavy.ttf",
        "themes/Default/Lato2OFL/Lato-HeavyItalic.ttf",
        "themes/Default/Lato2OFL/Lato-Italic.ttf",
        "themes/Default/Lato2OFL/Lato-Light.ttf",
        "themes/Default/Lato2OFL/Lato-LightItalic.ttf",
        "themes/Default/Lato2OFL/Lato-Medium.ttf",
        "themes/Default/Lato2OFL/Lato-MediumItalic.ttf",
        "themes/Default/Lato2OFL/Lato-Regular.ttf",
        "themes/Default/Lato2OFL/Lato-Semibold.ttf",
        "themes/Default/Lato2OFL/Lato-SemiboldItalic.ttf",
        "themes/Default/Lato2OFL/Lato-Thin.ttf",
        "themes/Default/Lato2OFL/Lato-ThinItalic.ttf"
    };

    for (auto&& fontName : fontNames) {
        std::string fontPath = std::string(":/") + fontName;
        int error = QFontDatabase::addApplicationFont(P2QSTRING(fontPath));
        SWIFT_LOG_ASSERT(error != -1, error) << "Failed to load font " << fontPath << std::endl;
    }

#ifdef SWIFTEN_PLATFORM_LINUX
    std::string fontPath = std::string(":/themes/Default/Noto/NotoColorEmoji.ttf");
    int error = QFontDatabase::addApplicationFont(P2QSTRING(fontPath));
    SWIFT_LOG_ASSERT(error != -1, error) << "Failed to load font " << fontPath << std::endl;
    QFont::insertSubstitution(QApplication::font().family(),"NotoColorEmoji");
#endif
#ifdef SWIFTEN_PLATFORM_WINDOWS
    QFont::insertSubstitution(QApplication::font().family(), "Segoe UI Emoji");
#endif
    enableAdHocCommandOnJID_ = options.count("enable-jid-adhocs") > 0;
    applicationPathProvider_ = new PlatformApplicationPathProvider(SWIFT_APPLICATION_NAME);
    storagesFactory_ = new FileStoragesFactory(applicationPathProvider_->getDataDir(), networkFactories_.getCryptoProvider());
    certificateStorageFactory_ = new CertificateFileStorageFactory(applicationPathProvider_->getDataDir(), tlsFactories_.getCertificateFactory(), networkFactories_.getCryptoProvider());
    soundPlayer_ = new QtSoundPlayer(applicationPathProvider_);

    // Ugly, because the dock depends on the tray, but the temporary
    // multi-account hack creates one tray per account.
    QtSystemTray* systemTray = new QtSystemTray();
    systemTrays_.push_back(systemTray);

#if defined(HAVE_GROWL)
    notifier_ = new GrowlNotifier(SWIFT_APPLICATION_NAME);
#elif defined(SWIFTEN_PLATFORM_WINDOWS)
    notifier_ = new WindowsNotifier(SWIFT_APPLICATION_NAME, applicationPathProvider_->getResourcePath("/images/logo-icon-32.png"), systemTray->getQSystemTrayIcon());
#elif defined(SWIFTEN_PLATFORM_LINUX)
    notifier_ = new FreeDesktopNotifier(SWIFT_APPLICATION_NAME);
#elif defined(SWIFTEN_PLATFORM_MACOSX)
    notifier_ = new NotificationCenterNotifier();
#else
    notifier_ = new NullNotifier();
#endif

#if defined(SWIFTEN_PLATFORM_MACOSX)
    dock_ = new MacOSXDock(&cocoaApplication_);
#else
    dock_ = new NullDock();
#endif

#if defined(SWIFTEN_PLATFORM_MACOSX)
    uriHandler_ = new QtURIHandler();
#elif defined(SWIFTEN_PLATFORM_WIN32)
    uriHandler_ = new NullURIHandler();
#else
    uriHandler_ = new QtDBUSURIHandler();
#endif

    statusCache_ = new StatusCache(applicationPathProvider_);

    splitter_->show();

    PlatformAutoUpdaterFactory autoUpdaterFactory;
    if (autoUpdaterFactory.isSupported() && settingsHierarchy_->getSetting(QtUISettingConstants::ENABLE_SOFTWARE_UPDATES)
        && !settingsHierarchy_->getSetting(QtUISettingConstants::SOFTWARE_UPDATE_CHANNEL).empty()) {
        autoUpdater_ = autoUpdaterFactory.createAutoUpdater(updateChannelToFeed(settingsHierarchy_->getSetting(QtUISettingConstants::SOFTWARE_UPDATE_CHANNEL)));
        autoUpdater_->checkForUpdates();
        autoUpdater_->onUpdateStateChanged.connect(boost::bind(&QtSwift::handleAutoUpdaterStateChanged, this, _1));

        settingsHierarchy_->onSettingChanged.connect([&](const std::string& path) {
            if (path == QtUISettingConstants::SOFTWARE_UPDATE_CHANNEL.getKey()) {
                autoUpdater_->setAppcastFeed(updateChannelToFeed(settingsHierarchy_->getSetting(QtUISettingConstants::SOFTWARE_UPDATE_CHANNEL)));
                autoUpdater_->checkForUpdates();
            }
        });
    }
    migrateLastLoginAccount();
    restoreAccounts();
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
}

QtSwift::~QtSwift() {
    delete autoUpdater_;
    for (auto* factory : uiFactories_) {
        delete factory;
    }
    for (auto* controller : accountControllers_) {
        delete controller;
    }
    delete notifier_;
    for (auto* tray : systemTrays_) {
        delete tray;
    }
    delete splitter_;
    delete settingsHierarchy_;
    delete qtSettings_;
    delete xmlSettings_;
    delete statusCache_;
    delete uriHandler_;
    delete dock_;
    delete soundPlayer_;
    delete certificateStorageFactory_;
    delete storagesFactory_;
    delete applicationPathProvider_;
}

void QtSwift::handleAboutToQuit() {
#if defined(SWIFTEN_PLATFORM_MACOSX)
    // This is required so Sparkle knows about the application shutting down
    // and can update the application in background.
     CocoaUIHelpers::sendCocoaApplicationWillTerminateNotification();
#endif
}

void QtSwift::handleAutoUpdaterStateChanged(AutoUpdater::State updatedState) {
    switch (updatedState) {
    case AutoUpdater::State::NotCheckedForUpdatesYet:
        break;
    case AutoUpdater::State::CheckingForUpdate:
        break;
    case AutoUpdater::State::DownloadingUpdate:
        break;
    case AutoUpdater::State::ErrorCheckingForUpdate:
        break;
    case AutoUpdater::State::NoUpdateAvailable:
        break;
    case AutoUpdater::State::RestartToInstallUpdate:
        notifier_->showMessage(Notifier::SystemMessage, Q2PSTRING(tr("Swift Update Available")), Q2PSTRING(tr("Restart Swift to update to the new Swift version.")), "", [](){});
    }
}

void QtSwift::handleWantsToAddAccount() {
    auto loginWindow = addAccount();
    if (!settingsHierarchy_->getSetting(SettingConstants::FORGET_PASSWORDS)) {
        for (const auto& profile : settingsHierarchy_->getAvailableProfiles()) {
            ProfileSettingsProvider profileSettings(profile, settingsHierarchy_);
            if (profileSettings.getIntSetting("enabled", 0)) {
                // No point showing accounts that're already logged in
                continue;
            }
            const auto& password = profileSettings.getStringSetting("pass");
            const auto& certificate = profileSettings.getStringSetting("certificate");
            const auto& jid = profileSettings.getStringSetting("jid");
            const auto& clientOptions = parseClientOptions(profileSettings.getStringSetting("options"));
            loginWindow->addAvailableAccount(jid, password, certificate, clientOptions);
        }
    }
}

void QtSwift::restoreAccounts() {
    if (!settingsHierarchy_->getSetting(SettingConstants::FORGET_PASSWORDS)) {
        for (const auto& profile : settingsHierarchy_->getAvailableProfiles()) {
            ProfileSettingsProvider profileSettings(profile, settingsHierarchy_);
            if (!profileSettings.getIntSetting("enabled", 0)) {
                continue;
            }
            const auto& jid = profileSettings.getStringSetting("jid");
            const auto& password = profileSettings.getStringSetting("pass");
            const auto& certificate = profileSettings.getStringSetting("certificate");
            const auto& clientOptions = parseClientOptions(profileSettings.getStringSetting("options"));
            auto loginWindow = addAccount();
            loginWindow->addAvailableAccount(jid, password, certificate, clientOptions);
            loginWindow->loginClicked();
        }
    }
}

void QtSwift::migrateLastLoginAccount() {
    const SettingsProvider::Setting<bool> loginAutomatically = SettingsProvider::Setting<bool>("loginAutomatically", false);
    if (settingsHierarchy_->getSetting(loginAutomatically)) {
        auto selectedLoginJID = settingsHierarchy_->getSetting(SettingsProvider::Setting<std::string>("lastLoginJID", ""));
        for (const auto& profile : settingsHierarchy_->getAvailableProfiles()) {
            ProfileSettingsProvider profileSettings(profile, settingsHierarchy_);
            if (profileSettings.getStringSetting("jid") == selectedLoginJID) {
                profileSettings.storeInt("enabled", 1);
                break;
            }
        }
        settingsHierarchy_->storeSetting(loginAutomatically, false);
    }
}

QtLoginWindow* QtSwift::addAccount() {
    if (uiFactories_.size() > 0) {
        // Don't add the first tray (see note above)
        systemTrays_.push_back(new QtSystemTray());
    }
    auto tabs = new QtChatTabs(settingsHierarchy_, true);
    QtUIFactory* uiFactory = new QtUIFactory(settingsHierarchy_, qtSettings_, tabs, splitter_, systemTrays_[systemTrays_.size() - 1], networkFactories_.getTimerFactory(), statusCache_, autoUpdater_, emoticons_, enableAdHocCommandOnJID_);
    uiFactories_.push_back(uiFactory);
    AccountController* accountController = new AccountController(
        &clientMainThreadCaller_,
        &networkFactories_,
        uiFactory,
        settingsHierarchy_,
        systemTrays_[systemTrays_.size() - 1],
        soundPlayer_,
        storagesFactory_,
        certificateStorageFactory_,
        dock_,
        notifier_,
        uriHandler_,
        &idleDetector_,
        emoticons_,
        useDelayForLatency_);
    accountControllers_.push_back(accountController);

    //FIXME - accountController has already created the window, so we can pass null here and get the old one
    auto loginWindow = uiFactory->createLoginWindow(nullptr);

    return dynamic_cast<QtLoginWindow*>(loginWindow);
}

//FIXME: Switch all this to boost::serialise

#define CHECK_PARSE_LENGTH if (i >= segments.size()) {return result;}
#define PARSE_INT_RAW(defaultValue) CHECK_PARSE_LENGTH intVal = defaultValue; try {intVal = boost::lexical_cast<int>(segments[i]);} catch(const boost::bad_lexical_cast&) {};i++;
#define PARSE_STRING_RAW CHECK_PARSE_LENGTH stringVal = byteArrayToString(Base64::decode(segments[i]));i++;

#define PARSE_BOOL(option, defaultValue) PARSE_INT_RAW(defaultValue); result.option = (intVal == 1);
#define PARSE_INT(option, defaultValue) PARSE_INT_RAW(defaultValue); result.option = intVal;
#define PARSE_STRING(option) PARSE_STRING_RAW; result.option = stringVal;
#define PARSE_SAFE_STRING(option) PARSE_STRING_RAW; result.option = SafeString(createSafeByteArray(stringVal));
#define PARSE_URL(option) {PARSE_STRING_RAW; result.option = URL::fromString(stringVal);}


ClientOptions QtSwift::parseClientOptions(const std::string& optionString) {
    ClientOptions result;
    size_t i = 0;
    int intVal = 0;
    std::string stringVal;
    std::vector<std::string> segments = String::split(optionString, ',');

    PARSE_BOOL(useStreamCompression, 1);
    PARSE_INT_RAW(-1);
    switch (intVal) {
    case 1: result.useTLS = ClientOptions::NeverUseTLS; break;
    case 2: result.useTLS = ClientOptions::UseTLSWhenAvailable; break;
    case 3: result.useTLS = ClientOptions::RequireTLS; break;
    default:;
    }
    PARSE_BOOL(allowPLAINWithoutTLS, 0);
    PARSE_BOOL(useStreamResumption, 0);
    PARSE_BOOL(useAcks, 1);
    PARSE_STRING(manualHostname);
    PARSE_INT(manualPort, -1);
    PARSE_INT_RAW(-1);
    switch (intVal) {
    case 1: result.proxyType = ClientOptions::NoProxy; break;
    case 2: result.proxyType = ClientOptions::SystemConfiguredProxy; break;
    case 3: result.proxyType = ClientOptions::SOCKS5Proxy; break;
    case 4: result.proxyType = ClientOptions::HTTPConnectProxy; break;
    }
    PARSE_STRING(manualProxyHostname);
    PARSE_INT(manualProxyPort, -1);
    PARSE_URL(boshURL);
    PARSE_URL(boshHTTPConnectProxyURL);
    PARSE_SAFE_STRING(boshHTTPConnectProxyAuthID);
    PARSE_SAFE_STRING(boshHTTPConnectProxyAuthPassword);
    PARSE_BOOL(tlsOptions.schannelTLS1_0Workaround, false);
    PARSE_BOOL(singleSignOn, false);

    return result;
}


}
