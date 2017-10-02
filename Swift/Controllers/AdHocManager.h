/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/Disco/GetDiscoItemsRequest.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/JID/JID.h>

#include <Swift/Controllers/AdHocController.h>
#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
class IQRouter;
class MainWindow;
class UIEventStream;
class AdHocCommandWindowFactory;

class AdHocManager {
public:
    AdHocManager(const JID& jid, AdHocCommandWindowFactory* factory, IQRouter* iqRouter, UIEventStream* uiEventStream, MainWindow* mainWindow);
    ~AdHocManager();
    void removeController(std::shared_ptr<AdHocController> contoller);
    void setServerDiscoInfo(std::shared_ptr<DiscoInfo> info);
    void setOnline(bool online);

private:
    void handleServerDiscoItemsResponse(std::shared_ptr<DiscoItems>, ErrorPayload::ref error);
    void handleUIEvent(std::shared_ptr<UIEvent> event);
    boost::signals2::signal<void (const AdHocController&)> onControllerComplete;

private:
    JID jid_;
    IQRouter* iqRouter_;
    UIEventStream* uiEventStream_;
    MainWindow* mainWindow_;
    AdHocCommandWindowFactory* factory_;
    GetDiscoItemsRequest::ref discoItemsRequest_;
    std::vector<std::shared_ptr<AdHocController> > controllers_;
};

}
