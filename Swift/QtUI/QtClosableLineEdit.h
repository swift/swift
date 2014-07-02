/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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

#pragma once

#include <QLineEdit>

class QToolButton;

namespace Swift {

class QtClosableLineEdit : public QLineEdit
{
	Q_OBJECT
	public:
		QtClosableLineEdit(QWidget *parent = 0);

	protected:
		void resizeEvent(QResizeEvent *);

	private slots:
		void updateCloseButton(const QString &text);
		void handleCloseButtonClicked();

	private:
		static const int clearButtonPadding;
		QToolButton *clearButton;
};

}
