/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Disco/GetDiscoItemsRequest.h>
#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
	class IQRouter;
	class MainWindow;
	class UIEventStream;
	class AdHocCommandWindowFactory;
	class AdHocManager {
		public:
			AdHocManager(const JID& jid, AdHocCommandWindowFactory* factory, IQRouter* iqRouter, UIEventStream* uiEventStream, MainWindow* mainWindow);
			~AdHocManager();
			void setServerDiscoInfo(boost::shared_ptr<DiscoInfo> info);
		private:
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			void handleServerDiscoItemsResponse(boost::shared_ptr<DiscoItems>, ErrorPayload::ref error);
			JID jid_;
			IQRouter* iqRouter_;
			UIEventStream* uiEventStream_;
			MainWindow* mainWindow_;
			AdHocCommandWindowFactory* factory_;
			GetDiscoItemsRequest::ref discoItemsRequest_;
	};
}
