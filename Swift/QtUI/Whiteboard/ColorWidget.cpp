/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */


#include "ColorWidget.h"
#include <QPainter>
#include <QMouseEvent>

namespace Swift {
	ColorWidget::ColorWidget(QWidget* parent) : QWidget(parent) {
		setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	}

	QSize ColorWidget::sizeHint() const {
		return QSize(20, 20);
	}

	void ColorWidget::setColor(QColor color) {
		this->color = color;
		update();
	}

	void ColorWidget::paintEvent(QPaintEvent* /*event*/) {
		QPainter painter(this);
		painter.fillRect(0, 0, 20, 20, color);
	}

	void ColorWidget::mouseReleaseEvent(QMouseEvent* event) {
		if (event->button() == Qt::LeftButton) {
			emit clicked();
		}
	}
}

