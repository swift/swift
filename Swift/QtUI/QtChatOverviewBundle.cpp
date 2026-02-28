/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtChatOverviewBundle.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include <Swiften/Elements/StatusShow.h>

#include <Swift/QtUI/ChattablesModel.h>
#include <Swift/QtUI/QtChatOverviewDelegate.h>
#include <Swift/QtUI/QtClickableLabel.h>
#include <Swift/QtUI/QtExpandedListView.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

BundleFilter::BundleFilter(QObject* parent) : QSortFilterProxyModel(parent) {
    sort(0, Qt::AscendingOrder);
    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
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
    if (!hideWhenEmpty_) {
        filterLabel_ = new QtClickableLabel(this);
        filterLabel_->setText(tr("Online"));
        filterLabel_->setStyleSheet("color: white;");
        headerLayout->addWidget(filterLabel_);
        connect(filterLabel_, SIGNAL(clicked()), this, SLOT(handleFilterClicked()));
    }
    listView_ = new QtExpandedListView(this);
    listView_->setModel(proxyModel_);
    listView_->setFrameStyle(QFrame::NoFrame);
    listView_->setItemDelegate(new QtChatOverviewDelegate(this));
    connect(listView_, SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleItemClicked(const QModelIndex&)));
    mainLayout->addWidget(listView_);

    if (hideWhenEmpty_) {
        connect(proxyModel_, &QAbstractItemModel::modelReset, this, [&](){
            updateVisibility();
        });
        connect(proxyModel_, &QAbstractItemModel::rowsInserted, this, [&](){
            updateVisibility();
        });
        connect(proxyModel_, &QAbstractItemModel::rowsRemoved, this, [&](){
            updateVisibility();
        });
        updateVisibility();
    }
}

QtChatOverviewBundle::~QtChatOverviewBundle() {}

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

void QtChatOverviewBundle::updateVisibility() {
    auto shouldBeVisible = (proxyModel_->rowCount(listView_->rootIndex()) > 0);
    setVisible(shouldBeVisible);
}


void QtChatOverviewBundle::handleItemClicked(const QModelIndex& index) {
    clicked(JID(Q2PSTRING(index.data(ChattablesModel::JIDRole).toString())));
}

} // namespace Swift
