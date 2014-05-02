/*
 * Copyright (c) 2010-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
