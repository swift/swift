/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/Trellis/QtGridSelectionDialog.h>

#include <QPaintEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionFrame>
#include <QStyleOptionMenuItem>
#include <QStyleOptionSizeGrip>

namespace Swift {

QtGridSelectionDialog::QtGridSelectionDialog(QWidget* parent) : QWidget(parent) {
	frameSize = QSize(23,23) * 2;
	maxGridSize = QSize(7,7);
	minGridSize = QSize(1,1);
	currentGridSize = QSize(1,1);
	padding = 4;
	setWindowFlags(Qt::FramelessWindowHint);
	setCursor(Qt::SizeAllCursor);
	horizontalMargin = style()->pixelMetric(QStyle::PM_MenuVMargin) * 2;
	verticalMargin = style()->pixelMetric(QStyle::PM_MenuVMargin) * 2;
}

QSize QtGridSelectionDialog::sizeHint() const {
	// PM_MenuVMargin | frameSize | ( padding | frameSize ) * | PM_MenuVMargin
	int width = horizontalMargin + frameSize.width() + (padding + frameSize.width()) * (currentGridSize.width() - 1) + horizontalMargin;
	int height = verticalMargin + frameSize.height() + (padding + frameSize.height()) * (currentGridSize.height() - 1) + verticalMargin;
	return QSize(width, height);
}

void QtGridSelectionDialog::setCurrentGridSize(const QSize& size) {
	currentGridSize = size;
	emit currentGridSizeChanged(size);
}

QSize QtGridSelectionDialog::getCurrentGridSize() const {
	return currentGridSize;
}

void QtGridSelectionDialog::setMinGridSize(const QSize& size) {
	minGridSize = size;
	emit minGridSizeChanged(size);
}

QSize QtGridSelectionDialog::getMinGridSize() const {
	return minGridSize;
}

void QtGridSelectionDialog::setMaxGridSize(const QSize& size) {
	maxGridSize = size;
	emit maxGridSizeChanged(size);
}

QSize QtGridSelectionDialog::getMaxGridSize() const {
	return maxGridSize;
}

void QtGridSelectionDialog::keyReleaseEvent(QKeyEvent* event) {
	if (event) {
		QSize newGridSize = currentGridSize;
		if (event->key() == Qt::Key_Up) {
			newGridSize += QSize(0, -1);
		}
		else if (event->key() == Qt::Key_Down) {
			newGridSize += QSize(0, 1);
		}
		else if (event->key() == Qt::Key_Left) {
			newGridSize += QSize(-1, 0);
		}
		else if (event->key() == Qt::Key_Right) {
			newGridSize += QSize(1, 0);
		}
		else if (event->key() == Qt::Key_Return) {
			hide();
			setCurrentGridSize(currentGridSize);
		}
		if (minGridSize.expandedTo(newGridSize).boundedTo(maxGridSize) != currentGridSize) {
			currentGridSize = minGridSize.expandedTo(newGridSize).boundedTo(maxGridSize);
			resize(sizeHint());
		}
	}
}

void QtGridSelectionDialog::mousePressEvent(QMouseEvent*) {
	hide();
	setCurrentGridSize(currentGridSize);
}

void QtGridSelectionDialog::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	QStyleOptionMenuItem option;
	option.state = QStyle::State_Enabled | QStyle::State_Selected;
	option.menuRect = QRect(QPoint(0,0), frameSize);
	for (int x = 0; x < currentGridSize.width(); x++) {
		for (int y = 0; y < currentGridSize.height(); y++) {
			int xPos = horizontalMargin + (x * (frameSize.width() + padding));
			int yPos = verticalMargin + (y * (frameSize.height() + padding));
			option.menuRect.moveTo(QPoint(xPos, yPos));
			option.rect = option.menuRect;
			style()->drawControl(QStyle::CE_MenuItem, &option, &painter, 0);
		}
	}

	QStyleOptionSizeGrip sizeGripOption;
	sizeGripOption.init(this);
	sizeGripOption.corner = Qt::BottomRightCorner;
	style()->drawControl(QStyle::CE_SizeGrip, &sizeGripOption, &painter, this);
}

void QtGridSelectionDialog::showEvent(QShowEvent*) {
	int xPos = horizontalMargin + frameSize.width() + (padding + frameSize.width()) * (currentGridSize.width() - 1) - frameSize.width()/2;
	int yPos = verticalMargin + frameSize.height() + (padding + frameSize.height()) * (currentGridSize.height() - 1) - frameSize.height()/2;
	QCursor::setPos(mapToGlobal(QPoint(xPos, yPos)));
	setMouseTracking(true);
}

void QtGridSelectionDialog::hideEvent(QHideEvent*) {
	setMouseTracking(false);
}

void QtGridSelectionDialog::mouseMoveEvent(QMouseEvent*) {
	QPoint diff = (frameGeometry().bottomRight() - QCursor::pos());
	QSize newDimensions = currentGridSize;
	if (diff.x() > frameSize.width() * 1.5) {
		newDimensions -= QSize(diff.x() / (frameSize.width() * 1.5), 0);
	}
	if (diff.y() > frameSize.height() * 1.5) {
		newDimensions -= QSize(0, diff.y() / (frameSize.height() * 1.5));
	}
	if (minGridSize.expandedTo(newDimensions).boundedTo(maxGridSize) != currentGridSize) {
		currentGridSize = minGridSize.expandedTo(newDimensions).boundedTo(maxGridSize);
		resize(sizeHint());
	}
}

void QtGridSelectionDialog::leaveEvent(QEvent *) {
	QPoint diff = (frameGeometry().bottomRight() - QCursor::pos());
	QSize newGridSize = currentGridSize;
	if (diff.x() < 0) {
		newGridSize += QSize(1,0);
	}
	if (diff.y() < 0) {
		newGridSize += QSize(0,1);
	}
	if (minGridSize.expandedTo(newGridSize).boundedTo(maxGridSize) != currentGridSize) {
		currentGridSize = minGridSize.expandedTo(newGridSize).boundedTo(maxGridSize);
		resize(sizeHint());
	}
}

}
