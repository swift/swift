/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/WhiteboardWindow.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Whiteboard/WhiteboardSession.h>
#include <Swiften/Elements/Whiteboard/WhiteboardElement.h>

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QPushButton>
#include <QSpinBox>
#include <QColorDialog>
#include <QToolButton>
#include <QCloseEvent>

#include "GView.h"
#include "ColorWidget.h"

namespace Swift {
	class QtWhiteboardWindow : public QWidget, public WhiteboardWindow
	{
		Q_OBJECT
	public:
		QtWhiteboardWindow(WhiteboardSession::ref whiteboardSession);
		void show();
		void setSession(WhiteboardSession::ref session);
		void activateWindow();
		void setName(const std::string& name);

	private slots:
		void changeLineWidth(int i);
		void showColorDialog();
		void showBrushColorDialog();
		void setRubberMode();
		void setLineMode();
		void setRectMode();
		void setCircleMode();
		void setHandLineMode();
		void setTextMode();
		void setPolygonMode();
		void setSelectMode();
		void handleLastItemChanged(QGraphicsItem* item, int pos, GView::Type type);
		void handleItemDeleted(QString id, int pos);

	private:
		void handleSessionTerminate();
		void handleWhiteboardOperationReceive(const WhiteboardOperation::ref operation);
		void closeEvent(QCloseEvent* event);

	private:
		QGraphicsScene* scene;
		GView* graphicsView;
		QVBoxLayout* layout;
		QVBoxLayout* sidebarLayout;
		QHBoxLayout* hLayout;
		QGridLayout* toolboxLayout;
		QHBoxLayout* strokeLayout;
		QHBoxLayout* fillLayout;
		ColorWidget* strokeColor;
		ColorWidget* fillColor;
		QWidget* widget;
		QPushButton* moveUpButton;
		QPushButton* moveDownButton;
		QSpinBox* widthBox;
		QToolButton* rubberButton;
		QToolButton* lineButton;
		QToolButton* rectButton;
		QToolButton* circleButton;
		QToolButton* handLineButton;
		QToolButton* textButton;
		QToolButton* polygonButton;
		QToolButton* selectButton;

		std::string lastOpID;
		WhiteboardSession::ref whiteboardSession_;
	};
}
