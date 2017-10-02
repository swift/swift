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

    height += getDescriptionTextHeight(width);

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

QSize QtGridSelectionDialog::getFrameSize() const {
    return frameSize;
}

int QtGridSelectionDialog::getDescriptionTextHeight() const {
    auto width = horizontalMargin + frameSize.width() + (padding + frameSize.width()) * (currentGridSize.width() - 1) + horizontalMargin;
    return getDescriptionTextHeight(width);
}

int QtGridSelectionDialog::getDescriptionTextHeight(int width) const {
    // Height of descriptive centered text below trellis
    auto fontMetrics = QFontMetrics(QApplication::font());
    auto descriptionBB = fontMetrics.boundingRect(QRect(0, 0, width - 2 * horizontalMargin, 1000), Qt::TextWordWrap, descriptionText, 0, 0);

    return (descriptionBB.height() + descriptionBB.y());
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
            QCursor::setPos(mapToGlobal(QPoint(newSizeHint.width(), newSizeHint.height()- getDescriptionTextHeight()) - QPoint(frameSize.width() / 2, frameSize.height() / 2)));
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
    timerId = startTimer(1000 / 25);
}

void QtGridSelectionDialog::hideEvent(QHideEvent*) {
    killTimer(timerId);
}

void QtGridSelectionDialog::timerEvent(QTimerEvent*) {

    const QPoint diff = QCursor::pos() - frameGeometry().topLeft() - QPoint(horizontalMargin, verticalMargin);
    const auto toleranceFactor = 4; // Ratio of how far (1/tolerance) the mouse should move for the next frame to be plotted
    // dx, dy - mouse position with respect to first top-left square
    const auto dx = diff.x();
    const auto dy = diff.y();
    // width, height - dimension of each square with padding
    const auto width = frameSize.width() + padding;
    const auto height = frameSize.height() + padding;
    // xThreshold, yThreshold - how far the mouse should be moved so that a new square is added or the existing one is hidden
    const auto xThreshold = width / toleranceFactor;
    const auto yThreshold = height / toleranceFactor;

    const auto getSize = [](int length, int threshold, int delta) {
        if (delta < length + threshold) {
            return 1;
        }
        else {
            return (delta + (length - threshold)) / length;
        }
    };
    const QSize newGridSize(getSize(width, xThreshold, dx), getSize(height, yThreshold, dy));

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
