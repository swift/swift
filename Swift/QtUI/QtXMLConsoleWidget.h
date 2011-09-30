/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/XMLConsoleWidget.h"
#include "QtTabbable.h"

class QTextEdit;
class QCheckBox;
class QColor;

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

		private:
			virtual void closeEvent(QCloseEvent* event);
			virtual void showEvent(QShowEvent* event);

			void appendTextIfEnabled(const std::string& data, const QColor& color);

		private:
			QTextEdit* textEdit;
			QCheckBox* enabled;
	};
}
