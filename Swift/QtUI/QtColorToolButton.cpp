/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <QColorDialog>
#include <QPainter>

#include <Swift/QtUI/QtColorToolButton.h>

namespace Swift {

QtColorToolButton::QtColorToolButton(QWidget* parent) :
	QToolButton(parent)
{
	connect(this, SIGNAL(clicked()), SLOT(onClicked()));
	setColorIcon(Qt::transparent);
}

void QtColorToolButton::setColor(const QColor& color)
{
	if (color.isValid() != color_.isValid() || (color.isValid() && color != color_)) {
		color_ = color;
		setColorIcon(color_);
		emit colorChanged(color_);
	}
}

void QtColorToolButton::onClicked()
{
	QColor c = QColorDialog::getColor(color_, this);
	if (c.isValid()) {
		setColor(c);
	}
}

void QtColorToolButton::setColorIcon(const QColor& color)
{
	QPixmap pix(iconSize());
	pix.fill(color.isValid() ? color : Qt::transparent);
	setIcon(pix);
}

}
