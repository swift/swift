#ifndef SWIFTEN_MainWindow_H
#define SWIFTEN_MainWindow_H

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/StatusShow.h"

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class TreeWidget;

	class MainWindow {
		public:
			virtual ~MainWindow() {};
			virtual TreeWidget* getTreeWidget() = 0;
			virtual void setMyName(const String& name) = 0;
			virtual void setMyAvatarPath(const String& path) = 0;
			virtual void setMyStatusText(const String& status) = 0;
			
			boost::signal<void (const JID&)> onStartChatRequest;
			boost::signal<void (const JID&, const String&)> onAddContactRequest;
			boost::signal<void (const JID&, const String&)> onJoinMUCRequest;
			boost::signal<void (StatusShow::Type, const String&)> onChangeStatusRequest;
			boost::signal<void (bool)> onShowOfflineToggled;
	};
}
#endif

