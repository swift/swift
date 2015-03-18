/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once
#include "Swift/Controllers/XMLConsoleController.h"
#include "Swift/Controllers/UIInterfaces/XMLConsoleWidget.h"
#include "QtTabbable.h"

class QTextEdit;
class QCheckBox;
class QComboBox;
class QSignalMapper;
class QVBoxLayout;
class QHBoxLayout;
class QLineEdit;
class QPushButton;
class QLabel;
namespace Swift {
	class QtXMLMsgSender : public QtTabbable,public XMLConsoleController,public XMLConsoleWidget {
			Q_OBJECT

		public:
			QtXMLMsgSender();
			~QtXMLMsgSender();

			void show();
			void activate();
			void showEvent(QShowEvent *event);
			void closeEvent(QCloseEvent *event);
			virtual void handleDataRead(const SafeByteArray& data);
			virtual void handleDataWritten(const SafeByteArray& data);
			virtual std::string getID() const;

		private slots:
			void sendXMLMessage(const int &text);
			void attributesSelector(const int &);
			void sendRawIqStanza();
			void sendRawPresenceStanza();
			void sendRawMessageStanza();
		private:
			QVBoxLayout* layout;
			QTextEdit* textEdit;
			QComboBox* comboBox;
			QSignalMapper* signalMapper;
			QWidget* bottom;
			QHBoxLayout* buttonLayout;
			QLineEdit* sendToAddress;
			QPushButton* sendButton;
			QVBoxLayout* customAttributes;
			QComboBox* typeChooser;
			QLabel* textLabel;
	};
}
