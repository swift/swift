/*
 * Copyright (c) 2010-2016 Isode Limited.
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
	private:
		QTextEdit* textEdit;
	};
}