/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#include <Swift/QtUI/QtWebView.h>

#include <boost/numeric/conversion/cast.hpp>

#include <QFocusEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMenu>

#include <Swiften/Base/Log.h>

namespace Swift {
QtWebView::QtWebView(QWidget* parent) : QWebView(parent), fontSizeIsMinimal(false) {
    setRenderHint(QPainter::SmoothPixmapTransform);
    filteredActions.push_back(QWebPage::CopyLinkToClipboard);
    filteredActions.push_back(QWebPage::CopyImageToClipboard);
    filteredActions.push_back(QWebPage::Copy);
    if (Log::getLogLevel() == Log::debug) {
        filteredActions.push_back(QWebPage::InspectElement);
    }
}

void QtWebView::keyPressEvent(QKeyEvent* event) {
    Qt::KeyboardModifiers modifiers = event->modifiers();
    int key = event->key();
    if (event->matches(QKeySequence::ZoomIn) || (key == Qt::Key_Equal && (modifiers & Qt::ControlModifier))) {
        event->accept();
        emit fontGrowRequested();
        return;
    }
    if (event->matches(QKeySequence::ZoomOut) || (key == Qt::Key_Minus && (modifiers & Qt::ControlModifier))) {
        event->accept();
        emit fontShrinkRequested();
        return;
    }
    if (modifiers == Qt::ShiftModifier && (key == Qt::Key_PageUp || key == Qt::Key_PageDown)) {
        modifiers = Qt::NoModifier;
    }
    QKeyEvent* translatedEvent = new QKeyEvent(QEvent::KeyPress,
                               key,
                               modifiers,
                               event->text(),
                               event->isAutoRepeat(),
                               boost::numeric_cast<unsigned short>(event->count()));
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
    for (auto action : actions) {
        bool removeAction = true;
        for(auto& filteredAction : filteredActions) {
            if (action == pageAction(filteredAction)) {
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
    menu->addAction(tr("Increase font size"), this, SIGNAL(fontGrowRequested()), QKeySequence(QKeySequence::ZoomIn));
    QAction* shrink = new QAction(tr("Decrease font size"), this);
    shrink->setShortcut(QKeySequence(QKeySequence::ZoomOut));
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
