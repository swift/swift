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

#include <QGraphicsLineItem>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPen>

#include <Swiften/Base/IDGenerator.h>

#include <Swift/QtUI/Whiteboard/FreehandLineItem.h>
#include <Swift/QtUI/Whiteboard/TextDialog.h>

namespace Swift {
    class GView : public QGraphicsView {
        Q_OBJECT
    public:
        enum Mode {    Rubber, Line, Rect, Circle, HandLine, Text, Polygon, Select };
        enum Type { New, Update, MoveUp, MoveDown };
        GView(QGraphicsScene* scene, QWidget* parent = nullptr);
        void setLineWidth(int i);
        void setLineColor(QColor color);
        QColor getLineColor();
        void setBrushColor(QColor color);
        QColor getBrushColor();
        void setMode(Mode mode);
        void mouseMoveEvent(QMouseEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* /*event*/);
        void addItem(QGraphicsItem* item, QString id, int pos);
        void clear();
        QGraphicsItem* getItem(QString id);
        void deleteItem(QString id);
        QString getNewID();
        void move(QGraphicsItem* item, int npos);
        void deselect(QString id);

    signals:
        void lastItemChanged(QGraphicsItem* item, int pos, GView::Type type);
        void itemDeleted(QString id, int pos);
        void lineWidthChanged(int i);
        void lineColorChanged(QColor color);
        void brushColorChanged(QColor color);

    public slots:
        void moveUpSelectedItem();
        void moveDownSelectedItem();

    private slots:
        void handleTextItemModified(QGraphicsTextItem*);

    private:
        void changePenAndBrush(QGraphicsItem* item, QPen pen, QBrush brush);
        void setActualPenAndBrushFromItem(QGraphicsItem* item);
        void deselect();

        int zValue;
        bool mousePressed;
        QPen pen;
        QBrush brush;
        QPen defaultPen;
        QBrush defaultBrush;
        Mode mode;
        QGraphicsItem* lastItem;
        QGraphicsRectItem* selectionRect;
        TextDialog* textDialog;
        QMap<QString, QGraphicsItem*> itemsMap_;
        QList<QGraphicsItem*> items_;
        IDGenerator idGenerator;
    };
}
