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
#include <Swift/QtUI/ServerList/QtServerListView.h>
#include <Swift/QtUI/ServerList/ServerListModel.h>

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
    serverList_ = new QtServerListView();
    serverListModel_ = new ServerListModel();
    serverList_->setModel(serverListModel_);
    serverListModel_->setModelData(&accountData_);
    accountData_.onDataChanged.connect(boost::bind(&ServerListModel::handleDataChanged, serverListModel_));
    auto addButton = new QPushButton("+", left);
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(serverList_);
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
    connect(serverList_, &QtServerListView::clicked, this, &QtSingleWindow::handleListItemClicked);
    connect(addButton, &QPushButton::clicked, this, &QtSingleWindow::wantsToAddAccount);
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
    loginWindows_->addWidget(loginWindow);
    tabs_->addWidget(tabs);
    std::string account = QString("Account %1").arg(loginWindows_->count()).toStdString();
    accountData_.addAccount(account);
    emit serverListModel_->layoutChanged();
}

void QtSingleWindow::handleListItemClicked(const QModelIndex& item) {
    auto currentTabs = tabs_->widget(tabs_->currentIndex());
    disconnect(currentTabs, SIGNAL(onTitleChanged(const QString&)), this, SLOT(handleTabsTitleChanged(const QString&)));
    loginWindows_->setCurrentIndex(item.row());
    tabs_->setCurrentIndex(item.row());
    currentTabs = tabs_->widget(tabs_->currentIndex());
    connect(currentTabs, SIGNAL(onTitleChanged(const QString&)), this, SLOT(handleTabsTitleChanged(const QString&)));
    //TODO change the title of the window.
    handleTabsTitleChanged(QString("Swift"));

}

}
