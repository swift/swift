#include "QtXMLConsoleWidget.h"

#include <QCloseEvent>

namespace Swift {
QtXMLConsoleWidget::QtXMLConsoleWidget() {

}

void QtXMLConsoleWidget::showEvent(QShowEvent* event) {
	emit windowOpening();
	QWidget::showEvent(event);
}

void QtXMLConsoleWidget::show() {
	QWidget::show();
	emit windowOpening();
}

void QtXMLConsoleWidget::activate() {
	emit wantsToActivate();
}

void QtXMLConsoleWidget::closeEvent(QCloseEvent* event) {
	emit windowClosing();
	event->accept();
}

}
