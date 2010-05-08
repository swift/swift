/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/MUCSearchController.h"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Queries/Requests/GetDiscoInfoRequest.h"
#include "Swiften/Queries/Requests/GetDiscoItemsRequest.h"

#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/RequestMUCSearchUIEvent.h"
#include "Swift/Controllers/UIInterfaces/MUCSearchWindow.h"
#include "Swift/Controllers/UIInterfaces/MUCSearchWindowFactory.h"

namespace Swift {

MUCSearchController::MUCSearchController(const JID& jid, UIEventStream* uiEventStream, MUCSearchWindowFactory* factory, IQRouter* iqRouter) : jid_(jid) {
	iqRouter_ = iqRouter;
	uiEventStream_ = uiEventStream;
	uiEventConnection_ = uiEventStream_->onUIEvent.connect(boost::bind(&MUCSearchController::handleUIEvent, this, _1));
	window_ = NULL;
	factory_ = factory;
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
			handleAddService(JID(jid_.getDomain()), true);
		}
		window_->setMUC("");
		window_->setNick(jid_.getNode());
		window_->show();
		return;
	}
}

void MUCSearchController::handleAddService(const JID& jid, bool userTriggered) {
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
	boost::shared_ptr<GetDiscoInfoRequest> discoInfoRequest(new GetDiscoInfoRequest(jid, iqRouter_));
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
	boost::shared_ptr<GetDiscoItemsRequest> discoItemsRequest(new GetDiscoItemsRequest(jid, iqRouter_));
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
		handleAddService(item.getJID());
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
