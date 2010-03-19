#include <QtGui>
#include "EventView.h"
#include "EventModel.h"
#include "QtEventWindow.h"

#include "Swiften/Events/MessageEvent.h"

int main(int argc, char *argv[])
{
		QApplication app(argc, argv);
		Swift::QtEventWindow* viewer = new Swift::QtEventWindow();
		viewer->show();
		boost::shared_ptr<Swift::Message> message1(new Swift::Message());
		message1->setBody("Oooh, shiny");
		boost::shared_ptr<Swift::MessageEvent> event1(new Swift::MessageEvent(message1));
		viewer->addEvent(boost::dynamic_pointer_cast<Swift::Event>(event1), true);
		for (int i = 0; i < 100; i++) {
			viewer->addEvent(boost::dynamic_pointer_cast<Swift::Event>(event1), false);
		}
		return app.exec();
}
