/*
 * Copyright (c) 2017 Barun Parruck.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/QtUI/QtTabbable.h>
#include <QTextEdit>
#include <QVBoxLayout>

namespace Swift {
	class QtXMLSenderWidget : public QWidget {
		Q_OBJECT

	public:
		QtXMLSenderWidget();
		~QtXMLSenderWidget();
		std::string readXML();
	public slots:
		virtual void sendXML();
	signals:
		void xmlSent(std::string data);
	private:
		QTextEdit* textEdit;
	};
}