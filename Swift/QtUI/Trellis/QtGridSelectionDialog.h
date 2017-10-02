/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QSize>
#include <QWidget>

namespace Swift {

    class QtGridSelectionDialog : public QWidget {
            Q_OBJECT

            Q_PROPERTY(QSize currentGridSize READ getCurrentGridSize WRITE setCurrentGridSize NOTIFY currentGridSizeChanged)
            Q_PROPERTY(QSize minGridSize READ getMinGridSize WRITE setMinGridSize NOTIFY minGridSizeChanged)
            Q_PROPERTY(QSize maxGridSize READ getMaxGridSize WRITE setMaxGridSize NOTIFY maxGridSizeChanged)
        public:
            explicit QtGridSelectionDialog(QWidget* parent = nullptr);

            virtual QSize sizeHint() const;

            void setCurrentGridSize(const QSize& size);
            QSize getCurrentGridSize() const;
            void setMinGridSize(const QSize& size);
            QSize getMinGridSize() const;
            void setMaxGridSize(const QSize& size);
            QSize getMaxGridSize() const;

            QSize getFrameSize() const;
            int getDescriptionTextHeight() const;

        signals:
            void currentGridSizeChanged(QSize);
            void minGridSizeChanged(QSize);
            void maxGridSizeChanged(QSize);

        protected:
            void keyReleaseEvent(QKeyEvent* event);
            void mousePressEvent(QMouseEvent* event);
            void paintEvent(QPaintEvent* event);
            void showEvent(QShowEvent* event);
            void hideEvent(QHideEvent* event);
            bool event(QEvent* event);
            void timerEvent(QTimerEvent* event);

    private:
        int getDescriptionTextHeight(int width) const;

        private:
            int padding;
            int horizontalMargin;
            int verticalMargin;
            int timerId = -1;

            QSize frameSize;

            QSize currentGridSize;
            QSize minGridSize;
            QSize maxGridSize;

            const QString descriptionText;
    };
}
