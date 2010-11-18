/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/MUCSearchController.h"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Disco/GetDiscoInfoRequest.h>
#include <Swiften/Disco/GetDiscoItemsRequest.h>

#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/RequestMUCSearchUIEvent.h>
#include <Swift/Controllers/UIInterfaces/MUCSearchWindow.h>
#include <Swift/Controllers/UIInterfaces/MUCSearchWindowFactory.h>
#include <Swift/Controllers/DiscoServiceWalker.h>
#include <Swiften/Client/NickResolver.h>

namespace Swift {

static const String SEARCHED_SERVICES = "searchedServices";

MUCSearchController::MUCSearchController(const JID& jid, UIEventStream* uiEventStream, MUCSearchWindowFactory* factory, IQRouter* iqRouter, SettingsProvider* settings, NickResolver *nickResolver) : jid_(jid) {
	iqRouter_ = iqRouter;
	settings_ = settings;
	uiEventStream_ = uiEventStream;
	nickResolver_ = nickResolver;
	itemsInProgress_ = 0;
	uiEventConnection_ = uiEventStream_->onUIEvent.connect(boost::bind(&MUCSearchController::handleUIEvent, this, _1));
	window_ = NULL;
	factory_ = factory;
	loadServices();
}

MUCSearchController::~MUCSearchController() {
	foreach (DiscoServiceWalker* walker, walksInProgress_) {
		delete walker;
	}
	delete window_;
}

void MUCSearchController::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<RequestMUCSearchUIEvent> searchEvent = boost::dynamic_pointer_cast<RequestMUCSearchUIEvent>(event);
	if (searchEvent) {
		if (!window_) {
			window_ = factory_->createMUCSearchWindow(uiEventStream_);
			window_->onAddService.connect(boost::bind(&MUCSearchController::handleAddService, this, _1));
			window_->addSavedServices(savedServices_);
			handleAddService(JID(jid_.getDomain()));
		}
		window_->setMUC("");
		window_->setNick(nickResolver_->jidToNick(jid_));
		window_->show();
		return;
	}
}

void MUCSearchController::loadServices() {
	savedServices_.clear();
	foreach (String stringItem, settings_->getStringSetting(SEARCHED_SERVICES).split('\n')) {
		savedServices_.push_back(JID(stringItem));
	}
}

void MUCSearchController::addAndSaveServices(const JID& jid) {
	savedServices_.erase(std::remove(savedServices_.begin(), savedServices_.end(), jid), savedServices_.end());
	savedServices_.push_back(jid);
	String collapsed;
	bool storeThis = savedServices_.size() < 15;
	foreach (JID jidItem, savedServices_) {
		if (!storeThis) {
			storeThis = true;
			continue;
		}
		if (!collapsed.isEmpty()) {
			collapsed += "\n";
		}
		collapsed += jidItem.toString();
	}
	settings_->storeString(SEARCHED_SERVICES, collapsed);
	window_->addSavedServices(savedServices_);
}

void MUCSearchController::handleAddService(const JID& jid) {
	if (!jid.isValid()) {
		//Set Window to say error this isn't valid
		return;
	}
	addAndSaveServices(jid);
	services_.push_back(jid);
	serviceDetails_[jid].setComplete(false);
	window_->setSearchInProgress(true);
	refreshView();
	DiscoServiceWalker* walker = new DiscoServiceWalker(jid, iqRouter_);
	walker->onServiceFound.connect(boost::bind(&MUCSearchController::handleDiscoServiceFound, this, _1, _2));
	walker->onWalkComplete.connect(boost::bind(&MUCSearchController::handleDiscoWalkFinished, this, walker));
	walksInProgress_.push_back(walker);
	walker->beginWalk();
}

void MUCSearchController::handleDiscoServiceFound(const JID& jid, boost::shared_ptr<DiscoInfo> info) {
	bool isMUC;
	String name;
	foreach (DiscoInfo::Identity identity, info->getIdentities()) {
			if ((identity.getCategory() == "directory"
				&& identity.getType() == "chatroom")
				|| (identity.getCategory() == "conference"
				&& identity.getType() == "text")) {
				isMUC = true;
				name = identity.getName();
			}
	}
	if (isMUC) {
		services_.erase(std::remove(services_.begin(), services_.end(), jid), services_.end()); /* Bring it back to the end on a refresh */
		services_.push_back(jid);
		serviceDetails_[jid].setName(name);
		serviceDetails_[jid].setJID(jid);
		serviceDetails_[jid].setComplete(false);
		itemsInProgress_++;
		GetDiscoItemsRequest::ref discoItemsRequest = GetDiscoItemsRequest::create(jid, iqRouter_);
		discoItemsRequest->onResponse.connect(boost::bind(&MUCSearchController::handleRoomsItemsResponse, this, _1, _2, jid));
		discoItemsRequest->send();
	} else {
		removeService(jid);
	}
	refreshView();
}

void MUCSearchController::handleDiscoWalkFinished(DiscoServiceWalker* walker) {
	walksInProgress_.erase(std::remove(walksInProgress_.begin(), walksInProgress_.end(), walker), walksInProgress_.end());
	updateInProgressness();
	delete walker;
}

void MUCSearchController::removeService(const JID& jid) {
	serviceDetails_.erase(jid);
	services_.erase(std::remove(services_.begin(), services_.end(), jid), services_.end()); 
	refreshView();
}

void MUCSearchController::handleRoomsItemsResponse(boost::shared_ptr<DiscoItems> items, ErrorPayload::ref error, const JID& jid) {
	itemsInProgress_--;
	updateInProgressness();
	if (error) {
		handleDiscoError(jid, error);
		return;
	}
	serviceDetails_[jid].clearRooms();
	foreach (DiscoItems::Item item, items->getItems()) {
		serviceDetails_[jid].addRoom(MUCService::MUCRoom(item.getJID().getNode(), item.getName(), -1));
	}
	serviceDetails_[jid].setComplete(true);
	refreshView();
}

void MUCSearchController::handleDiscoError(const JID& jid, ErrorPayload::ref error) {
	serviceDetails_[jid].setComplete(true);
	serviceDetails_[jid].setError(error->getText());
}

void MUCSearchController::refreshView() {
	window_->clearList();
	foreach (JID jid, services_) {
		window_->addService(serviceDetails_[jid]);
	}
}

void MUCSearchController::updateInProgressness() {
	window_->setSearchInProgress(walksInProgress_.size() + itemsInProgress_ > 0);
}

}
