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
	public:
		QtXMLSenderWidget();
		~QtXMLSenderWidget();
	public slots:
		std::string readXML();
	private:
		QTextEdit* textEdit;
	};
}