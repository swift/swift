/*
 * Copyright (c) 2010-2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QDialog>
#include <QLineEdit>

#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>

class QBoxLayout;

namespace Swift {
    class UIEventStream;
    class QtFormWidget;
    class QtAdHocCommandWithJIDWindow : public QDialog {
        Q_OBJECT
        public:
            QtAdHocCommandWithJIDWindow(UIEventStream* eventStream);
            virtual ~QtAdHocCommandWithJIDWindow();
        public slots:
            void handleAcceptClick();
            void handleRejectClick();
        private:
            UIEventStream* uiEventStream_;
            QLineEdit* jid_;
            QLineEdit* node_;
    };
}
