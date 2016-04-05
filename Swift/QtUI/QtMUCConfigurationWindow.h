/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include <Swiften/Elements/Form.h>

class QBoxLayout;
class QCloseEvent;

namespace Swift {
    class QtFormWidget;
    class QtMUCConfigurationWindow : public QWidget {
        Q_OBJECT
        public:
            QtMUCConfigurationWindow(Form::ref form);
            virtual ~QtMUCConfigurationWindow();
            boost::signals2::signal<void (Form::ref)> onFormComplete;
            boost::signals2::signal<void ()> onFormCancelled;
        private slots:
            void handleCancelClicked();
            void handleOKClicked();
        protected:
            virtual void closeEvent(QCloseEvent* event);
        private:
            QtFormWidget* formWidget_;
            QPushButton* okButton_;
            QPushButton* cancelButton_;
            bool closed_;
    };
}
