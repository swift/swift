#ifndef SWIFT_QtMainEventLoop_H
#define SWIFT_QtMainEventLoop_H

#include <QObject>
#include <QEvent>
#include <QCoreApplication>

#include "Swiften/EventLoop/EventLoop.h"

class QtMainEventLoop : public QObject, public Swift::EventLoop
{
	public:
		QtMainEventLoop() {}

		virtual void post(const Event& event) {
			QCoreApplication::postEvent(this, new Event(event));
		}

		virtual bool event(QEvent* qevent) {
			Event* event = dynamic_cast<Event*>(qevent);
			if (event) {
        handleEvent(event->event_);
				//event->deleteLater(); FIXME: Leak?
				return true;
			}

			return false;
		}
	
	private:
		struct Event : public QEvent {
				Event(const EventLoop::Event& event) :
						QEvent(QEvent::User), event_(event) {
				}

				EventLoop::Event event_;
		};
};

#endif
