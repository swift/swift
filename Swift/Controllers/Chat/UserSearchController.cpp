/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/Chat/UserSearchController.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Disco/GetDiscoInfoRequest.h>
#include <Swiften/Disco/GetDiscoItemsRequest.h>

#include <Swift/Controllers/DiscoServiceWalker.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/RequestChatWithUserDialogUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestAddUserDialogUIEvent.h>
#include <Swift/Controllers/UIInterfaces/UserSearchWindow.h>
#include <Swift/Controllers/UIInterfaces/UserSearchWindowFactory.h>
#include <Swift/Controllers/Roster/RosterController.h>

namespace Swift {
UserSearchController::UserSearchController(Type type, const JID& jid, UIEventStream* uiEventStream, UserSearchWindowFactory* factory, IQRouter* iqRouter, RosterController* rosterController) : type_(type), jid_(jid), uiEventStream_(uiEventStream), factory_(factory), iqRouter_(iqRouter), rosterController_(rosterController) {
	uiEventStream_->onUIEvent.connect(boost::bind(&UserSearchController::handleUIEvent, this, _1));
	window_ = NULL;
	discoWalker_ = NULL;
}

UserSearchController::~UserSearchController() {
	endDiscoWalker();
	delete discoWalker_;
	if (window_) {
		window_->onFormRequested.disconnect(boost::bind(&UserSearchController::handleFormRequested, this, _1));
		window_->onSearchRequested.disconnect(boost::bind(&UserSearchController::handleSearch, this, _1, _2));
		delete window_;
	}
	uiEventStream_->onUIEvent.disconnect(boost::bind(&UserSearchController::handleUIEvent, this, _1));
}

void UserSearchController::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	bool handle = false;
	if (type_ == AddContact) {
		if (boost::dynamic_pointer_cast<RequestAddUserDialogUIEvent>(event)) {
			handle = true;
		}
	} else {
		if (boost::dynamic_pointer_cast<RequestChatWithUserDialogUIEvent>(event)) {
			handle = true;
		}
	}
	if (handle) {
		if (!window_) {
			window_ = factory_->createUserSearchWindow(type_ == AddContact ? UserSearchWindow::AddContact : UserSearchWindow::ChatToContact, uiEventStream_, rosterController_->getGroups());
			window_->onFormRequested.connect(boost::bind(&UserSearchController::handleFormRequested, this, _1));
			window_->onSearchRequested.connect(boost::bind(&UserSearchController::handleSearch, this, _1, _2));
			window_->setSelectedService(JID(jid_.getDomain()));
			window_->clear();
		}
		window_->show();
		return;
	}
}

void UserSearchController::handleFormRequested(const JID& service) {
	window_->setSearchError(false);
	window_->setServerSupportsSearch(true);

	//Abort a previous search if is active
	endDiscoWalker();
	delete discoWalker_;
	discoWalker_ = new DiscoServiceWalker(service, iqRouter_);
	discoWalker_->onServiceFound.connect(boost::bind(&UserSearchController::handleDiscoServiceFound, this, _1, _2));
	discoWalker_->onWalkComplete.connect(boost::bind(&UserSearchController::handleDiscoWalkFinished, this));
	discoWalker_->beginWalk();
}

void UserSearchController::endDiscoWalker() {
	if (discoWalker_) {
		discoWalker_->endWalk();
		discoWalker_->onServiceFound.disconnect(boost::bind(&UserSearchController::handleDiscoServiceFound, this, _1, _2));
		discoWalker_->onWalkComplete.disconnect(boost::bind(&UserSearchController::handleDiscoWalkFinished, this));
	}
}


void UserSearchController::handleDiscoServiceFound(const JID& jid, boost::shared_ptr<DiscoInfo> info) {
	bool isUserDirectory = false;
	bool supports55 = false;
	foreach (DiscoInfo::Identity identity, info->getIdentities()) {
		if ((identity.getCategory() == "directory"
			&& identity.getType() == "user")) {
			isUserDirectory = true;
		}
	}
	std::vector<std::string> features = info->getFeatures();
	supports55 = std::find(features.begin(), features.end(), DiscoInfo::JabberSearchFeature) != features.end();
	if (/*isUserDirectory && */supports55) { //FIXME: once M-Link correctly advertises directoryness.
		/* Abort further searches.*/
		endDiscoWalker();
		boost::shared_ptr<GenericRequest<SearchPayload> > searchRequest(new GenericRequest<SearchPayload>(IQ::Get, jid, boost::shared_ptr<SearchPayload>(new SearchPayload()), iqRouter_));
		searchRequest->onResponse.connect(boost::bind(&UserSearchController::handleFormResponse, this, _1, _2));
		searchRequest->send();
	}
}

void UserSearchController::handleFormResponse(boost::shared_ptr<SearchPayload> fields, ErrorPayload::ref error) {
	if (error || !fields) {
		window_->setServerSupportsSearch(false);
		return;
	}
	window_->setSearchFields(fields);
}

void UserSearchController::handleSearch(boost::shared_ptr<SearchPayload> fields, const JID& jid) {
	boost::shared_ptr<GenericRequest<SearchPayload> > searchRequest(new GenericRequest<SearchPayload>(IQ::Set, jid, fields, iqRouter_));
	searchRequest->onResponse.connect(boost::bind(&UserSearchController::handleSearchResponse, this, _1, _2));
	searchRequest->send();
}

void UserSearchController::handleSearchResponse(boost::shared_ptr<SearchPayload> resultsPayload, ErrorPayload::ref error) {
	if (error || !resultsPayload) {
		window_->setSearchError(true);
		return;
	}
	std::vector<UserSearchResult> results;
	foreach (SearchPayload::Item item, resultsPayload->getItems()) {
		JID jid(item.jid);
		std::map<std::string, std::string> fields;
		fields["first"] = item.first;
		fields["last"] = item.last;
		fields["nick"] = item.nick;
		fields["email"] = item.email;
		UserSearchResult result(jid, fields);
		results.push_back(result);
	}
	window_->setResults(results);
}

void UserSearchController::handleDiscoWalkFinished() {
	window_->setServerSupportsSearch(false);
	endDiscoWalker();
}

}
