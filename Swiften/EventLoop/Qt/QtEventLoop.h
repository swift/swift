/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/thread.hpp>

#include <QCoreApplication>
#include <QEvent>
#include <QObject>

#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class QtEventLoop : public QObject, public EventLoop {
		public:
			QtEventLoop() : isEventInQtEventLoop_(false) {}
			virtual ~QtEventLoop() {
				QCoreApplication::removePostedEvents(this);
			}

		protected:
			virtual void eventPosted() {
				boost::recursive_mutex::scoped_lock lock(isEventInQtEventLoopMutex_);	
				if (!isEventInQtEventLoop_) {
					isEventInQtEventLoop_ = true;
					QCoreApplication::postEvent(this, new Event());
				}
			}

			virtual bool event(QEvent* qevent) {
				Event* event = dynamic_cast<Event*>(qevent);
				if (event) {
					{
						boost::recursive_mutex::scoped_lock lock(isEventInQtEventLoopMutex_);	
						isEventInQtEventLoop_ = false;
					}
					handleNextEvents();
					//event->deleteLater(); FIXME: Leak?
					return true;
				}

				return false;
			}
		
		private:
			struct Event : public QEvent {
					Event() :
							QEvent(QEvent::User) {
					}
			};

			bool isEventInQtEventLoop_;
			boost::recursive_mutex isEventInQtEventLoopMutex_;
	};
}
