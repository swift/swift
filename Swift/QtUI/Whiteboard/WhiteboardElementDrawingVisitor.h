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

#include <Swiften/Elements/Whiteboard/WhiteboardElementVisitor.h>
#include <Swiften/Elements/Whiteboard/WhiteboardEllipseElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardFreehandPathElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardLineElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardPolygonElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardRectElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardTextElement.h>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/Whiteboard/GView.h>

namespace Swift {
    class WhiteboardElementDrawingVisitor : public WhiteboardElementVisitor {
    public:
        WhiteboardElementDrawingVisitor(GView* graphicsView, int pos, GView::Type type) : graphicsView_(graphicsView), pos_(pos), type_(type) {}

        void visit(WhiteboardLineElement& element) {
            QGraphicsLineItem *item;
            if (type_ == GView::New) {
                item = new QGraphicsLineItem(element.x1(), element.y1(), element.x2(), element.y2());
                graphicsView_->addItem(item, P2QSTRING(element.getID()), pos_);
            } else {
                item = qgraphicsitem_cast<QGraphicsLineItem*>(graphicsView_->getItem(P2QSTRING(element.getID())));
                QLineF line(element.x1(), element.y1(), element.x2(), element.y2());
                item->setLine(line);
                item->setPos(0,0);
                graphicsView_->deselect(P2QSTRING(element.getID()));
            }
            if (item) {
                QPen pen;
                WhiteboardColor color = element.getColor();
                pen.setColor(QColor(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()));
                pen.setWidth(element.getPenWidth());
                item->setPen(pen);
                QString id = P2QSTRING(element.getID());
                item->setData(100, id);
            }
        }

        void visit(WhiteboardFreehandPathElement& element) {
            FreehandLineItem *item;
            if (type_ == GView::New) {
                item = new FreehandLineItem;
            } else {
                item = qgraphicsitem_cast<FreehandLineItem*>(graphicsView_->getItem(P2QSTRING(element.getID())));
                item->setPos(0,0);
                graphicsView_->deselect(P2QSTRING(element.getID()));
            }

            if (item) {
                QPen pen;
                WhiteboardColor color = element.getColor();
                pen.setColor(QColor(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()));
                pen.setWidth(element.getPenWidth());
                item->setPen(pen);

                std::vector<std::pair<int, int> >::const_iterator it = element.getPoints().begin();
                item->setStartPoint(QPointF(it->first, it->second));
                for (++it; it != element.getPoints().end(); ++it) {
                    item->lineTo(QPointF(it->first, it->second));
                }

                QString id = P2QSTRING(element.getID());
                item->setData(100, id);
            }
            if (type_ == GView::New) {
                graphicsView_->addItem(item, P2QSTRING(element.getID()), pos_);
            }
        }

        void visit(WhiteboardRectElement& element) {
            QGraphicsRectItem* item;
            if (type_ == GView::New) {
                item = new QGraphicsRectItem(element.getX(), element.getY(), element.getWidth(), element.getHeight());
                graphicsView_->addItem(item, P2QSTRING(element.getID()), pos_);
            } else {
                item = qgraphicsitem_cast<QGraphicsRectItem*>(graphicsView_->getItem(P2QSTRING(element.getID())));
                QRectF rect(element.getX(), element.getY(), element.getWidth(), element.getHeight());
                item->setRect(rect);
                item->setPos(0,0);
                graphicsView_->deselect(P2QSTRING(element.getID()));
            }

            if (item) {
                QPen pen;
                QBrush brush(Qt::SolidPattern);
                WhiteboardColor penColor = element.getPenColor();
                WhiteboardColor brushColor = element.getBrushColor();
                pen.setColor(QColor(penColor.getRed(), penColor.getGreen(), penColor.getBlue(), penColor.getAlpha()));
                pen.setWidth(element.getPenWidth());
                brush.setColor(QColor(brushColor.getRed(), brushColor.getGreen(), brushColor.getBlue(), brushColor.getAlpha()));
                item->setPen(pen);
                item->setBrush(brush);
                QString id = P2QSTRING(element.getID());
                item->setData(100, id);
            }
        }

        void visit(WhiteboardPolygonElement& element) {
            QGraphicsPolygonItem* item = qgraphicsitem_cast<QGraphicsPolygonItem*>(graphicsView_->getItem(P2QSTRING(element.getID())));
            if (item == nullptr && type_ == GView::New) {
                item = new QGraphicsPolygonItem();
                QString id = P2QSTRING(element.getID());
                item->setData(100, id);
                graphicsView_->addItem(item, id, pos_);
            }
            graphicsView_->deselect(P2QSTRING(element.getID()));
            QPen pen;
            QBrush brush(Qt::SolidPattern);
            WhiteboardColor penColor = element.getPenColor();
            WhiteboardColor brushColor = element.getBrushColor();
            pen.setColor(QColor(penColor.getRed(), penColor.getGreen(), penColor.getBlue(), penColor.getAlpha()));
            pen.setWidth(element.getPenWidth());
            brush.setColor(QColor(brushColor.getRed(), brushColor.getGreen(), brushColor.getBlue(), brushColor.getAlpha()));
            item->setPen(pen);
            item->setBrush(brush);

            item->setPos(0,0);
            QPolygonF polygon;
            std::vector<std::pair<int, int> >::const_iterator it = element.getPoints().begin();
            for (; it != element.getPoints().end(); ++it) {
                polygon.append(QPointF(it->first, it->second));
            }
            item->setPolygon(polygon);
        }

        void visit(WhiteboardTextElement& element) {
            QGraphicsTextItem* item;
            QString id = P2QSTRING(element.getID());
            if (type_ == GView::New) {
                item = new QGraphicsTextItem;
                graphicsView_->addItem(item, id, pos_);
            } else {
                item = qgraphicsitem_cast<QGraphicsTextItem*>(graphicsView_->getItem(id));
                graphicsView_->deselect(P2QSTRING(element.getID()));
            }
            if (item) {
                item->setPlainText(P2QSTRING(element.getText()));
                item->setPos(QPointF(element.getX(), element.getY()));
                QFont font = item->font();
                font.setPointSize(element.getSize());
                item->setFont(font);
                WhiteboardColor color = element.getColor();
                item->setDefaultTextColor(QColor(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()));
                item->setData(100, id);
            }
        }

        void visit(WhiteboardEllipseElement& element) {
            QRectF rect;
            QGraphicsEllipseItem* item;
            QString id = P2QSTRING(element.getID());
            rect.setTopLeft(QPointF(element.getCX()-element.getRX(), element.getCY()-element.getRY()));
            rect.setBottomRight(QPointF(element.getCX()+element.getRX(), element.getCY()+element.getRY()));
            if (type_ == GView::New) {
                item = new QGraphicsEllipseItem(rect);
                graphicsView_->addItem(item, id, pos_);
            } else {
                item = qgraphicsitem_cast<QGraphicsEllipseItem*>(graphicsView_->getItem(id));
                item->setRect(rect);
                item->setPos(0,0);
                graphicsView_->deselect(P2QSTRING(element.getID()));
            }
            QPen pen;
            QBrush brush(Qt::SolidPattern);
            WhiteboardColor penColor = element.getPenColor();
            WhiteboardColor brushColor = element.getBrushColor();
            pen.setColor(QColor(penColor.getRed(), penColor.getGreen(), penColor.getBlue(), penColor.getAlpha()));
            pen.setWidth(element.getPenWidth());
            brush.setColor(QColor(brushColor.getRed(), brushColor.getGreen(), brushColor.getBlue(), brushColor.getAlpha()));
            item->setPen(pen);
            item->setBrush(brush);
            item->setData(100, id);
        }

    private:
        GView* graphicsView_;
        int pos_;
        GView::Type type_;
    };
}
