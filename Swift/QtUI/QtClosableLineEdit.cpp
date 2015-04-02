/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

/* Contains demo Trolltech code from http://git.forwardbias.in/?p=lineeditclearbutton.git with license: */
/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#include <Swift/QtUI/QtClosableLineEdit.h>

#include <QApplication>
#include <QKeyEvent>
#include <QStyle>
#include <QToolButton>
#include <QtGlobal>

namespace Swift {

const int QtClosableLineEdit::clearButtonPadding = 2;

QtClosableLineEdit::QtClosableLineEdit(QWidget *parent) : QLineEdit(parent) {
	clearButton = new QToolButton(this);
	clearButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	clearButton->setIconSize(QSize(16,16));
	clearButton->setCursor(Qt::ArrowCursor);
	clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	clearButton->hide();
	connect(clearButton, SIGNAL(clicked()), this, SLOT(handleCloseButtonClicked()));
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateCloseButton(const QString&)));
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(clearButton->sizeHint().width() + frameWidth + 1));
	QSize minimumSize = minimumSizeHint();
	setMinimumSize(qMax(minimumSize.width(), clearButton->sizeHint().width() + frameWidth * 2 + clearButtonPadding),
								qMax(minimumSize.height(), clearButton->sizeHint().height() + frameWidth * 2 + clearButtonPadding));
}

void QtClosableLineEdit::resizeEvent(QResizeEvent *) {
	QSize size = clearButton->sizeHint();
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	int verticalAdjustment = 1;
#if defined(Q_OS_WIN32)
	// This vertical adjustment is required on Windows so the close button is vertically centered in the line edit.
	verticalAdjustment += 2;
#endif 
	clearButton->move(rect().right() - frameWidth - size.width(), (rect().bottom() + verticalAdjustment - size.height())/2);
}

void QtClosableLineEdit::updateCloseButton(const QString& text) {
	clearButton->setVisible(!text.isEmpty());
}

void QtClosableLineEdit::handleCloseButtonClicked() {
	clear();
	QApplication::postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier));
	QApplication::postEvent(this, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Escape, Qt::NoModifier));
}

}
