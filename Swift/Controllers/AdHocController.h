/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>

namespace Swift {

class AdHocCommandWindowFactory;
class AdHocCommandWindow;

class AdHocController {
public:
    AdHocController(AdHocCommandWindowFactory* factory, boost::shared_ptr<OutgoingAdHocCommandSession> command);
    ~AdHocController();
    boost::signal<void ()> onDeleting;
    void setOnline(bool online);
private:
    void handleWindowClosed();
    AdHocCommandWindow* window_;
};

}
