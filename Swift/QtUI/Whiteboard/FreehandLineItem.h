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

#include <QGraphicsItem>
#include <QPainter>

namespace Swift {
    class FreehandLineItem : public QGraphicsItem {
    public:
        enum {Type = UserType + 1};
        FreehandLineItem(QGraphicsItem* parent = nullptr);
        QRectF boundingRect() const;
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/ = nullptr);
        void setStartPoint(QPointF point);
        void lineTo(QPointF point);
        bool collidesWithPath(const QPainterPath& path, Qt::ItemSelectionMode /*mode*/ = Qt::IntersectsItemShape) const;
        void setPen(const QPen& pen);
        QPen pen() const;
        const QVector<QPointF>& points() const;
        int type() const;

    private:
        QPen pen_;
        QVector<QPointF> points_;
        QRectF boundRect;
    };
}
