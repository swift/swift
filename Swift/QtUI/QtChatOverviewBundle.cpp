/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtChatOverviewBundle.h>

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QPalette>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include <Swiften/Elements/StatusShow.h>

#include <Swift/QtUI/ChattablesModel.h>
#include <Swift/QtUI/QtChatOverviewDelegate.h>
#include <Swift/QtUI/QtClickableLabel.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

BundleFilter::BundleFilter(QObject* parent) : QSortFilterProxyModel(parent) {
    sort(0, Qt::AscendingOrder);
    setDynamicSortFilter(true);
}

void BundleFilter::addFilter(Filter filter) {
    filters_.emplace(filter);
    invalidateFilter();
}

bool BundleFilter::hasFilter(Filter filter) {
    return filters_.count(filter) > 0;
}

void BundleFilter::removeFilter(Filter filter) {
    filters_.erase(filter);
    invalidateFilter();
}

bool BundleFilter::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
    auto row = sourceModel()->index(sourceRow, 0, sourceParent);
    if (filters_.count(Filter::Unread)) {
        if (row.data(ChattablesModel::UnreadCountRole).toInt() == 0) {
            return false;
        }
    }
    if (filters_.count(Filter::People)) {
        if (row.data(ChattablesModel::TypeRole).toString() != "PERSON") {
            return false;
        }
    }
    if (filters_.count(Filter::Rooms)) {
        if (row.data(ChattablesModel::TypeRole).toString() != "ROOM") {
            return false;
        }
    }
    if (filters_.count(Filter::Online)) {
        if (static_cast<StatusShow::Type>(row.data(ChattablesModel::StatusRole).toInt()) == StatusShow::None) {
            return false;
        }
    }
    return true;
}

QtChatOverviewBundle::QtChatOverviewBundle(ChattablesModel* rootModel, QString name, bool hideWhenEmpty, QWidget* parent) : QWidget(parent), rootModel_(rootModel), hideWhenEmpty_(hideWhenEmpty) {
    proxyModel_ = new BundleFilter(this);
    if (name == "UNREAD") { // FIXME: Obviously needs a better approach
        proxyModel_->addFilter(BundleFilter::Filter::Unread);
    }
    if (name == "PEOPLE") {
        proxyModel_->addFilter(BundleFilter::Filter::People);
        proxyModel_->addFilter(BundleFilter::Filter::Online);
    }
    if (name == "ROOMS") {
        proxyModel_->addFilter(BundleFilter::Filter::Rooms);
        proxyModel_->addFilter(BundleFilter::Filter::Online);
    }
    proxyModel_->setSourceModel(rootModel);


    auto mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    auto headerLayout = new QHBoxLayout();
    mainLayout->addLayout(headerLayout);
    auto nameLabel = new QLabel(name, this);
    nameLabel->setStyleSheet("color: white;");
    headerLayout->addWidget(nameLabel);
    headerLayout->addStretch();
    if (!hideWhenEmpty) {
        filterLabel_ = new QtClickableLabel(this);
        filterLabel_->setText(tr("Online"));
        filterLabel_->setStyleSheet("color: white;");
        headerLayout->addWidget(filterLabel_);
        connect(filterLabel_, SIGNAL(clicked()), this, SLOT(handleFilterClicked()));
    }
    listView_ = new QListView(this);
    listView_->setModel(proxyModel_);
    listView_->setFrameStyle(QFrame::NoFrame);
    listView_->setItemDelegate(new QtChatOverviewDelegate(this));
    connect(listView_, SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleItemClicked(const QModelIndex&)));
    recalculateSize();
    mainLayout->addWidget(listView_);
    connect(proxyModel_, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(recalculateSize()));
    connect(proxyModel_, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), this, SLOT(recalculateSize()));
    connect(proxyModel_, SIGNAL(modelReset()), this, SLOT(recalculateSize()));
}

QtChatOverviewBundle::~QtChatOverviewBundle() {}

void QtChatOverviewBundle::recalculateSize() {
    int totalHeight = 0;
    for (int i = 0; i < proxyModel_->rowCount(); i++) {
        totalHeight += listView_->sizeHintForRow(i);
    }
    listView_->setFixedHeight(totalHeight);
    if (hideWhenEmpty_ && totalHeight == 0) {
        hide();
    }
    else {
        show();
    }
}

void QtChatOverviewBundle::handleFilterClicked() {
    if (proxyModel_->hasFilter(BundleFilter::Filter::Online)) {
        proxyModel_->removeFilter(BundleFilter::Filter::Online);
        filterLabel_->setText(tr("All"));
    }
    else {
        proxyModel_->addFilter(BundleFilter::Filter::Online);
        filterLabel_->setText(tr("Online"));
    }
}

void QtChatOverviewBundle::handleItemClicked(const QModelIndex& index) {
    clicked(JID(Q2PSTRING(index.data(ChattablesModel::JIDRole).toString())));
}

} // namespace Swift
