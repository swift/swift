/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QObject>
#include <QEvent>
#include <QCoreApplication>

#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class QtEventLoop : public QObject, public EventLoop {
		public:
			QtEventLoop() {}
			virtual ~QtEventLoop() {
				QCoreApplication::removePostedEvents(this);
			}

			virtual void post(const Swift::Event& event) {
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
					Event(const Swift::Event& event) :
							QEvent(QEvent::User), event_(event) {
					}

					Swift::Event event_;
			};
	};
}
