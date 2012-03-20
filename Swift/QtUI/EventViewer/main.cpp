/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QtGui>
#include "EventView.h"
#include "EventModel.h"
#include "QtEventWindow.h"

#include "Swiften/Events/MessageEvent.h"
#include "Swiften/Events/ErrorEvent.h"
#include "Swiften/JID/JID.h"

int main(int argc, char *argv[])
{
		QApplication app(argc, argv);
		Swift::UIEventStream eventStream;
		Swift::QtEventWindow* viewer = new Swift::QtEventWindow(&eventStream);
		viewer->show();
		boost::shared_ptr<Swift::Message> message1(new Swift::Message());
		message1->setBody("Oooh, shiny");
		boost::shared_ptr<Swift::MessageEvent> event1(new Swift::MessageEvent(message1));
		viewer->addEvent(boost::dynamic_pointer_cast<Swift::StanzaEvent>(event1), true);
		for (int i = 0; i < 100; i++) {
			viewer->addEvent(boost::dynamic_pointer_cast<Swift::StanzaEvent>(event1), false);
		}
		viewer->addEvent(boost::dynamic_pointer_cast<Swift::StanzaEvent>(boost::make_shared<Swift::ErrorEvent>(Swift::JID("me@example.com"), "Something bad did happen to you.")), true);
		return app.exec();
}
