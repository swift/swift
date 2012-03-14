/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/Elements/SearchPayload.h>
#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/VCard.h>

namespace Swift {
	class UIEventStream;
	class UIEvent;
	class UserSearchWindow;
	class UserSearchWindowFactory;
	class IQRouter;
	class DiscoServiceWalker;
	class RosterController;
	class VCardManager;

	class UserSearchResult {
		public:
			UserSearchResult(const JID& jid, const std::map<std::string, std::string>& fields) : jid_(jid), fields_(fields) {}
			const JID& getJID() const {return jid_;}
			const std::map<std::string, std::string>& getFields() const {return fields_;}
		private:
			JID jid_;
			std::map<std::string, std::string> fields_;
	};

	class UserSearchController {
		public:
			enum Type {AddContact, StartChat};
			UserSearchController(Type type, const JID& jid, UIEventStream* uiEventStream, VCardManager* vcardManager, UserSearchWindowFactory* userSearchWindowFactory, IQRouter* iqRouter, RosterController* rosterController);
			~UserSearchController();

		private:
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			void handleFormRequested(const JID& service);
			void handleDiscoServiceFound(const JID& jid, boost::shared_ptr<DiscoInfo> info);
			void handleDiscoWalkFinished();
			void handleFormResponse(boost::shared_ptr<SearchPayload> items, ErrorPayload::ref error);
			void handleSearch(boost::shared_ptr<SearchPayload> fields, const JID& jid);
			void handleSearchResponse(boost::shared_ptr<SearchPayload> results, ErrorPayload::ref error);
			void handleNameSuggestionRequest(const JID& jid);
			void handleVCardChanged(const JID& jid, VCard::ref vcard);
			void endDiscoWalker();

		private:
			Type type_;
			JID jid_;
			JID suggestionsJID_;
			UIEventStream* uiEventStream_;
			VCardManager* vcardManager_;
			UserSearchWindowFactory* factory_;
			IQRouter* iqRouter_;
			RosterController* rosterController_;
			UserSearchWindow* window_;
			DiscoServiceWalker* discoWalker_;
	};
}
