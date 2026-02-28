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

#include <Swift/QtUI/Whiteboard/FreehandLineItem.h>

namespace Swift {
    FreehandLineItem::FreehandLineItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
    }

    QRectF FreehandLineItem::boundingRect() const
    {
        return boundRect;
    }

    void FreehandLineItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
    {
        painter->setPen(pen_);
        if (points_.size() > 0) {
            QVector<QPointF>::const_iterator it = points_.begin();
            QPointF previous = *it;
            ++it;
            for (; it != points_.end(); ++it) {
                painter->drawLine(previous, *it);
                previous = *it;
            }
        }
    }

    void FreehandLineItem::setStartPoint(QPointF point)
    {
        points_.clear();
        points_.append(point);
        QRectF rect(point, point);
        prepareGeometryChange();
        boundRect = rect;
    }

    void FreehandLineItem::lineTo(QPointF point)
    {
        qreal x1, x2, y1, y2;
        x1 = points_.last().x();
        x2 = point.x();
        y1 = points_.last().y();
        y2 = point.y();
        if (x1 > x2) {
            qreal temp = x1;
            x1 = x2;
            x2 = temp;
        }
        if (y1 > y2) {
            qreal temp = y1;
            y1 = y2;
            y2 = temp;
        }
        QRectF rect(x1-1, y1-1, x2+1-x1, y2+1-y1);

        points_.append(point);

        prepareGeometryChange();
        boundRect |= rect;
    }

    bool FreehandLineItem::collidesWithPath(const QPainterPath& path, Qt::ItemSelectionMode /*mode*/) const
    {
        QVector<QPointF>::const_iterator it;
        QSizeF size(1,1);
        for (it = points_.begin(); it != points_.end(); ++it)    {
            if (path.intersects(QRectF(*it, size))) {
                return true;
            }
        }
        return false;
    }

    void FreehandLineItem::setPen(const QPen& pen)
    {
        pen_ = pen;
        update(boundRect);
    }

    QPen FreehandLineItem::pen() const
    {
        return pen_;
    }

    const QVector<QPointF>& FreehandLineItem::points() const {
        return points_;
    }

    int FreehandLineItem::type() const {
        return Type;
    }
}
