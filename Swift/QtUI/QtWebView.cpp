/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#include "QtWebView.h"

#include <QKeyEvent>
#include <QFocusEvent>
#include <QMenu>

namespace Swift {
QtWebView::QtWebView(QWidget* parent) : QWebView(parent), fontSizeIsMinimal(false) {
	setRenderHint(QPainter::SmoothPixmapTransform);
	filteredActions.push_back(QWebPage::CopyLinkToClipboard);
	filteredActions.push_back(QWebPage::CopyImageToClipboard);
	filteredActions.push_back(QWebPage::Copy);
}

void QtWebView::keyPressEvent(QKeyEvent* event) {
	Qt::KeyboardModifiers modifiers = event->modifiers();
	int key = event->key();
	if (modifiers == Qt::ShiftModifier && (key == Qt::Key_PageUp || key == Qt::Key_PageDown)) {
		modifiers = Qt::NoModifier;
	}
	QKeyEvent* translatedEvent = new QKeyEvent(QEvent::KeyPress,
							   key,
							   modifiers,
							   event->text(),
							   event->isAutoRepeat(),
							   event->count());
	QWebView::keyPressEvent(translatedEvent);
	delete translatedEvent;
}

void QtWebView::dragEnterEvent(QDragEnterEvent*) {

}

void QtWebView::setFontSizeIsMinimal(bool minimum) {
	fontSizeIsMinimal = minimum;
}

void QtWebView::contextMenuEvent(QContextMenuEvent* ev) {
	// Filter out the relevant actions from the standard actions
	
	QMenu* menu = page()->createStandardContextMenu();
	QList<QAction*> actions(menu->actions());
	for (int i = 0; i < actions.size(); ++i) {
		QAction* action = actions.at(i);
		bool removeAction = true;
		for(size_t j = 0; j < filteredActions.size(); ++j) {
			if (action == pageAction(filteredActions[j])) {
				removeAction = false;
				break;
			}
		}
		if (removeAction) {
			menu->removeAction(action);
		}
	}

	// Add our own custom actions
	menu->addAction(tr("Clear"), this, SIGNAL(clearRequested()));
	menu->addAction(tr("Increase font size"), this, SIGNAL(fontGrowRequested()));
	QAction* shrink = new QAction(tr("Decrease font size"), this);
	shrink->setEnabled(!fontSizeIsMinimal);
	connect(shrink, SIGNAL(triggered()), this, SIGNAL(fontShrinkRequested()));
	menu->addAction(shrink);

	menu->exec(ev->globalPos());
	delete menu;
}

void QtWebView::focusInEvent(QFocusEvent* event) {
	QWebView::focusInEvent(event);
	emit gotFocus();
}

}
