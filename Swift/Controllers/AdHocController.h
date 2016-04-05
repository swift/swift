/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>

namespace Swift {

class AdHocCommandWindowFactory;
class AdHocCommandWindow;

class AdHocController {
public:
    AdHocController(AdHocCommandWindowFactory* factory, std::shared_ptr<OutgoingAdHocCommandSession> command);
    ~AdHocController();
    boost::signals2::signal<void ()> onDeleting;
    void setOnline(bool online);
private:
    void handleWindowClosed();
    AdHocCommandWindow* window_;
};

}
