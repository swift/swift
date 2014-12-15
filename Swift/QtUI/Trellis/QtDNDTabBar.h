/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QTabBar>

namespace Swift {

class QtDNDTabBar : public QTabBar {
	Q_OBJECT
	public:
		explicit QtDNDTabBar(QWidget* parent = 0);
		virtual ~QtDNDTabBar();

		int getDragIndex() const;
		QString getDragText() const;
		QWidget* getDragWidget() const;

		virtual QSize sizeHint() const;

	signals:
		void onDropSucceeded();

	protected:
		void dragEnterEvent(QDragEnterEvent* dragEnterEvent);
		void dropEvent(QDropEvent* dropEvent);
		bool event(QEvent* event);

	private:
		int defaultTabHeight;
		int dragIndex;
		QString dragText;
		QWidget* dragWidget;
};

}
