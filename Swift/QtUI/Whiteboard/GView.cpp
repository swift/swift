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

#include <Swift/QtUI/Whiteboard/GView.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
    GView::GView(QGraphicsScene* scene, QWidget* parent) : QGraphicsView(scene, parent), zValue(0), mousePressed(false), brush(QColor(Qt::white)), defaultBrush(QColor(Qt::white)), mode(GView::Select), lastItem(nullptr), selectionRect(nullptr), textDialog(nullptr) {
    }

    void GView::setLineWidth(int i) {
        pen.setWidth(i);
        if (selectionRect) {
            QGraphicsItem* item = selectionRect->data(1).value<QGraphicsItem*>();
            changePenAndBrush(selectionRect->data(1).value<QGraphicsItem*>(), pen, brush);
            lastItemChanged(item, items_.indexOf(item)+1, Update);
        } else {
            defaultPen.setWidth(i);
        }
    }

    void GView::setLineColor(QColor color) {
        pen.setColor(color);
        if (selectionRect) {
            QGraphicsItem* item = selectionRect->data(1).value<QGraphicsItem*>();
            changePenAndBrush(selectionRect->data(1).value<QGraphicsItem*>(), pen, brush);
            lastItemChanged(item, items_.indexOf(item)+1, Update);
        } else {
            defaultPen.setColor(color);
        }
        lineColorChanged(color);
    }

    QColor GView::getLineColor() {
        return pen.color();
    }

    void GView::setBrushColor(QColor color) {
        brush.setColor(color);
        if (selectionRect) {
            QGraphicsItem* item = selectionRect->data(1).value<QGraphicsItem*>();
            changePenAndBrush(selectionRect->data(1).value<QGraphicsItem*>(), pen, brush);
            lastItemChanged(item, items_.indexOf(item)+1, Update);
        } else {
            defaultBrush.setColor(color);
        }
        brushColorChanged(color);
    }

    QColor GView::getBrushColor() {
        return brush.color();
    }

    void GView::setMode(Mode mode) {
        this->mode = mode;
        lastItem = nullptr;
        deselect();
    }

    void GView::addItem(QGraphicsItem* item, QString id, int pos) {
        itemsMap_.insert(id, item);
        if (pos > items_.size()) {
            item->setZValue(zValue++);
            scene()->addItem(item);
            items_.append(item);
        } else {
            QGraphicsItem* temp = items_.at(pos-1);
            item->setZValue(temp->zValue());
            scene()->addItem(item);
            item->stackBefore(temp);
            items_.insert(pos-1, item);
        }
    }

    void GView::clear() {
        scene()->clear();
        items_.clear();
        itemsMap_.clear();
        lastItem = nullptr;
        selectionRect = nullptr;
        brush = QBrush(QColor(Qt::white));
        defaultBrush = QBrush(QColor(Qt::white));
        pen = QPen();
        pen.setWidth(1);
        defaultPen = pen;
        lineWidthChanged(1);
        lineColorChanged(pen.color());
        brushColorChanged(brush.color());
    }

    QGraphicsItem* GView::getItem(QString id) {
        return itemsMap_.value(id);
    }

    void GView::deleteItem(QString id) {
        deselect(id);
        QGraphicsItem* item = itemsMap_.value(id);
        items_.removeOne(item);
        itemsMap_.remove(id);
        scene()->removeItem(item);
        delete item;
    }

    QString GView::getNewID() {
        return P2QSTRING(idGenerator.generateID());
    }

    void GView::mouseMoveEvent(QMouseEvent* event)
    {
        if (!mousePressed) {
            return;
        }

        if (mode == Line) {
            QGraphicsLineItem* item = qgraphicsitem_cast<QGraphicsLineItem*>(lastItem);
            if(item != nullptr) {
                QLineF line = item->line();
                line.setP1(this->mapToScene(event->pos()));
                item->setLine(line);

            }
        }
        else if (mode == Rect) {
            QGraphicsRectItem* item = qgraphicsitem_cast<QGraphicsRectItem*>(lastItem);
            if (item != nullptr) {
                QPointF beginPoint = item->data(0).toPointF();
                QPointF newPoint = this->mapToScene(event->pos());
                QRectF rect = item->rect();
                if (beginPoint.x() <= newPoint.x() && beginPoint.y() <= newPoint.y()) {
                    rect.setTopLeft(beginPoint);
                    rect.setBottomRight(newPoint);
                }
                else if (beginPoint.x() > newPoint.x() && beginPoint.y() <= newPoint.y()) {
                    rect.setTopRight(beginPoint);
                    rect.setBottomLeft(newPoint);
                }
                else if (beginPoint.x() <= newPoint.x() && beginPoint.y() > newPoint.y()) {
                    rect.setBottomLeft(beginPoint);
                    rect.setTopRight(newPoint);
                }
                else if (beginPoint.x() > newPoint.x() && beginPoint.y() > newPoint.y()) {
                    rect.setBottomRight(beginPoint);
                    rect.setTopLeft(newPoint);
                }
                item->setRect(rect);
            }
        }
        else if (mode == Circle) {
            QGraphicsEllipseItem* item = qgraphicsitem_cast<QGraphicsEllipseItem*>(lastItem);
            QPainterPath path;
            QPointF beginPoint = item->data(0).toPointF();
            QPointF newPoint = this->mapToScene(event->pos());
            QRectF rect = item->rect();
            if (beginPoint.x() <= newPoint.x() && beginPoint.y() <= newPoint.y()) {
                rect.setTopLeft(beginPoint);
                rect.setBottomRight(newPoint);
            }
            else if (beginPoint.x() > newPoint.x() && beginPoint.y() <= newPoint.y()) {
                rect.setTopRight(beginPoint);
                rect.setBottomLeft(newPoint);
            }
            else if (beginPoint.x() <= newPoint.x() && beginPoint.y() > newPoint.y()) {
                rect.setBottomLeft(beginPoint);
                rect.setTopRight(newPoint);
            }
            else if (beginPoint.x() > newPoint.x() && beginPoint.y() > newPoint.y()) {
                rect.setBottomRight(beginPoint);
                rect.setTopLeft(newPoint);
            }

            item->setRect(rect);
        }
        else if (mode == HandLine) {
            FreehandLineItem* item  = qgraphicsitem_cast<FreehandLineItem*>(lastItem);
            if (item != nullptr) {
                QPointF newPoint = this->mapToScene(event->pos());
                item->lineTo(newPoint);
            }
        }
        else if (mode == Polygon) {
            QGraphicsPolygonItem* item = qgraphicsitem_cast<QGraphicsPolygonItem*>(lastItem);
            QPointF newPoint = this->mapToScene(event->pos());
            QPolygonF polygon = item->polygon();
            polygon.erase(polygon.end()-1);
            polygon.append(newPoint);
            item->setPolygon(polygon);
        }
        else if (mode == Select) {
            QGraphicsItem* item = selectionRect->data(1).value<QGraphicsItem*>();
            if (item != nullptr) {
                QPainterPath path;
                QPointF beginPoint = selectionRect->data(0).toPointF();
                QPointF newPoint = this->mapToScene(event->pos());
                item->setPos(beginPoint + newPoint);
                selectionRect->setPos(beginPoint + newPoint);
            }
        }
    }

    void GView::mousePressEvent(QMouseEvent *event)
    {
        mousePressed = true;
        deselect();
        if (mode == Line) {
            QPointF point = this->mapToScene(event->pos());
            QGraphicsItem* item = scene()->addLine(point.x(), point.y(), point.x(), point.y(), pen);
            QString id = getNewID();
            item->setZValue(10000000);
            item->setData(100, id);
            item->setData(101, items_.size());
            lastItem = item;
        }
        else if (mode == Rect) {
            QPointF point = this->mapToScene(event->pos());
            QGraphicsRectItem* item = scene()->addRect(point.x(), point.y(), 0, 0, pen, brush);
            QString id = getNewID();
            item->setZValue(10000000);
            item->setData(0, point);
            item->setData(100, id);
            item->setData(101, items_.size());
            lastItem = item;
        }
        else if (mode == Rubber) {
            QPointF point = this->mapToScene(event->pos());
            int w = pen.width();
            QRectF rect(point.x()-w, point.y()-w, w*2, w*2);
            QList<QGraphicsItem*> list = scene()->items(rect);
            if (!list.isEmpty())
            {
                QGraphicsItem* item = scene()->items(rect).first();
                QString id = item->data(100).toString();
                int pos = items_.indexOf(item)+1;
                itemDeleted(id, pos);
                deleteItem(id);
            }
        }
        else if (mode == Circle) {
            QPointF point = this->mapToScene(event->pos());
            QGraphicsEllipseItem* item = scene()->addEllipse(point.x(), point.y(), 0, 0, pen, brush);
            QString id = getNewID();
            item->setZValue(10000000);
            item->setData(0, point);
            item->setData(100, id);
            item->setData(101, items_.size());
            lastItem = item;
        }
        else if (mode == HandLine) {
            QPointF point = this->mapToScene(event->pos());
            FreehandLineItem* item = new FreehandLineItem;
            QString id = getNewID();
            item->setPen(pen);
            item->setStartPoint(point);
            item->setZValue(10000000);
            item->setData(100, id);
            item->setData(101, items_.size());
            scene()->addItem(item);
            lastItem = item;
        }
        else if (mode == Text) {
            QPointF point = this->mapToScene(event->pos());
            QGraphicsTextItem* item = scene()->addText("");
            QString id = getNewID();
            item->setData(100, id);
            item->setData(101, items_.size());
            item->setDefaultTextColor(pen.color());
            textDialog = new TextDialog(item, this);
            connect(textDialog, SIGNAL(accepted(QGraphicsTextItem*)), this, SLOT(handleTextItemModified(QGraphicsTextItem*)));
            textDialog->setAttribute(Qt::WA_DeleteOnClose);
            textDialog->show();
            item->setPos(point);
            lastItem = item;
        }
        else if (mode == Polygon) {
            QPointF point = this->mapToScene(event->pos());
            QGraphicsPolygonItem* item = dynamic_cast<QGraphicsPolygonItem*>(lastItem);
            if (item == nullptr) {
                QPolygonF polygon;
                polygon.append(point);
                polygon.append(point);
                item = scene()->addPolygon(polygon, pen, brush);
                QString id = getNewID();
                item->setZValue(10000000);
                item->setData(100, id);
                item->setData(101, items_.size());
                lastItem = item;
            }
            else {
                QPolygonF polygon;
                polygon = item->polygon();
                polygon.append(point);
                item->setPolygon(polygon);
            }
        }
        else if (mode == Select) {
            QPointF point = this->mapToScene(event->pos());
            int w = pen.width();
            if (w == 0) {
                w = 1;
            }
            QRectF rect(point.x()-w, point.y()-w, w*2, w*2);
            QList<QGraphicsItem*> list = scene()->items(rect);
            if (!list.isEmpty()) {
                QPen pen;
                pen.setColor(QColor(Qt::gray));
                pen.setStyle(Qt::DashLine);
                QGraphicsItem *item = scene()->items(rect).first();
                selectionRect = scene()->addRect(item->boundingRect(), pen);
                selectionRect->setZValue(1000000);
                selectionRect->setData(0, item->pos()-point);
                selectionRect->setPos(item->pos());
                QVariant var(QVariant::UserType);
                var.setValue(item);
                selectionRect->setData(1, var);
                setActualPenAndBrushFromItem(item);
            }
        }
    }

    void GView::mouseReleaseEvent(QMouseEvent* /*event*/)
    {
        mousePressed = false;
        QGraphicsPolygonItem* polygon = dynamic_cast<QGraphicsPolygonItem*>(lastItem);
        if (polygon && polygon->polygon().size() >= 3) {
            lastItemChanged(polygon, items_.indexOf(polygon)+1, Update);
        } else if (lastItem) {
            zValue++;
            lastItem->setZValue(zValue++);
            items_.append(lastItem);
            itemsMap_.insert(lastItem->data(100).toString(), lastItem);

            lastItemChanged(lastItem, items_.size(), New);
        } else if (selectionRect){
            QGraphicsItem* item = selectionRect->data(1).value<QGraphicsItem*>();
            lastItemChanged(item, items_.indexOf(item)+1, Update);
        }
    }


    void GView::handleTextItemModified(QGraphicsTextItem* item) {
        lastItemChanged(item, item->data(101).toInt(), Update);
    }

    void GView::moveUpSelectedItem()
    {
        if (selectionRect) {
            QGraphicsItem* item = selectionRect->data(1).value<QGraphicsItem*>();
            int pos = items_.indexOf(item);
            if (pos < items_.size()-1) {
                lastItemChanged(item, pos+1, MoveUp);
                move(item, pos+2);
            }
        }
    }

    void GView::moveDownSelectedItem()
    {
        if (selectionRect) {
            QGraphicsItem* item = selectionRect->data(1).value<QGraphicsItem*>();
            int pos = items_.indexOf(item);
            if (pos > 0) {
                lastItemChanged(item, pos+1, MoveDown);
                move(item, pos);
            }
        }
    }

    void GView::move(QGraphicsItem* item, int npos) {
        int pos = items_.indexOf(item);
        QGraphicsItem* itemAfter = nullptr;
        if (npos-1 > pos) {
            if (npos == items_.size()) {
                item->setZValue(zValue++);
            } else {
                itemAfter = items_.at(npos);
            }

            items_.insert(npos, item);
            items_.removeAt(pos);
        } else if (npos-1 < pos) {
            itemAfter = items_.at(npos-1);
            items_.insert(npos-1, item);
            items_.removeAt(pos+1);
        }
        if (itemAfter) {
            item->setZValue(itemAfter->zValue());
            item->stackBefore(itemAfter);
        }
    }

    void GView::changePenAndBrush(QGraphicsItem* item, QPen pen, QBrush brush) {
        QGraphicsLineItem* lineItem = qgraphicsitem_cast<QGraphicsLineItem*>(item);
        if (lineItem) {
            lineItem->setPen(pen);
        }

        FreehandLineItem* handLineItem = qgraphicsitem_cast<FreehandLineItem*>(item);
        if (handLineItem) {
            handLineItem->setPen(pen);
        }

        QGraphicsRectItem* rectItem = qgraphicsitem_cast<QGraphicsRectItem*>(item);
        if (rectItem) {
            rectItem->setPen(pen);
            rectItem->setBrush(brush);
        }

        QGraphicsTextItem* textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item);
        if (textItem) {
            textItem->setDefaultTextColor(pen.color());
        }

        QGraphicsPolygonItem* polygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
        if (polygonItem) {
            polygonItem->setPen(pen);
            polygonItem->setBrush(brush);
        }

        QGraphicsEllipseItem* ellipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
        if (ellipseItem) {
            ellipseItem->setPen(pen);
            ellipseItem->setBrush(brush);
        }
        lineColorChanged(pen.color());
        brushColorChanged(brush.color());
    }

    void GView::setActualPenAndBrushFromItem(QGraphicsItem* item) {
        QGraphicsLineItem* lineItem = qgraphicsitem_cast<QGraphicsLineItem*>(item);
        if (lineItem) {
            pen = lineItem->pen();
        }

        FreehandLineItem* handLineItem = qgraphicsitem_cast<FreehandLineItem*>(item);
        if (handLineItem) {
            pen = handLineItem->pen();
        }

        QGraphicsRectItem* rectItem = qgraphicsitem_cast<QGraphicsRectItem*>(item);
        if (rectItem) {
            pen = rectItem->pen();
            brush = rectItem->brush();
        }

        QGraphicsTextItem* textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item);
        if (textItem) {
            pen.setColor(textItem->defaultTextColor());
        }

        QGraphicsPolygonItem* polygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
        if (polygonItem) {
            pen = polygonItem->pen();
            brush = polygonItem->brush();
        }

        QGraphicsEllipseItem* ellipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
        if (ellipseItem) {
            pen = ellipseItem->pen();
            brush = ellipseItem->brush();
        }
        lineWidthChanged(pen.width());
        lineColorChanged(pen.color());
        brushColorChanged(brush.color());
    }

    void GView::deselect() {
        if (selectionRect != nullptr)    {
            pen = defaultPen;
            brush = defaultBrush;
            scene()->removeItem(selectionRect);
            delete selectionRect;
            selectionRect = nullptr;
            lineWidthChanged(pen.width());
            lineColorChanged(pen.color());
            brushColorChanged(brush.color());
        }
    }

    void GView::deselect(QString id) {
        if (selectionRect != nullptr)    {
            QGraphicsItem* item = getItem(id);
            if (item && selectionRect->data(1).value<QGraphicsItem*>() == item) {
                pen = defaultPen;
                brush = defaultBrush;
                scene()->removeItem(selectionRect);
                delete selectionRect;
                selectionRect = nullptr;
                lineWidthChanged(pen.width());
                lineColorChanged(pen.color());
                brushColorChanged(brush.color());
            }
        }
    }
}
