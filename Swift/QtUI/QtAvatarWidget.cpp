/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */



#include <Swift/QtUI/QtAvatarWidget.h>

#include <QAction>
#include <QBuffer>
#include <QFileDialog>
#include <QImageReader>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QVBoxLayout>

#include <Swiften/Base/Path.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtAvatarWidget::QtAvatarWidget(QWidget* parent) : QWidget(parent), editable(false) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);

    QSizePolicy sp(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sp.setHorizontalStretch(0);
    sp.setVerticalStretch(0);
    setSizePolicy(sp);
    setMinimumSize(QSize(96, 96));
    setMaximumSize(QSize(96, 96));

    label = new QLabel(this);
    label->setWordWrap(true);
    label->setSizePolicy(sp);
    label->setMinimumSize(QSize(96, 96));
    label->setMaximumSize(QSize(96, 96));
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}

void QtAvatarWidget::setAvatar(const ByteArray& data, const std::string& type) {
    this->data = data;
    this->type = type;

    QImage image;
    if (!data.empty()) {
        image.loadFromData(reinterpret_cast<const uchar*>(vecptr(data)), data.size());
    }

    if (image.isNull()) {
        image = QImage(":/icons/no-avatar.png");
        QPainter painter(&image);
        painter.setPen(Qt::gray);
        QFont font = painter.font();
        font.setPointSize(14);
        painter.setFont(font);
        painter.drawText(0, 0, image.height(), image.width(), Qt::AlignHCenter | Qt::AlignVCenter, tr("No picture"));
    }

    if (image.height() > label->height() || image.width() > label->width()) {
        image = image.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    label->setPixmap(QPixmap::fromImage(image));
}

void QtAvatarWidget::mousePressEvent(QMouseEvent* event) {
    if (!editable) {
        return;
    }
    QMenu menu;

    QAction* selectPicture = new QAction(tr("Select picture ..."), this);
    menu.addAction(selectPicture);

    QAction* clearPicture = new QAction(tr("Clear picture"), this);
    menu.addAction(clearPicture);

    QAction* result = menu.exec(event->globalPos());
    if (result == selectPicture) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Select picture"), "", tr("Image Files (*.png *.jpg *.jpeg *.gif)"));
        if (!fileName.isEmpty()) {
            ByteArray data;
            readByteArrayFromFile(data, stringToPath(Q2PSTRING(fileName)));

            QBuffer buffer;
            buffer.setData(reinterpret_cast<const char*>(vecptr(data)), data.size());
            buffer.open(QIODevice::ReadOnly);
            QString type = QImageReader::imageFormat(&buffer).toLower();
            if (!type.isEmpty()) {
                type = "image/" + type;
                setAvatar(data, Q2PSTRING(type));
            }
            else {
                QMessageBox::critical(this, tr("Error"), tr("The selected picture is in an unrecognized format"));
            }
        }
    }
    else if (result == clearPicture) {
        setAvatar(ByteArray(), "");
    }
}



}
