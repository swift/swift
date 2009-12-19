#include "QtTabWidget.h"

namespace Swift {

QtTabWidget::QtTabWidget(QWidget* parent) : QTabWidget(parent) {

}

QtTabWidget::~QtTabWidget() {

}

QTabBar* QtTabWidget::tabBar() {
	return QTabWidget::tabBar();
}

}
