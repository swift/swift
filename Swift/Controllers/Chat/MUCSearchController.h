/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>
#include "Swiften/Base/boost_bsignals.h"

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"

#include "Swift/Controllers/UIEvents/UIEvent.h"
#include "Swift/Controllers/Chat/MUCSearchController.h"
#include "Swift/Controllers/Settings/SettingsProvider.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Elements/DiscoItems.h"
#include "Swiften/Elements/ErrorPayload.h"

namespace Swift {
	class UIEventStream;
	class MUCSearchWindow;
	class MUCSearchWindowFactory;
	class IQRouter;
	class DiscoServiceWalker;
	class NickResolver;

	class MUCService {
		public:
			class MUCRoom {
				public:
					MUCRoom(const String& node, const String& name, int occupants) : node_(node), name_(name), occupants_(occupants) {}
					String getNode() {return node_;}
					String getName() {return name_;}
					int getOccupantCount() {return occupants_;}
				private:
					String node_;
					String name_;
					int occupants_;
			};

			MUCService() {error_ = false; complete_ = false;}

			void setComplete(bool complete) {
				complete_ = complete;
			}

			void setName(const String& name) {
				name_ = name;
			}

			void setJID(const JID& jid) {
				jid_ = jid;
			}

			bool getComplete() const {
				return complete_;
			}

			JID getJID() const {
				return jid_;
			}

			String getName() const {
				return name_;
			}

			void setError(const String& errorText) {error_ = true; errorText_ = errorText;}

			void clearRooms() {rooms_.clear();}

			void addRoom(const MUCRoom& room) {rooms_.push_back(room);}

			std::vector<MUCRoom> getRooms() const {return rooms_;}
		private:
			String name_;
			JID jid_;
			std::vector<MUCRoom> rooms_;
			bool complete_;
			bool error_;
			String errorText_;
	};

	class MUCSearchController {
		public:
			MUCSearchController(const JID& jid, MUCSearchWindowFactory* mucSearchWindowFactory, IQRouter* iqRouter, SettingsProvider* settings);
			~MUCSearchController();

			void openSearchWindow();

		public:
			boost::signal<void (const JID&)> onMUCSelected;

		private:
			void handleSearchService(const JID& jid);
			void handleRoomsItemsResponse(boost::shared_ptr<DiscoItems> items, ErrorPayload::ref error, const JID& jid);
			void handleDiscoError(const JID& jid, ErrorPayload::ref error);
			void handleDiscoServiceFound(const JID&, boost::shared_ptr<DiscoInfo>);
			void handleDiscoWalkFinished();
			void handleMUCSearchFinished(const boost::optional<JID>& result);
			void removeService(const JID& jid);
			void refreshView();
			void loadSavedServices();
			void addToSavedServices(const JID& jid);
			void updateInProgressness();

		private:
			JID jid_;
			MUCSearchWindowFactory* factory_;
			IQRouter* iqRouter_;
			SettingsProvider* settings_;
			MUCSearchWindow* window_;
			DiscoServiceWalker* walker_;
			std::list<JID> services_;
			std::list<JID> savedServices_;
			std::map<JID, MUCService> serviceDetails_;
			std::vector<DiscoServiceWalker*> walksInProgress_;
			int itemsInProgress_;
	};
}
