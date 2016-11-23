/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/AdHocManager.h>

#include <memory>

#include <boost/bind.hpp>

#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>
#include <Swiften/Queries/IQRouter.h>

#include <Swift/Controllers/UIEvents/RequestAdHocUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestAdHocWithJIDUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/AdHocCommandWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/MainWindow.h>

namespace Swift {

AdHocManager::AdHocManager(const JID& jid, AdHocCommandWindowFactory* factory, IQRouter* iqRouter, UIEventStream* uiEventStream, MainWindow* mainWindow) : jid_(jid) {
    iqRouter_ = iqRouter;
    uiEventStream_ = uiEventStream;
    mainWindow_ = mainWindow;
    factory_ = factory;

    uiEventStream_->onUIEvent.connect(boost::bind(&AdHocManager::handleUIEvent, this, _1));
}

AdHocManager::~AdHocManager() {
    uiEventStream_->onUIEvent.disconnect(boost::bind(&AdHocManager::handleUIEvent, this, _1));
    for (auto& controller : controllers_) {
        controller->onDeleting.disconnect(boost::bind(&AdHocManager::removeController, this, controller));
    }
}

void AdHocManager::removeController(std::shared_ptr<AdHocController> controller) {
    controller->onDeleting.disconnect(boost::bind(&AdHocManager::removeController, this, controller));
    controllers_.erase(std::find(controllers_.begin(), controllers_.end(), controller));
}

void AdHocManager::setServerDiscoInfo(std::shared_ptr<DiscoInfo> info) {
    if (iqRouter_->isAvailable() && info->hasFeature(DiscoInfo::CommandsFeature)) {
        if (discoItemsRequest_) {
            discoItemsRequest_->onResponse.disconnect(boost::bind(&AdHocManager::handleServerDiscoItemsResponse, this, _1, _2));
            discoItemsRequest_.reset();
        }
        discoItemsRequest_ = GetDiscoItemsRequest::create(JID(jid_.getDomain()), DiscoInfo::CommandsFeature, iqRouter_);
        discoItemsRequest_->onResponse.connect(boost::bind(&AdHocManager::handleServerDiscoItemsResponse, this, _1, _2));
        discoItemsRequest_->send();
    } else {
        mainWindow_->setAvailableAdHocCommands(std::vector<DiscoItems::Item>());
    }
}

void AdHocManager::setOnline(bool online) {
    for (auto&& controller : controllers_) {
        controller->setOnline(online);
    }
}

void AdHocManager::handleServerDiscoItemsResponse(std::shared_ptr<DiscoItems> items, ErrorPayload::ref error) {
    std::vector<DiscoItems::Item> commands;
    if (!error) {
        for (const auto& item : items->getItems()) {
            if (item.getNode() != "http://isode.com/xmpp/commands#test") {
                commands.push_back(item);
            }
        }
    }
    mainWindow_->setAvailableAdHocCommands(commands);
}

void AdHocManager::handleUIEvent(std::shared_ptr<UIEvent> event) {
    std::shared_ptr<RequestAdHocUIEvent> adHocEvent = std::dynamic_pointer_cast<RequestAdHocUIEvent>(event);
    if (adHocEvent) {
        std::shared_ptr<OutgoingAdHocCommandSession> command = std::make_shared<OutgoingAdHocCommandSession>(adHocEvent->getCommand().getJID(), adHocEvent->getCommand().getNode(), iqRouter_);
        std::shared_ptr<AdHocController> controller = std::make_shared<AdHocController>(factory_, command);
        controller->onDeleting.connect(boost::bind(&AdHocManager::removeController, this, controller));
        controllers_.push_back(controller);
    }
    std::shared_ptr<RequestAdHocWithJIDUIEvent> adHocJIDEvent = std::dynamic_pointer_cast<RequestAdHocWithJIDUIEvent>(event);
    if (!!adHocJIDEvent) {
        std::shared_ptr<OutgoingAdHocCommandSession> command = std::make_shared<OutgoingAdHocCommandSession>(adHocJIDEvent->getJID(), adHocJIDEvent->getNode(), iqRouter_);
        std::shared_ptr<AdHocController> controller = std::make_shared<AdHocController>(factory_, command);
        controller->onDeleting.connect(boost::bind(&AdHocManager::removeController, this, controller));
        controllers_.push_back(controller);
    }
}

}
