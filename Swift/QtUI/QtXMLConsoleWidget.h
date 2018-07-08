/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/XMLConsoleWidget.h>

#include <Swift/QtUI/QtTabbable.h>

class QTextEdit;
class QCheckBox;
class QColor;
class QPushButton;


namespace Swift {
    class QtXMLConsoleWidget : public QtTabbable, public XMLConsoleWidget {
            Q_OBJECT

        public:
            QtXMLConsoleWidget();
            ~QtXMLConsoleWidget();

            void show();
            void activate();

            virtual void handleDataRead(const SafeByteArray& data);
            virtual void handleDataWritten(const SafeByteArray& data);

            virtual std::string getID() const;

        public slots:
            virtual void sendXML();

        private:
            virtual void closeEvent(QCloseEvent* event);
            virtual void showEvent(QShowEvent* event);

            void appendTextIfEnabled(const std::string& data, const QColor& color);

        private:
            QTextEdit* textEdit = nullptr;
            QCheckBox* enabled = nullptr;
            QCheckBox* debugEnabled = nullptr;
            QTextEdit* xmlWindow = nullptr;
            QPushButton* sendXMLButton = nullptr;
    };
}
