/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <mutex>

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
                std::unique_lock<std::recursive_mutex> lock(isEventInQtEventLoopMutex_);
                if (!isEventInQtEventLoop_) {
                    isEventInQtEventLoop_ = true;
                    QCoreApplication::postEvent(this, new Event());
                }
            }

            virtual bool event(QEvent* qevent) {
                Event* event = dynamic_cast<Event*>(qevent);
                if (event) {
                    {
                        std::unique_lock<std::recursive_mutex> lock(isEventInQtEventLoopMutex_);
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
            std::recursive_mutex isEventInQtEventLoopMutex_;
    };
}
