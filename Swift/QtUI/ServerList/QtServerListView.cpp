/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/ServerList/QtServerListView.h>

namespace Swift {

QtServerListView::QtServerListView() {
    QPalette newPalette = palette();
    //TODO move color and theme variables to a shared location.
    newPalette.setColor(QPalette::Base, { 38, 81, 112 });
    setAutoFillBackground(true);
    setPalette(newPalette);
    delegate_ = std::make_unique<ServerListDelegate>();
    setItemDelegate(delegate_.get());
    setMaximumWidth(widgetWidth);
    setMinimumWidth(widgetWidth);
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSelectionMode(QAbstractItemView::NoSelection);
}

QtServerListView::~QtServerListView() {

}

}
