#pragma once

#include "Swift/Controllers/UIInterfaces/XMLConsoleWidget.h"
#include "QtTabbable.h"

class QTextEdit;
class QCheckBox;

namespace Swift {
	class QtXMLConsoleWidget : public QtTabbable, public XMLConsoleWidget {
			Q_OBJECT

		public:
			QtXMLConsoleWidget();

			void show();
			void activate();

			virtual void handleDataRead(const String& data);
			virtual void handleDataWritten(const String& data);

		private:
			virtual void closeEvent(QCloseEvent* event);
			virtual void showEvent(QShowEvent* event);

			void appendTextIfEnabled(const String& data);

		private:
			QTextEdit* textEdit;
			QCheckBox* enabled;
	};
}
