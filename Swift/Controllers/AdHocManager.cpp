/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/AdHocManager.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>
#include <Swiften/Base/foreach.h>
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
    for (size_t i = 0; i < controllers_.size(); ++i) {
        controllers_[i]->onDeleting.disconnect(boost::bind(&AdHocManager::removeController, this, controllers_[i]));
    }
}

void AdHocManager::removeController(boost::shared_ptr<AdHocController> controller) {
    controller->onDeleting.disconnect(boost::bind(&AdHocManager::removeController, this, controller));
    controllers_.erase(std::find(controllers_.begin(), controllers_.end(), controller));
}

void AdHocManager::setServerDiscoInfo(boost::shared_ptr<DiscoInfo> info) {
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
    foreach (boost::shared_ptr<AdHocController> controller, controllers_) {
        controller->setOnline(online);
    }
}

void AdHocManager::handleServerDiscoItemsResponse(boost::shared_ptr<DiscoItems> items, ErrorPayload::ref error) {
    std::vector<DiscoItems::Item> commands;
    if (!error) {
        foreach (DiscoItems::Item item, items->getItems()) {
            if (item.getNode() != "http://isode.com/xmpp/commands#test") {
                commands.push_back(item);
            }
        }
    }
    mainWindow_->setAvailableAdHocCommands(commands);
}

void AdHocManager::handleUIEvent(boost::shared_ptr<UIEvent> event) {
    boost::shared_ptr<RequestAdHocUIEvent> adHocEvent = boost::dynamic_pointer_cast<RequestAdHocUIEvent>(event);
    if (adHocEvent) {
        boost::shared_ptr<OutgoingAdHocCommandSession> command = boost::make_shared<OutgoingAdHocCommandSession>(adHocEvent->getCommand().getJID(), adHocEvent->getCommand().getNode(), iqRouter_);
        boost::shared_ptr<AdHocController> controller = boost::make_shared<AdHocController>(factory_, command);
        controller->onDeleting.connect(boost::bind(&AdHocManager::removeController, this, controller));
        controllers_.push_back(controller);
    }
    boost::shared_ptr<RequestAdHocWithJIDUIEvent> adHocJIDEvent = boost::dynamic_pointer_cast<RequestAdHocWithJIDUIEvent>(event);
    if (!!adHocJIDEvent) {
        boost::shared_ptr<OutgoingAdHocCommandSession> command = boost::make_shared<OutgoingAdHocCommandSession>(adHocJIDEvent->getJID(), adHocJIDEvent->getNode(), iqRouter_);
        boost::shared_ptr<AdHocController> controller = boost::make_shared<AdHocController>(factory_, command);
        controller->onDeleting.connect(boost::bind(&AdHocManager::removeController, this, controller));
        controllers_.push_back(controller);
    }
}

}
