/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <QtGui>

#include <EventModel.h>
#include <EventView.h>

#include <Swiften/Events/ErrorEvent.h>
#include <Swiften/Events/MessageEvent.h>
#include <Swiften/JID/JID.h>

#include <Swift/QtUI/EventViewer/QtEventWindow.h>

int main(int argc, char *argv[])
{
        QApplication app(argc, argv);
        Swift::UIEventStream eventStream;
        Swift::QtEventWindow* viewer = new Swift::QtEventWindow(&eventStream);
        viewer->show();
        std::shared_ptr<Swift::Message> message1(new Swift::Message());
        message1->setBody("Oooh, shiny");
        std::shared_ptr<Swift::MessageEvent> event1(new Swift::MessageEvent(message1));
        viewer->addEvent(std::dynamic_pointer_cast<Swift::StanzaEvent>(event1), true);
        for (int i = 0; i < 100; i++) {
            viewer->addEvent(std::dynamic_pointer_cast<Swift::StanzaEvent>(event1), false);
        }
        viewer->addEvent(std::dynamic_pointer_cast<Swift::StanzaEvent>(std::make_shared<Swift::ErrorEvent>(Swift::JID("me@example.com"), "Something bad did happen to you.")), true);
        return app.exec();
}
