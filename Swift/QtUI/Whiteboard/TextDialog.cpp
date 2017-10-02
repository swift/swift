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

#include <Swift/QtUI/Whiteboard/TextDialog.h>

namespace Swift {
    TextDialog::TextDialog(QGraphicsTextItem* item, QWidget* parent) : QDialog(parent)
    {
        this->item = item;

        layout = new QVBoxLayout(this);
        hLayout = new QHBoxLayout;

        editor = new QLineEdit(this);
        connect(editor, SIGNAL(textChanged(const QString&)), this, SLOT(changeItemText(const QString&)));

        fontSizeBox = new QSpinBox(this);
        fontSizeBox->setMinimum(1);
        connect(fontSizeBox, SIGNAL(valueChanged(int)), this, SLOT(changeItemFontSize(int)));
        fontSizeBox->setValue(13);


        buttonBox = new QDialogButtonBox(this);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);
        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

        hLayout->addWidget(editor);
        hLayout->addWidget(fontSizeBox);
        layout->addLayout(hLayout);
        layout->addWidget(buttonBox);
    }

    void TextDialog::changeItemText(const QString &text)
    {
        item->setPlainText(text);
    }

    void TextDialog::changeItemFontSize(int i)
    {
        QFont font = item->font();
        font.setPointSize(i);
        item->setFont(font);
    }

    void TextDialog::accept() {
        emit accepted(item);
        done(QDialog::Accepted);
    }
}
