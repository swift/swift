/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <iostream>

#include <QDialog>
#include <QDialogButtonBox>
#include <QGraphicsTextItem>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

namespace Swift {
    class TextDialog : public QDialog
    {
        Q_OBJECT
    public:
        TextDialog(QGraphicsTextItem* item, QWidget* parent = nullptr);

    private:
        QGraphicsTextItem* item;
        QLineEdit* editor;
        QDialogButtonBox* buttonBox;
        QVBoxLayout* layout;
        QHBoxLayout* hLayout;
        QSpinBox* fontSizeBox;

    signals:
        void accepted(QGraphicsTextItem* item);

    private slots:
        void accept();
        void changeItemText(const QString &text);
        void changeItemFontSize(int i);
    };
}

