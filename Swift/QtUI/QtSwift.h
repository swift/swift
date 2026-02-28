/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <string>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

#include <Swiften/Base/Platform.h>
#include <Swiften/Client/ClientOptions.h>
#include <Swiften/EventLoop/Qt/QtEventLoop.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/TLS/PlatformTLSFactories.h>

#include <SwifTools/AutoUpdater/AutoUpdater.h>
#include <SwifTools/Idle/ActualIdleDetector.h>
#include <SwifTools/Idle/PlatformIdleQuerier.h>

#include <Swift/QtUI/QtSettingsProvider.h>

#if defined(SWIFTEN_PLATFORM_MACOSX)
#include <SwifTools/Application/CocoaApplication.h>
#include <CocoaApplicationActivateHelper.h>
#endif
#if defined(SWIFTEN_PLATFORM_WINDOWS)
#include <WindowsNotifier.h>
#endif

namespace po = boost::program_options;

class QSplitter;

namespace Swift {
    class ApplicationPathProvider;
    class AvatarStorage;
    class CapsStorage;
    class CertificateStorageFactory;
    class Dock;
    class EventLoop;
    class AccountController;
    class Notifier;
    class QtChatTabs;
    class QtChatWindowFactory;
    class QtLoginWindow;
    class QtMUCSearchWindowFactory;
    class QtSingleWindow;
    class QtSoundPlayer;
    class QtSystemTray;
    class QtUIFactory;
    class QtUserSearchWindowFactory;
    class SettingsProviderHierachy;
    class StatusCache;
    class StoragesFactory;
    class URIHandler;
    class XMLSettingsProvider;

    class QtSwift : public QObject {
        Q_OBJECT
        public:
            QtSwift(const po::variables_map& options);
            static po::options_description getOptionsDescription();
            ~QtSwift();

        private slots:
            void handleAboutToQuit();
            void handleAutoUpdaterStateChanged(AutoUpdater::State updatedState);
            void handleWantsToAddAccount();

        private:
            XMLSettingsProvider* loadSettingsFile(const QString& fileName);
            void loadEmoticonsFile(const QString& fileName, std::map<std::string, std::string>& emoticons);
            static const std::string& updateChannelToFeed(const std::string& channel);
            QtLoginWindow* addAccount();
            ClientOptions parseClientOptions(const std::string& optionString);
            void restoreAccounts();
            /**
            * Upgrades the config from pre-multi-account to post-multi-account format (added in 5.0).
            */
            void migrateLastLoginAccount();

        private:
            QtEventLoop clientMainThreadCaller_;
            PlatformTLSFactories tlsFactories_;
            BoostNetworkFactories networkFactories_;
            QtChatWindowFactory* chatWindowFactory_;
            std::vector<AccountController*> accountControllers_;
            std::vector<QtSystemTray*> systemTrays_;
            std::vector<QtUIFactory*> uiFactories_;
            QtSettingsProvider* qtSettings_;
            XMLSettingsProvider* xmlSettings_;
            SettingsProviderHierachy* settingsHierarchy_;
            QtSingleWindow* splitter_;
            QtSoundPlayer* soundPlayer_;
            Dock* dock_;
            URIHandler* uriHandler_;
            ApplicationPathProvider* applicationPathProvider_;
            StoragesFactory* storagesFactory_;
            CertificateStorageFactory* certificateStorageFactory_;
            AutoUpdater* autoUpdater_ = nullptr;
            Notifier* notifier_;
            StatusCache* statusCache_;
            PlatformIdleQuerier idleQuerier_;
            ActualIdleDetector idleDetector_;
            std::map<std::string, std::string> emoticons_;
            bool enableAdHocCommandOnJID_ = false;
            bool useDelayForLatency_;
#if defined(SWIFTEN_PLATFORM_MACOSX)
            CocoaApplication cocoaApplication_;
            CocoaApplicationActivateHelper cocoaApplicationActivateHelper_;
#endif
    };
}
