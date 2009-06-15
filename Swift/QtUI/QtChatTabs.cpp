#pragma once

#include "QtChatTabs.h"

#include <QTabWidget>
#include <QLayout>

namespace Swift {
QtChatTabs::QtChatTabs() {
	tabs_ = new QTabWidget(this);
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setContentsMargins(0, 3, 0, 0);
	layout->addWidget(tabs_);
	setLayout(layout);
	resize(400, 300);
}

void QtChatTabs::addTab(QWidget* tab) {
	tabs_->addTab(tab, tab->windowTitle());
}

void QtChatTabs::tabClosing() {

}

}