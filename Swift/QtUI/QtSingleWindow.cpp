/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtSingleWindow.h>

#include <QPushButton>
#include <QVBoxLayout>

#include <Swiften/Base/Platform.h>

#include <Swift/QtUI/QtChatTabs.h>
#include <Swift/QtUI/QtLoginWindow.h>
#include <Swift/QtUI/QtSettingsProvider.h>

namespace Swift {

static const QString SINGLE_WINDOW_GEOMETRY = QString("SINGLE_WINDOW_GEOMETRY");
static const QString SINGLE_WINDOW_SPLITS = QString("SINGLE_WINDOW_SPLITS");

QtSingleWindow::QtSingleWindow(QtSettingsProvider* settings) : QSplitter() {
    settings_ = settings;
    auto geometryVariant = settings_->getQSettings()->value(SINGLE_WINDOW_GEOMETRY);
    if (geometryVariant.isValid()) {
        restoreGeometry(geometryVariant.toByteArray());
    }
    connect(this, SIGNAL(splitterMoved(int, int)), this, SLOT(handleSplitterMoved(/*int, int*/)));
    setChildrenCollapsible(false);

    auto left = new QWidget(this);
    list_ = new QListWidget(left);
    auto addButton = new QPushButton("+", left);
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(list_);
    leftLayout->addWidget(addButton);
    left->setLayout(leftLayout);
    QSplitter::addWidget(left);
    loginWindows_ = new QStackedWidget(this);
    QSplitter::addWidget(loginWindows_);
    tabs_ = new QStackedWidget(this);
    QSplitter::addWidget(tabs_);
    restoreSplitters();
    setStretchFactor(0, 0);
    setStretchFactor(1, 0);
    setStretchFactor(2, 1);
    connect(list_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleListItemClicked(QListWidgetItem*)));
    connect(addButton, SIGNAL(clicked()), this, SIGNAL(wantsToAddAccount()));
#ifdef SWIFTEN_PLATFORM_MACOSX
    setHandleWidth(0);
#endif
}

QtSingleWindow::~QtSingleWindow() {

}

void QtSingleWindow::handleTabsTitleChanged(const QString& title) {
    setWindowTitle(title);
}

void QtSingleWindow::handleSplitterMoved() {
    QList<QVariant> variantValues;
    QList<int> intValues = sizes();
    for (const auto& value : intValues) {
        variantValues.append(QVariant(value));
    }
    settings_->getQSettings()->setValue(SINGLE_WINDOW_SPLITS, QVariant(variantValues));
}

void QtSingleWindow::restoreSplitters() {
    auto splitsVariant = settings_->getQSettings()->value(SINGLE_WINDOW_SPLITS);
    if (splitsVariant.isValid()) {
        auto variantValues = splitsVariant.toList();
        QList<int> intValues;
        for (auto&& value : variantValues) {
            intValues.append(value.toInt());
        }
        setSizes(intValues);
    }
    else {
        handleSplitterMoved();
    }
}

void QtSingleWindow::handleGeometryChanged() {
    settings_->getQSettings()->setValue(SINGLE_WINDOW_GEOMETRY, saveGeometry());
}

void QtSingleWindow::resizeEvent(QResizeEvent* event) {
    handleGeometryChanged();
    QSplitter::resizeEvent(event);
}

void QtSingleWindow::moveEvent(QMoveEvent*) {
    handleGeometryChanged();
}

void QtSingleWindow::addAccount(QtLoginWindow* loginWindow, QtChatTabs* tabs) {
    if (!loginWindows_->count()) {
        connect(tabs, SIGNAL(onTitleChanged(const QString&)), this, SLOT(handleTabsTitleChanged(const QString&)));
    }
    loginWindows_->addWidget(loginWindow);
    tabs_->addWidget(tabs);
    list_->addItem(QString("Account %1").arg(loginWindows_->count()));
}

void QtSingleWindow::handleListItemClicked(QListWidgetItem* /*item*/) {
    //FIXME: Should use a full model/view and do this properly (and render pretty things ourselves too)
    auto currentTabs = tabs_->widget(tabs_->currentIndex());
    disconnect(currentTabs, SIGNAL(onTitleChanged(const QString&)), this, SLOT(handleTabsTitleChanged(const QString&)));
    loginWindows_->setCurrentIndex(list_->currentRow());
    tabs_->setCurrentIndex(list_->currentRow());
    currentTabs = tabs_->widget(tabs_->currentIndex());
    connect(currentTabs, SIGNAL(onTitleChanged(const QString&)), this, SLOT(handleTabsTitleChanged(const QString&)));
    //TODO change the title of the window.
    handleTabsTitleChanged(QString("Swift"));
}

}
