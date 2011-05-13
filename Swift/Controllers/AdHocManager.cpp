/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/AdHocManager.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>
#include <Swift/Controllers/UIInterfaces/MainWindow.h>
#include <Swift/Controllers/UIInterfaces/AdHocCommandWindowFactory.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/RequestAdHocUIEvent.h>

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
		factory_->createAdHocCommandWindow(boost::make_shared<OutgoingAdHocCommandSession>(adHocEvent->getCommand().getJID(), adHocEvent->getCommand().getNode(), iqRouter_));
	}
}

}
