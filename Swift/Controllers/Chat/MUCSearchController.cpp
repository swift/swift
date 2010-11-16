/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/MUCSearchController.h"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Disco/GetDiscoInfoRequest.h"
#include "Swiften/Disco/GetDiscoItemsRequest.h"

#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/RequestMUCSearchUIEvent.h"
#include "Swift/Controllers/UIInterfaces/MUCSearchWindow.h"
#include "Swift/Controllers/UIInterfaces/MUCSearchWindowFactory.h"

namespace Swift {

static const String SEARCHED_SERVICES = "searchedServices";

MUCSearchController::MUCSearchController(const JID& jid, UIEventStream* uiEventStream, MUCSearchWindowFactory* factory, IQRouter* iqRouter, SettingsProvider* settings) : jid_(jid) {
	iqRouter_ = iqRouter;
	settings_ = settings;
	uiEventStream_ = uiEventStream;
	uiEventConnection_ = uiEventStream_->onUIEvent.connect(boost::bind(&MUCSearchController::handleUIEvent, this, _1));
	window_ = NULL;
	factory_ = factory;
	loadServices();
}

MUCSearchController::~MUCSearchController() {
	delete window_;
}

void MUCSearchController::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<RequestMUCSearchUIEvent> searchEvent = boost::dynamic_pointer_cast<RequestMUCSearchUIEvent>(event);
	if (searchEvent) {
		if (!window_) {
			window_ = factory_->createMUCSearchWindow(uiEventStream_);
			window_->onAddService.connect(boost::bind(&MUCSearchController::handleAddService, this, _1, true));
			window_->addSavedServices(savedServices_);
			handleAddService(JID(jid_.getDomain()), true);
		}
		window_->setMUC("");
		window_->setNick(jid_.getNode());
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

void MUCSearchController::handleAddService(const JID& jid, bool userTriggered) {
	if (userTriggered) {
		addAndSaveServices(jid);
	}
	if (std::find(services_.begin(), services_.end(), jid) != services_.end()) {
		if (!userTriggered) {
			/* No infinite recursion. (Some buggy servers do infinitely deep disco of themselves)*/
			return;
		}
	} else if (userTriggered) {
		services_.push_back(jid);
		serviceDetails_[jid].setComplete(false);
		refreshView();
	}
	if (!jid.isValid()) {
		//Set Window to say error this isn't valid
		return;
	}
	GetDiscoInfoRequest::ref discoInfoRequest = GetDiscoInfoRequest::create(jid, iqRouter_);
	discoInfoRequest->onResponse.connect(boost::bind(&MUCSearchController::handleDiscoInfoResponse, this, _1, _2, jid));
	discoInfoRequest->send();
}

void MUCSearchController::removeService(const JID& jid) {
	serviceDetails_.erase(jid);
	services_.erase(std::remove(services_.begin(), services_.end(), jid), services_.end()); 
	refreshView();
}

void MUCSearchController::handleDiscoInfoResponse(boost::shared_ptr<DiscoInfo> info, const boost::optional<ErrorPayload>& error, const JID& jid) {
	if (error) {
		handleDiscoError(jid, error.get());
		return;
	}
	GetDiscoItemsRequest::ref discoItemsRequest = GetDiscoItemsRequest::create(jid, iqRouter_);
	bool mucService = false;
	bool couldContainServices = false;
	String name;
	foreach (DiscoInfo::Identity identity, info->getIdentities()) {
		if ((identity.getCategory() == "directory" 
			&& identity.getType() == "chatroom")
			|| (identity.getCategory() == "conference" 
			&& identity.getType() == "text")) {
			mucService = true;
			name = identity.getName();
		}
		if (identity.getCategory() == "server") {
			couldContainServices = true;
			name = identity.getName();
		}
	}
	services_.erase(std::remove(services_.begin(), services_.end(), jid), services_.end()); /* Bring it back to the end on a refresh */
	services_.push_back(jid);
	serviceDetails_[jid].setName(name);
	serviceDetails_[jid].setJID(jid);
	serviceDetails_[jid].setComplete(false);

	if (mucService) {
		discoItemsRequest->onResponse.connect(boost::bind(&MUCSearchController::handleRoomsItemsResponse, this, _1, _2, jid));
	} else if (couldContainServices) {
		discoItemsRequest->onResponse.connect(boost::bind(&MUCSearchController::handleServerItemsResponse, this, _1, _2, jid));
	} else {
		removeService(jid);
		return;
	}
	discoItemsRequest->send();
	refreshView();
}

void MUCSearchController::handleRoomsItemsResponse(boost::shared_ptr<DiscoItems> items, const boost::optional<ErrorPayload>& error, const JID& jid) {
	if (error) {
		handleDiscoError(jid, error.get());
		return;
	}
	serviceDetails_[jid].clearRooms();
	foreach (DiscoItems::Item item, items->getItems()) {
		serviceDetails_[jid].addRoom(MUCService::MUCRoom(item.getJID().getNode(), item.getName(), -1));
	}
	serviceDetails_[jid].setComplete(true);
	refreshView();
}

void MUCSearchController::handleServerItemsResponse(boost::shared_ptr<DiscoItems> items, const boost::optional<ErrorPayload>& error, const JID& jid) {
	if (error) {
		handleDiscoError(jid, error.get());
		return;
	}
	if (jid.isValid()) {
		removeService(jid);
	}
	foreach (DiscoItems::Item item, items->getItems()) {
		if (item.getNode().isEmpty()) {
			/* Don't look at noded items. It's possible that this will exclude some services,
			 * but I've never seen one in the wild, and it's an easy fix for not looping.
			 */
			handleAddService(item.getJID());
		}
	}
	refreshView();
}

void MUCSearchController::handleDiscoError(const JID& jid, const ErrorPayload& error) {
	serviceDetails_[jid].setComplete(true);
	serviceDetails_[jid].setError(error.getText());
}

void MUCSearchController::refreshView() {
	window_->clearList();
	foreach (JID jid, services_) {
		window_->addService(serviceDetails_[jid]);
	}
}

}
