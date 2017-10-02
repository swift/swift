/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <QWidget>

namespace Swift {
    class QtTabbable : public QWidget {
        Q_OBJECT
        public:
            enum AlertType {NoActivity, WaitingActivity, ImpendingActivity};
            virtual ~QtTabbable();

            bool isWidgetSelected();
            virtual AlertType getWidgetAlertState() {return NoActivity;}
            virtual int getCount() {return 0;}
            virtual std::string getID() const = 0;
            virtual void setEmphasiseFocus(bool /*emphasise*/) {}

        protected:
            QtTabbable();
            virtual bool event(QEvent* event);
            virtual void closeEvent(QCloseEvent* event);

        signals:
            void titleUpdated();
            void countUpdated();
            void windowClosing();
            void windowOpening();
            void wantsToActivate();
            void requestPreviousTab();
            void requestNextTab();
            void requestActiveTab();
            void requestFlash();
    };
}
