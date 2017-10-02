/*
 * Copyright (c) 2014-2016 Isode Limited.
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

#pragma once

#include <QLineEdit>

class QToolButton;

namespace Swift {

class QtClosableLineEdit : public QLineEdit
{
    Q_OBJECT
    public:
        QtClosableLineEdit(QWidget *parent = nullptr);

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
