#pragma once

#include "Swift/Controllers/UIInterfaces/XMLConsoleWidget.h"
#include "QtTabbable.h"

namespace Swift {
	class QtXMLConsoleWidget : public QtTabbable, public XMLConsoleWidget {
		Q_OBJECT
		public:
			QtXMLConsoleWidget();
	};
}
