/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QDialog>

#include <Swift/Controllers/XMPPEvents/SubscriptionRequestEvent.h>

namespace Swift {
    class QtSubscriptionRequestWindow : public QDialog {
        Q_OBJECT
        public:
            static QtSubscriptionRequestWindow* getWindow(std::shared_ptr<SubscriptionRequestEvent> event, QWidget* parent = nullptr);
            ~QtSubscriptionRequestWindow();
            std::shared_ptr<SubscriptionRequestEvent> getEvent();
        private slots:
            void handleYes();
            void handleNo();
            void handleDefer();
        private:
            QtSubscriptionRequestWindow(std::shared_ptr<SubscriptionRequestEvent> event, QWidget* parent = nullptr);
            static QList<QtSubscriptionRequestWindow*> windows_;
            std::shared_ptr<SubscriptionRequestEvent> event_;
            /*QPushButton* yesButton_;
            QPushButton* noButton_;
            QPushButton* deferButton_;*/
    };
}
