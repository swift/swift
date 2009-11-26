#include "QtXMLConsoleWidgetFactory.h"

#include "QtXMLConsoleWidget.h"
#include "QtChatTabs.h"

namespace Swift {

QtXMLConsoleWidgetFactory::QtXMLConsoleWidgetFactory(QtChatTabs* tabs) {
	tabs_ = tabs;
}

XMLConsoleWidget* QtXMLConsoleWidgetFactory::createXMLConsoleWidget() {
	QtXMLConsoleWidget* widget = new QtXMLConsoleWidget();
	tabs_->addTab(widget);
	if (!tabs_->isVisible()) {
		tabs_->show();
	}
	widget->show();
	return widget;
}

}
