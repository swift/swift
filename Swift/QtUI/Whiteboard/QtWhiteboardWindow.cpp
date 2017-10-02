/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/Whiteboard/QtWhiteboardWindow.h>

#include <iostream>
#include <memory>

#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <QLabel>
#include <QMessageBox>

#include <Swiften/Elements/Whiteboard/WhiteboardDeleteOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardInsertOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardLineElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardRectElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardUpdateOperation.h>
#include <Swiften/Elements/WhiteboardPayload.h>
#include <Swiften/Whiteboard/WhiteboardSession.h>

#include <Swift/QtUI/Whiteboard/WhiteboardElementDrawingVisitor.h>

namespace Swift {
    QtWhiteboardWindow::QtWhiteboardWindow(WhiteboardSession::ref whiteboardSession) : QWidget() {
#ifndef Q_OS_MAC
#ifdef  Q_OS_WIN32
    setWindowIcon(QIcon(":/logo-icon-16-win.png"));
#else
    setWindowIcon(QIcon(":/logo-icon-16.png"));
#endif
#endif
        layout = new QVBoxLayout(this);
        hLayout = new QHBoxLayout;
        sidebarLayout = new QVBoxLayout;
        toolboxLayout = new QGridLayout;

        scene = new QGraphicsScene(this);
        scene->setSceneRect(0, 0, 400, 400);

        graphicsView = new GView(scene, this);
        graphicsView->setMode(GView::Line);
        connect(graphicsView, SIGNAL(lastItemChanged(QGraphicsItem*, int, GView::Type)), this, SLOT(handleLastItemChanged(QGraphicsItem*, int, GView::Type)));
        connect(graphicsView, SIGNAL(itemDeleted(QString, int)), this, SLOT(handleItemDeleted(QString, int)));

        widthBox = new QSpinBox(this);
        connect(widthBox, SIGNAL(valueChanged(int)), this, SLOT(changeLineWidth(int)));
        connect(graphicsView, SIGNAL(lineWidthChanged(int)), widthBox, SLOT(setValue(int)));
        widthBox->setValue(1);

        moveUpButton = new QPushButton("Move Up", this);
        connect(moveUpButton, SIGNAL(clicked()), graphicsView, SLOT(moveUpSelectedItem()));

        moveDownButton = new QPushButton("Move Down", this);
        connect(moveDownButton, SIGNAL(clicked()), graphicsView, SLOT(moveDownSelectedItem()));

        strokeLayout = new QHBoxLayout;
        strokeColor = new ColorWidget;
        strokeLayout->addWidget(new QLabel("Stroke:"));
        strokeLayout->addWidget(strokeColor);
        connect(strokeColor, SIGNAL(clicked()), this, SLOT(showColorDialog()));
        connect(graphicsView, SIGNAL(lineColorChanged(QColor)), strokeColor, SLOT(setColor(QColor)));

        fillLayout = new QHBoxLayout;
        fillColor = new ColorWidget;
        fillLayout->addWidget(new QLabel("Fill:"));
        fillLayout->addWidget(fillColor);
        connect(fillColor, SIGNAL(clicked()), this, SLOT(showBrushColorDialog()));
        connect(graphicsView, SIGNAL(brushColorChanged(QColor)), fillColor, SLOT(setColor(QColor)));

        rubberButton = new QToolButton(this);
        rubberButton->setIcon(QIcon(":/icons/eraser.png"));
        rubberButton->setCheckable(true);
        rubberButton->setAutoExclusive(true);
        connect(rubberButton, SIGNAL(clicked()), this, SLOT(setRubberMode()));

        lineButton = new QToolButton(this);
        lineButton->setIcon(QIcon(":/icons/line.png"));
        lineButton->setCheckable(true);
        lineButton->setAutoExclusive(true);
        lineButton->setChecked(true);
        connect(lineButton, SIGNAL(clicked()), this, SLOT(setLineMode()));

        rectButton = new QToolButton(this);
        rectButton->setIcon(QIcon(":/icons/rect.png"));
        rectButton->setCheckable(true);
        rectButton->setAutoExclusive(true);
        connect(rectButton, SIGNAL(clicked()), this, SLOT(setRectMode()));

        circleButton = new QToolButton(this);
        circleButton->setIcon(QIcon(":/icons/circle.png"));
        circleButton->setCheckable(true);
        circleButton->setAutoExclusive(true);
        connect(circleButton, SIGNAL(clicked()), this, SLOT(setCircleMode()));

        handLineButton = new QToolButton(this);
        handLineButton->setIcon(QIcon(":/icons/handline.png"));
        handLineButton->setCheckable(true);
        handLineButton->setAutoExclusive(true);
        connect(handLineButton, SIGNAL(clicked()), this, SLOT(setHandLineMode()));

        textButton = new QToolButton(this);
        textButton->setIcon(QIcon(":/icons/text.png"));
        textButton->setCheckable(true);
        textButton->setAutoExclusive(true);
        connect(textButton, SIGNAL(clicked()), this, SLOT(setTextMode()));

        polygonButton = new QToolButton(this);
        polygonButton->setIcon(QIcon(":/icons/polygon.png"));
        polygonButton->setCheckable(true);
        polygonButton->setAutoExclusive(true);
        connect(polygonButton, SIGNAL(clicked()), this, SLOT(setPolygonMode()));

        selectButton = new QToolButton(this);
        selectButton->setIcon(QIcon(":/icons/cursor.png"));
        selectButton->setCheckable(true);
        selectButton->setAutoExclusive(true);
        connect(selectButton, SIGNAL(clicked()), this, SLOT(setSelectMode()));

        toolboxLayout->addWidget(rubberButton, 0, 0);
        toolboxLayout->addWidget(selectButton, 0, 1);
        toolboxLayout->addWidget(lineButton, 0, 2);
        toolboxLayout->addWidget(circleButton, 1, 0);
        toolboxLayout->addWidget(handLineButton, 1, 1);
        toolboxLayout->addWidget(rectButton, 1, 2);
        toolboxLayout->addWidget(textButton, 2, 0);
        toolboxLayout->addWidget(polygonButton, 2, 1);

        sidebarLayout->addLayout(toolboxLayout);
        sidebarLayout->addSpacing(30);
        sidebarLayout->addWidget(moveUpButton);
        sidebarLayout->addWidget(moveDownButton);
        sidebarLayout->addSpacing(40);
        sidebarLayout->addWidget(widthBox);
        sidebarLayout->addLayout(strokeLayout);
        sidebarLayout->addLayout(fillLayout);
        sidebarLayout->addStretch();
        hLayout->addWidget(graphicsView);
        hLayout->addLayout(sidebarLayout);
        layout->addLayout(hLayout);
        this->setLayout(layout);

        setSession(whiteboardSession);
    }

    void QtWhiteboardWindow::handleWhiteboardOperationReceive(const WhiteboardOperation::ref operation) {
        WhiteboardInsertOperation::ref insertOp = std::dynamic_pointer_cast<WhiteboardInsertOperation>(operation);
        if (insertOp) {
            WhiteboardElementDrawingVisitor visitor(graphicsView, operation->getPos(), GView::New);
            insertOp->getElement()->accept(visitor);
        }

        WhiteboardUpdateOperation::ref updateOp = std::dynamic_pointer_cast<WhiteboardUpdateOperation>(operation);
        if (updateOp) {
            WhiteboardElementDrawingVisitor visitor(graphicsView, operation->getPos(), GView::Update);
            updateOp->getElement()->accept(visitor);
            if (updateOp->getPos() != updateOp->getNewPos()) {
                graphicsView->move(graphicsView->getItem(P2QSTRING(updateOp->getElement()->getID())), updateOp->getNewPos());
            }
        }

        WhiteboardDeleteOperation::ref deleteOp = std::dynamic_pointer_cast<WhiteboardDeleteOperation>(operation);
        if (deleteOp) {
            graphicsView->deleteItem(P2QSTRING(deleteOp->getElementID()));
        }
    }

    void QtWhiteboardWindow::changeLineWidth(int i)
    {
        graphicsView->setLineWidth(i);
    }

    void QtWhiteboardWindow::showColorDialog()
    {
        QColor color = QColorDialog::getColor(graphicsView->getLineColor(), nullptr, "Select pen color", QColorDialog::ShowAlphaChannel);
        if(color.isValid())
            graphicsView->setLineColor(color);
    }

    void QtWhiteboardWindow::showBrushColorDialog()
    {
        QColor color = QColorDialog::getColor(graphicsView->getBrushColor(), nullptr, "Select brush color", QColorDialog::ShowAlphaChannel);
        if(color.isValid())
            graphicsView->setBrushColor(color);
    }

    void QtWhiteboardWindow::setRubberMode()
    {
        graphicsView->setMode(GView::Rubber);
    }

    void QtWhiteboardWindow::setLineMode()
    {
        graphicsView->setMode(GView::Line);
    }

    void QtWhiteboardWindow::setRectMode()
    {
        graphicsView->setMode(GView::Rect);
    }

    void QtWhiteboardWindow::setCircleMode()
    {
        graphicsView->setMode(GView::Circle);
    }

    void QtWhiteboardWindow::setHandLineMode()
    {
        graphicsView->setMode(GView::HandLine);
    }

    void QtWhiteboardWindow::setTextMode()
    {
        graphicsView->setMode(GView::Text);
    }

    void QtWhiteboardWindow::setPolygonMode()
    {
        graphicsView->setMode(GView::Polygon);
    }

    void QtWhiteboardWindow::setSelectMode()
    {
        graphicsView->setMode(GView::Select);
    }

    void QtWhiteboardWindow::show()
    {
        QWidget::show();
    }

    void QtWhiteboardWindow::setSession(WhiteboardSession::ref session) {
        graphicsView->clear();
        whiteboardSession_ = session;
        whiteboardSession_->onOperationReceived.connect(boost::bind(&QtWhiteboardWindow::handleWhiteboardOperationReceive, this, _1));
        whiteboardSession_->onRequestAccepted.connect(boost::bind(&QWidget::show, this));
        whiteboardSession_->onSessionTerminated.connect(boost::bind(&QtWhiteboardWindow::handleSessionTerminate, this));
    }

    void QtWhiteboardWindow::activateWindow() {
        QWidget::activateWindow();
    }

    void QtWhiteboardWindow::setName(const std::string& name) {
        setWindowTitle(P2QSTRING(name));
    }

    void QtWhiteboardWindow::handleLastItemChanged(QGraphicsItem* item, int pos, GView::Type type) {
        WhiteboardElement::ref el;
        QGraphicsLineItem* lineItem = qgraphicsitem_cast<QGraphicsLineItem*>(item);
        if (lineItem != nullptr) {
            QLine line = lineItem->line().toLine();
            QColor color = lineItem->pen().color();
            WhiteboardLineElement::ref element = std::make_shared<WhiteboardLineElement>(line.x1()+lineItem->pos().x(), line.y1()+lineItem->pos().y(), line.x2()+lineItem->pos().x(), line.y2()+lineItem->pos().y());
            element->setColor(WhiteboardColor(color.red(), color.green(), color.blue(), color.alpha()));
            element->setPenWidth(lineItem->pen().width());

            element->setID(lineItem->data(100).toString().toStdString());
            el = element;
        }

        FreehandLineItem* freehandLineItem = qgraphicsitem_cast<FreehandLineItem*>(item);
        if (freehandLineItem != nullptr) {
            WhiteboardFreehandPathElement::ref element = std::make_shared<WhiteboardFreehandPathElement>();
            QColor color = freehandLineItem->pen().color();
            std::vector<std::pair<int, int> > points;
            QVector<QPointF>::const_iterator it = freehandLineItem->points().constBegin();
            for ( ; it != freehandLineItem->points().constEnd(); ++it) {
                points.push_back(std::pair<int, int>(
                            boost::numeric_cast<int>(it->x()+item->pos().x()),
                            boost::numeric_cast<int>(it->y()+item->pos().y())));
            }

            element->setColor(WhiteboardColor(color.red(), color.green(), color.blue(), color.alpha()));
            element->setPenWidth(freehandLineItem->pen().width());
            element->setPoints(points);

            element->setID(freehandLineItem->data(100).toString().toStdString());
            el = element;
        }

        QGraphicsRectItem* rectItem = qgraphicsitem_cast<QGraphicsRectItem*>(item);
        if (rectItem != nullptr) {
            QRectF rect = rectItem->rect();
            WhiteboardRectElement::ref element = std::make_shared<WhiteboardRectElement>(rect.x()+item->pos().x(), rect.y()+item->pos().y(), rect.width(), rect.height());
            QColor penColor = rectItem->pen().color();
            QColor brushColor = rectItem->brush().color();

            element->setBrushColor(WhiteboardColor(brushColor.red(), brushColor.green(), brushColor.blue(), brushColor.alpha()));
            element->setPenColor(WhiteboardColor(penColor.red(), penColor.green(), penColor.blue(), penColor.alpha()));
            element->setPenWidth(rectItem->pen().width());

            element->setID(rectItem->data(100).toString().toStdString());
            el = element;
        }

        QGraphicsTextItem* textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item);
        if (textItem != nullptr) {
            QPointF point = textItem->pos();
            WhiteboardTextElement::ref element = std::make_shared<WhiteboardTextElement>(point.x(), point.y());
            element->setText(textItem->toPlainText().toStdString());
            element->setSize(textItem->font().pointSize());
            QColor color = textItem->defaultTextColor();
            element->setColor(WhiteboardColor(color.red(), color.green(), color.blue(), color.alpha()));

            element->setID(textItem->data(100).toString().toStdString());
            el = element;
        }

        QGraphicsPolygonItem* polygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
        if (polygonItem) {
            WhiteboardPolygonElement::ref element = std::make_shared<WhiteboardPolygonElement>();
            QPolygonF polygon = polygonItem->polygon();
            std::vector<std::pair<int, int> > points;
            QVector<QPointF>::const_iterator it = polygon.begin();
            for (; it != polygon.end(); ++it) {
                points.push_back(std::pair<int, int>(
                            boost::numeric_cast<int>(it->x()+item->pos().x()),
                            boost::numeric_cast<int>(it->y()+item->pos().y())));
            }

            element->setPoints(points);

            QColor penColor = polygonItem->pen().color();
            QColor brushColor = polygonItem->brush().color();
            element->setPenColor(WhiteboardColor(penColor.red(), penColor.green(), penColor.blue(), penColor.alpha()));
            element->setBrushColor(WhiteboardColor(brushColor.red(), brushColor.green(), brushColor.blue(), brushColor.alpha()));
            element->setPenWidth(polygonItem->pen().width());

            element->setID(polygonItem->data(100).toString().toStdString());
            el = element;
        }

        QGraphicsEllipseItem* ellipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
        if (ellipseItem) {
            QRectF rect = ellipseItem->rect();
            int cx = boost::numeric_cast<int>(rect.x()+rect.width()/2 + item->pos().x());
            int cy = boost::numeric_cast<int>(rect.y()+rect.height()/2 + item->pos().y());
            int rx = boost::numeric_cast<int>(rect.width()/2);
            int ry = boost::numeric_cast<int>(rect.height()/2);
            WhiteboardEllipseElement::ref element = std::make_shared<WhiteboardEllipseElement>(cx, cy, rx, ry);

            QColor penColor = ellipseItem->pen().color();
            QColor brushColor = ellipseItem->brush().color();
            element->setPenColor(WhiteboardColor(penColor.red(), penColor.green(), penColor.blue(), penColor.alpha()));
            element->setBrushColor(WhiteboardColor(brushColor.red(), brushColor.green(), brushColor.blue(), brushColor.alpha()));
            element->setPenWidth(ellipseItem->pen().width());

            element->setID(ellipseItem->data(100).toString().toStdString());
            el = element;
        }

        if (type == GView::New) {
            WhiteboardInsertOperation::ref insertOp = std::make_shared<WhiteboardInsertOperation>();
            insertOp->setPos(pos);
            insertOp->setElement(el);
            whiteboardSession_->sendOperation(insertOp);
        } else {
            WhiteboardUpdateOperation::ref updateOp = std::make_shared<WhiteboardUpdateOperation>();
            updateOp->setPos(pos);
            if (type == GView::Update) {
                updateOp->setNewPos(pos);
            } else if (type == GView::MoveUp) {
                updateOp->setNewPos(pos+1);
            } else if (type == GView::MoveDown) {
                updateOp->setNewPos(pos-1);
            }
            updateOp->setElement(el);
            whiteboardSession_->sendOperation(updateOp);
        }
    }

    void QtWhiteboardWindow::handleItemDeleted(QString id, int pos) {
        WhiteboardDeleteOperation::ref deleteOp = std::make_shared<WhiteboardDeleteOperation>();
        deleteOp->setElementID(Q2PSTRING(id));
        deleteOp->setPos(pos);
        whiteboardSession_->sendOperation(deleteOp);
    }

    void QtWhiteboardWindow::handleSessionTerminate() {
        hide();
    }

    void QtWhiteboardWindow::closeEvent(QCloseEvent* event) {
        QMessageBox box(this);
        box.setText(tr("Closing window is equivalent closing the session. Are you sure you want to do this?"));
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        box.setIcon(QMessageBox::Question);
        if (box.exec() == QMessageBox::Yes) {
            whiteboardSession_->cancel();
        } else {
            event->ignore();
        }
    }
}
