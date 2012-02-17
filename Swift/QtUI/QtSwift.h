/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>

#include <Swiften/TLS/PlatformTLSFactories.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <string>
#include "Swiften/Base/Platform.h"
#include "Swiften/EventLoop/Qt/QtEventLoop.h"
#include "QtSettingsProvider.h"
#if defined(SWIFTEN_PLATFORM_MACOSX)
#include "SwifTools/Application/CocoaApplication.h"
#endif
#if defined(SWIFTEN_PLATFORM_WINDOWS)
#include "WindowsNotifier.h"
#endif
#include "SwifTools/Idle/PlatformIdleQuerier.h"
#include "SwifTools/Idle/ActualIdleDetector.h"

namespace po = boost::program_options;

class QSplitter;

namespace Swift {
	class QtUIFactory;
	class CertificateStorageFactory;
	class Dock;
	class Notifier;
	class StoragesFactory;
	class AutoUpdater;
	class ApplicationPathProvider;
	class AvatarStorage;
	class CapsStorage;
	class MainController;
	class QtSystemTray;
	class QtChatTabs;
	class QtChatWindowFactory;
	class QtSoundPlayer;
	class QtMUCSearchWindowFactory;
	class QtUserSearchWindowFactory;
	class EventLoop;
	class URIHandler;
	class SettingsProviderHierachy;
	class XMLSettingsProvider;
		
	class QtSwift : public QObject {
		Q_OBJECT
		public:
			QtSwift(const po::variables_map& options);
			static po::options_description getOptionsDescription();
			~QtSwift();
		private:
			XMLSettingsProvider* loadSettingsFile(const QString& fileName);
		private:
			QtEventLoop clientMainThreadCaller_;
			PlatformTLSFactories tlsFactories_;
			BoostNetworkFactories networkFactories_;
			QtChatWindowFactory* chatWindowFactory_;
			std::vector<MainController*> mainControllers_;
			std::vector<QtSystemTray*> systemTrays_;
			std::vector<QtUIFactory*> uiFactories_;
			QtSettingsProvider* qtSettings_;
			XMLSettingsProvider* xmlSettings_;
			SettingsProviderHierachy* settingsHierachy_;
			QSplitter* splitter_;
			QtSoundPlayer* soundPlayer_;
			Dock* dock_;
			URIHandler* uriHandler_;
			QtChatTabs* tabs_;
			ApplicationPathProvider* applicationPathProvider_;
			StoragesFactory* storagesFactory_;
			CertificateStorageFactory* certificateStorageFactory_;
			AutoUpdater* autoUpdater_;
			Notifier* notifier_;
			PlatformIdleQuerier idleQuerier_;
			ActualIdleDetector idleDetector_;
#if defined(SWIFTEN_PLATFORM_MACOSX)
			CocoaApplication cocoaApplication_;
#endif
	};
}
