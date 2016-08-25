/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Chat/MUCSearchController.h>

#include <iostream>
#include <memory>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Disco/DiscoServiceWalker.h>
#include <Swiften/Disco/GetDiscoItemsRequest.h>

#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/MUCSearchWindowFactory.h>

namespace Swift {

static const std::string SEARCHED_SERVICES = "searchedServices";

MUCSearchController::MUCSearchController(const JID& jid, MUCSearchWindowFactory* factory, IQRouter* iqRouter, ProfileSettingsProvider* settings) : jid_(jid), factory_(factory), iqRouter_(iqRouter), settings_(settings), window_(nullptr), walker_(nullptr) {
    itemsInProgress_ = 0;
    loadSavedServices();
}

MUCSearchController::~MUCSearchController() {
    delete walker_;
    delete window_;
}

void MUCSearchController::openSearchWindow() {
    if (!window_) {
        window_ = factory_->createMUCSearchWindow();
        window_->onSearchService.connect(boost::bind(&MUCSearchController::handleSearchService, this, _1));
        window_->onFinished.connect(boost::bind(&MUCSearchController::handleMUCSearchFinished, this, _1));
        window_->addSavedServices(savedServices_);
    }
    handleSearchService(JID(jid_.getDomain()));
    window_->show();
}

void MUCSearchController::loadSavedServices() {
    savedServices_.clear();
    for (auto&& stringItem : String::split(settings_->getStringSetting(SEARCHED_SERVICES), '\n')) {
        savedServices_.push_back(JID(stringItem));
    }
}

void MUCSearchController::addToSavedServices(const JID& jid) {
    savedServices_.erase(std::remove(savedServices_.begin(), savedServices_.end(), jid), savedServices_.end());
    savedServices_.push_front(jid);

    std::string collapsed;
    int i = 0;
    for (auto&& jidItem : savedServices_) {
        if (i >= 15) {
            break;
        }
        if (!collapsed.empty()) {
            collapsed += "\n";
        }
        collapsed += jidItem.toString();
        ++i;
    }
    settings_->storeString(SEARCHED_SERVICES, collapsed);
    window_->addSavedServices(savedServices_);
}

void MUCSearchController::handleSearchService(const JID& jid) {
    if (!jid.isValid()) {
        //Set Window to say error this isn't valid
        return;
    }
    addToSavedServices(jid);

    services_.clear();
    serviceDetails_.clear();

    window_->setSearchInProgress(true);
    refreshView();

    if (walker_) {
        walker_->endWalk();
        walker_->onServiceFound.disconnect(boost::bind(&MUCSearchController::handleDiscoServiceFound, this, _1, _2));
        walker_->onWalkComplete.disconnect(boost::bind(&MUCSearchController::handleDiscoWalkFinished, this));
        delete walker_;
    }

    SWIFT_LOG(debug) << "Starting walking MUC services" << std::endl;
    itemsInProgress_ = 0;
    walker_ = new DiscoServiceWalker(jid, iqRouter_);
    walker_->onServiceFound.connect(boost::bind(&MUCSearchController::handleDiscoServiceFound, this, _1, _2));
    walker_->onWalkComplete.connect(boost::bind(&MUCSearchController::handleDiscoWalkFinished, this));
    walker_->beginWalk();
}

void MUCSearchController::handleDiscoServiceFound(const JID& jid, std::shared_ptr<DiscoInfo> info) {
    bool isMUC = false;
    std::string name;
    for (auto&& identity : info->getIdentities()) {
            if ((identity.getCategory() == "directory"
                && identity.getType() == "chatroom")
                || (identity.getCategory() == "conference"
                && identity.getType() == "text")) {
                isMUC = true;
                name = identity.getName();
            }
    }
    if (isMUC) {
        SWIFT_LOG(debug) << "MUC Service found: " << jid << std::endl;
        services_.erase(std::remove(services_.begin(), services_.end(), jid), services_.end());
        services_.push_back(jid);
        serviceDetails_[jid].setName(name);
        serviceDetails_[jid].setJID(jid);
        serviceDetails_[jid].setComplete(false);
        itemsInProgress_++;
        SWIFT_LOG(debug) << "Requesting items of " << jid << " (" << itemsInProgress_ << " item requests in progress)" << std::endl;
        GetDiscoItemsRequest::ref discoItemsRequest = GetDiscoItemsRequest::create(jid, iqRouter_);
        discoItemsRequest->onResponse.connect(boost::bind(&MUCSearchController::handleRoomsItemsResponse, this, _1, _2, jid));
        discoItemsRequest->send();
    }
    else {
        removeService(jid);
    }
    refreshView();
}

void MUCSearchController::handleDiscoWalkFinished() {
    SWIFT_LOG(debug) << "MUC Walk finished" << std::endl;
    updateInProgressness();
}

void MUCSearchController::removeService(const JID& jid) {
    serviceDetails_.erase(jid);
    services_.erase(std::remove(services_.begin(), services_.end(), jid), services_.end());
    refreshView();
}

void MUCSearchController::handleRoomsItemsResponse(std::shared_ptr<DiscoItems> items, ErrorPayload::ref error, const JID& jid) {
    itemsInProgress_--;
    SWIFT_LOG(debug) << "Items received for " << jid << " (" << itemsInProgress_ << " item requests in progress)" << std::endl;
    updateInProgressness();
    if (error) {
        handleDiscoError(jid, error);
        return;
    }
    serviceDetails_[jid].clearRooms();
    for (auto&& item : items->getItems()) {
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
    for (auto&& jid : services_) {
        window_->addService(serviceDetails_[jid]);
    }
}

void MUCSearchController::updateInProgressness() {
    window_->setSearchInProgress((walker_ && walker_->isActive()) || itemsInProgress_ > 0);
}

void MUCSearchController::handleMUCSearchFinished(const boost::optional<JID>& result) {
    if (result) {
        onMUCSelected(*result);
    }
}

}
