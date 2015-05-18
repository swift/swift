/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QGridLayout>
#include <QHash>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QWidget>

namespace Swift {
	class QtTabbable;
	class QtTabWidget;

	class QtDynamicGridLayout : public QWidget {
		Q_OBJECT
	public:
		explicit QtDynamicGridLayout(QWidget* parent = 0, bool enableDND = false);
		virtual ~QtDynamicGridLayout();

		QSize getDimension() const;

		// emulate QtTabWidget API
		int addTab(QtTabbable* tab, const QString& title);
		void removeTab(int index);
		int count() const;

		QWidget* widget(int index) const;
		QWidget* currentWidget() const;
		void setCurrentWidget(QWidget* widget);

		QtTabWidget* indexToTabWidget(int index, int& tabIndex);

		int indexOf(const QWidget* widget) const;
		int currentIndex() const;
		void setCurrentIndex(int index);

		bool isDNDEnabled() const;

		QHash<QString, QPoint> getTabPositions() const;
		void setTabPositions(const QHash<QString, QPoint> positions);

		bool eventFilter(QObject* object, QEvent* event);

	signals:
		void tabCloseRequested(int index);
		void onCurrentIndexChanged(int newIndex);

	public slots:
		void setDimensions(const QSize& dim);

		// Tab Management
		void moveCurrentTabRight();
		void moveCurrentTabLeft();
		void moveCurrentTabToNextGroup();
		void moveCurrentTabToPreviousGroup();

		void updateTabPositions();

	private slots:
		void handleTabCloseRequested(int index);
		void handleTabCurrentChanged(int index);
		void handleApplicationFocusChanged(QWidget* oldFocus, QWidget* newFocus);

	private:
		void moveTab(QtTabWidget* tabWidget, int oldIndex, int newIndex);
		QtTabWidget* createDNDTabWidget(QWidget* parent);

	private:
		QGridLayout *gridLayout_;
		bool dndEnabled_;
		QHash<QString, QPoint> tabPositions_;
		QtTabbable* movingTab_;
	};
}
