/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/Trellis/QtGridSelectionDialog.h>

#include <QApplication>
#include <QCursor>
#include <QPaintEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

namespace Swift {

QtGridSelectionDialog::QtGridSelectionDialog(QWidget* parent) : QWidget(parent), descriptionText(tr("Select the number of rows and columns for your layout. You can change the size by moving the mouse or cursor keys.")) {
    frameSize = QSize(28,28) * 2;
    maxGridSize = QSize(7,7);
    minGridSize = QSize(1,1);
    currentGridSize = QSize(1,1);
    padding = 4;
    setWindowFlags(Qt::FramelessWindowHint);
    setCursor(Qt::SizeAllCursor);
    horizontalMargin = style()->pixelMetric(QStyle::PM_LayoutLeftMargin);
    verticalMargin = style()->pixelMetric(QStyle::PM_LayoutBottomMargin);
}

QSize QtGridSelectionDialog::sizeHint() const {
    // PM_MenuVMargin | frameSize | ( padding | frameSize ) * | PM_MenuVMargin
    int width = horizontalMargin + frameSize.width() + (padding + frameSize.width()) * (currentGridSize.width() - 1) + horizontalMargin;
    int height = verticalMargin + frameSize.height() + (padding + frameSize.height()) * (currentGridSize.height() - 1) + verticalMargin;

    // Add space for descriptive centered text below.
    auto fontMetrics = QFontMetrics(QApplication::font());
    auto descriptionBB = fontMetrics.boundingRect(QRect(0,0,width - 2*horizontalMargin,1000), Qt::TextWordWrap, descriptionText, 0, 0);

    height += descriptionBB.height() + descriptionBB.y();

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
        else if (event->key() == Qt::Key_Escape) {
            hide();
        }

        if (minGridSize.expandedTo(newGridSize).boundedTo(maxGridSize) != currentGridSize) {
            currentGridSize = minGridSize.expandedTo(newGridSize).boundedTo(maxGridSize);

            QSize newSizeHint = sizeHint();
            resize(newSizeHint);
            QCursor::setPos(mapToGlobal(QPoint(newSizeHint.width(), newSizeHint.height()) - QPoint(frameSize.width() / 2, frameSize.height() / 2)));
        }
    }
}

void QtGridSelectionDialog::mousePressEvent(QMouseEvent*) {
    hide();
    setCurrentGridSize(currentGridSize);
}

void QtGridSelectionDialog::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    // draw grid
    QRect gridCell = QRect(QPoint(0,0), frameSize);
    painter.setBrush(palette().highlight());
    painter.setPen(Qt::NoPen);
    for (int x = 0; x < currentGridSize.width(); x++) {
        for (int y = 0; y < currentGridSize.height(); y++) {
            int xPos = horizontalMargin + (x * (frameSize.width() + padding));
            int yPos = verticalMargin + (y * (frameSize.height() + padding));
            gridCell.moveTo(QPoint(xPos, yPos));
            painter.drawRect(gridCell);
        }
    }

    // draw description text
    auto fontMetrics = QFontMetrics(QApplication::font());
    auto descriptionBB = fontMetrics.boundingRect(QRect(0,0, width() - 2 * horizontalMargin,0), Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, descriptionText, 0, 0);

    QStyleOption opt;
    opt.initFrom(this);
    int textY = verticalMargin + (currentGridSize.height() * (frameSize.height() + padding));
    int textX = (size().width() - descriptionBB.width()) / 2;
    style()->drawItemText(&painter, QRect(textX, textY, descriptionBB.width(), descriptionBB.height()), Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, opt.palette, true, descriptionText, foregroundRole());
}

void QtGridSelectionDialog::showEvent(QShowEvent*) {
    int xPos = horizontalMargin + frameSize.width() + (padding + frameSize.width()) * (currentGridSize.width() - 1) - frameSize.width()/2;
    int yPos = verticalMargin + frameSize.height() + (padding + frameSize.height()) * (currentGridSize.height() - 1) - frameSize.height()/2;
    // Apply the above offset to the global cursor position (which was positioned at a known position before showing the widget):
    QPoint pos = QCursor::pos();
    pos.rx() += xPos;
    pos.ry() += yPos;
    QCursor::setPos(pos);
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

bool QtGridSelectionDialog::event(QEvent* event) {
    // Hide the window when it becomes a non-top-level window.
    if (event->type() == QEvent::WindowDeactivate) {
        hide();
        return true;
    }
    return QWidget::event(event);
}

}
