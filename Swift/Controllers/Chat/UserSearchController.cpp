/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/UserSearchController.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Disco/GetDiscoInfoRequest.h>
#include <Swiften/Disco/GetDiscoItemsRequest.h>

#include <Swift/Controllers/DiscoServiceWalker.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/RequestUserSearchUIEvent.h>
#include <Swift/Controllers/UIInterfaces/UserSearchWindow.h>
#include <Swift/Controllers/UIInterfaces/UserSearchWindowFactory.h>

namespace Swift {
UserSearchController::UserSearchController(const JID& jid, UIEventStream* uiEventStream, UserSearchWindowFactory* factory, IQRouter* iqRouter) : jid_(jid) {
	iqRouter_ = iqRouter;
	uiEventStream_ = uiEventStream;
	uiEventConnection_ = uiEventStream_->onUIEvent.connect(boost::bind(&UserSearchController::handleUIEvent, this, _1));
	window_ = NULL;
	factory_ = factory;
	discoWalker_ = NULL;
}

UserSearchController::~UserSearchController() {
	delete window_;
	delete discoWalker_;
}

void UserSearchController::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<RequestUserSearchUIEvent> searchEvent = boost::dynamic_pointer_cast<RequestUserSearchUIEvent>(event);
	if (searchEvent) {
		if (!window_) {
			window_ = factory_->createUserSearchWindow(uiEventStream_);
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
	delete discoWalker_;
	discoWalker_ = new DiscoServiceWalker(service, iqRouter_);
	discoWalker_->onServiceFound.connect(boost::bind(&UserSearchController::handleDiscoServiceFound, this, _1, _2, discoWalker_));
	discoWalker_->onWalkComplete.connect(boost::bind(&UserSearchController::handleDiscoWalkFinished, this, discoWalker_));
	discoWalker_->beginWalk();
}

void UserSearchController::handleDiscoServiceFound(const JID& jid, boost::shared_ptr<DiscoInfo> info, DiscoServiceWalker* walker) {
	bool isUserDirectory = false;
	bool supports55 = false;
	foreach (DiscoInfo::Identity identity, info->getIdentities()) {
		if ((identity.getCategory() == "directory"
			&& identity.getType() == "user")) {
			isUserDirectory = true;
		}
	}
	std::vector<String> features = info->getFeatures();
	supports55 = std::find(features.begin(), features.end(), DiscoInfo::JabberSearchFeature) != features.end();
	if (/*isUserDirectory && */supports55) { //FIXME: once M-Link correctly advertises directoryness.
		/* Abort further searches.*/
		delete discoWalker_;
		discoWalker_ = NULL;
		boost::shared_ptr<GenericRequest<SearchPayload> > searchRequest(new GenericRequest<SearchPayload>(IQ::Get, jid, boost::shared_ptr<SearchPayload>(new SearchPayload()), iqRouter_));
		searchRequest->onResponse.connect(boost::bind(&UserSearchController::handleFormResponse, this, _1, _2, jid));
		searchRequest->send();
	}
}

void UserSearchController::handleFormResponse(boost::shared_ptr<SearchPayload> fields, ErrorPayload::ref error, const JID& jid) {
	if (error || !fields) {
		window_->setServerSupportsSearch(false);
		return;
	}
	window_->setSearchFields(fields);
}

void UserSearchController::handleSearch(boost::shared_ptr<SearchPayload> fields, const JID& jid) {
	boost::shared_ptr<GenericRequest<SearchPayload> > searchRequest(new GenericRequest<SearchPayload>(IQ::Set, jid, fields, iqRouter_));
	searchRequest->onResponse.connect(boost::bind(&UserSearchController::handleSearchResponse, this, _1, _2, jid));
	searchRequest->send();
}

void UserSearchController::handleSearchResponse(boost::shared_ptr<SearchPayload> resultsPayload, ErrorPayload::ref error, const JID& jid) {
	if (error || !resultsPayload) {
		window_->setSearchError(true);
		return;
	}
	std::vector<UserSearchResult> results;
	foreach (SearchPayload::Item item, resultsPayload->getItems()) {
		JID jid(item.jid);
		std::map<String, String> fields;
		fields["first"] = item.first;
		fields["last"] = item.last;
		fields["nick"] = item.nick;
		fields["email"] = item.email;
		UserSearchResult result(jid, fields);
		results.push_back(result);
	}
	window_->setResults(results);
}

void UserSearchController::handleDiscoWalkFinished(DiscoServiceWalker* walker) {
	window_->setServerSupportsSearch(false);
	delete discoWalker_;
	discoWalker_ = NULL;
}

}
