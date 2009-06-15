#include "QtChatTabs.h"

#include <QTabWidget>
#include <QLayout>

namespace Swift {
QtChatTabs::QtChatTabs() : QWidget() {
	tabs_ = new QTabWidget(this);
#if QT_VERSION >= 0x040500
	//For Macs, change the tab rendering.
	tabs_->setDocumentMode(true);
#endif
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setContentsMargins(0, 3, 0, 0);
	layout->addWidget(tabs_);
	setLayout(layout);
	resize(400, 300);
}

void QtChatTabs::addTab(QtTabbable* tab) {
	tabs_->addTab(tab, tab->windowTitle());
	connect(tab, SIGNAL(titleUpdated()), this, SLOT(tabTitleUpdated()));
}

void QtChatTabs::tabClosing() {

}

void QtChatTabs::tabTitleUpdated() {
	QWidget* widget = qobject_cast<QWidget*>(sender());
	if (!widget) {
		return;
	}
	int index = tabs_->indexOf(widget);
	if (index < 0) {
		return;
	}
	tabs_->setTabText(index, widget->windowTitle());
}

}