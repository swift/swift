/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/Roster/QtTreeWidget.h>

#include <memory>

#include <boost/bind.hpp>

#include <QFont>
#include <QLabel>
#include <QMimeData>
#include <QObject>
#include <QTimer>
#include <QToolTip>
#include <QUrl>

#include <Swiften/Base/Platform.h>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/UIEvents/SendFileUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/QtUI/Roster/RosterModel.h>

namespace Swift {

QtTreeWidget::QtTreeWidget(UIEventStream* eventStream, SettingsProvider* settings, MessageTarget messageTarget, QWidget* parent) : QTreeView(parent),  eventStream_(eventStream), settings_(settings), messageTarget_(messageTarget) {
    model_ = new RosterModel(this, settings_->getSetting(QtUISettingConstants::USE_SCREENREADER));
    setModel(model_);
    delegate_ = new RosterDelegate(this, settings_->getSetting(QtUISettingConstants::COMPACT_ROSTER));
    setItemDelegate(delegate_);
    setHeaderHidden(true);
#ifdef SWIFT_PLATFORM_MACOSX
    setAlternatingRowColors(true);
#endif
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    expandAll();
    setAnimated(true);
    setIndentation(0);
#ifdef SWIFT_EXPERIMENTAL_FT
    setAcceptDrops(true);
#endif
    setDragEnabled(true);
    setRootIsDecorated(true);
    connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleItemActivated(const QModelIndex&)));
    connect(model_, SIGNAL(itemExpanded(const QModelIndex&, bool)), this, SLOT(handleModelItemExpanded(const QModelIndex&, bool)));
    connect(this, SIGNAL(expanded(const QModelIndex&)), this, SLOT(handleExpanded(const QModelIndex&)));
    connect(this, SIGNAL(collapsed(const QModelIndex&)), this, SLOT(handleCollapsed(const QModelIndex&)));
    connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleClicked(const QModelIndex&)));

    settings_->onSettingChanged.connect(boost::bind(&QtTreeWidget::handleSettingChanged, this, _1));

    QFont lato = font();
    lato.setFamily("Lato");
    setFont(lato);
}

QtTreeWidget::~QtTreeWidget() {
    settings_->onSettingChanged.disconnect(boost::bind(&QtTreeWidget::handleSettingChanged, this, _1));
    delete model_;
    delete delegate_;
}

void QtTreeWidget::handleSettingChanged(const std::string& setting) {
    if (setting == QtUISettingConstants::COMPACT_ROSTER.getKey()) {
        delegate_->setCompact(settings_->getSetting(QtUISettingConstants::COMPACT_ROSTER));
        repaint();
    }
}

void QtTreeWidget::handleRefreshTooltip() {
    if (tooltipShown_) {
        QPoint position = QCursor::pos();
        QModelIndex index = indexAt(mapFromGlobal(position));
        QToolTip::showText(position, model_->data(index, Qt::ToolTipRole).toString());
    }
}

void QtTreeWidget::setRosterModel(Roster* roster) {
    roster_ = roster;
    model_->setRoster(roster);
    expandAll();
}

void QtTreeWidget::refreshTooltip() {
    // Qt needs some time to emit the events we need to detect tooltip's visibility correctly; 20 ms should be enough
    QTimer::singleShot(20, this, SLOT(handleRefreshTooltip()));
}

QtTreeWidgetItem* QtTreeWidget::getRoot() {
    return treeRoot_;
}

void QtTreeWidget::handleClicked(const QModelIndex& index) {
    GroupRosterItem* item = dynamic_cast<GroupRosterItem*>(static_cast<RosterItem*>(index.internalPointer()));
    if (item) {
        setExpanded(index, !isExpanded(index));
    }
    currentChanged(index, QModelIndex());
}

QModelIndexList QtTreeWidget::getSelectedIndexes() const {
    // Not using selectedIndexes(), because this seems to cause a crash in Qt (4.7.0) in the
    // QModelIndexList destructor.
    // This is a workaround posted in http://www.qtcentre.org/threads/16933 (although this case
    // was resolved by linking against the debug libs, ours isn't, and we're not alone)
    QItemSelection ranges = selectionModel()->selection();
    QModelIndexList selectedIndexList;
    for (int i = 0; i < ranges.count(); ++i) {
        QModelIndex parent = ranges.at(i).parent();
        int right = ranges.at(i).model()->columnCount(parent) - 1;
        if (ranges.at(i).left() == 0 && ranges.at(i).right() == right) {
            for (int r = ranges.at(i).top(); r <= ranges.at(i).bottom(); ++r) {
                selectedIndexList.append(ranges.at(i).model()->index(r, 0, parent));
            }
        }
    }
    return selectedIndexList;
}

void QtTreeWidget::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
    RosterItem* item = nullptr;
    QModelIndexList selectedIndexList = getSelectedIndexes();
    if (selectedIndexList.empty() || !selectedIndexList[0].isValid()) {
        /* I didn't quite understand why using current didn't seem to work here.*/
    }
    else if (current.isValid()) {
        item = static_cast<RosterItem*>(current.internalPointer());
        item = dynamic_cast<ContactRosterItem*>(item);
    }
    onSomethingSelectedChanged(item);
    QTreeView::currentChanged(current, previous);
}

void QtTreeWidget::handleItemActivated(const QModelIndex& index) {
    JID target = jidFromIndex(index);
    if (target.isValid()) {
        eventStream_->send(std::make_shared<RequestChatUIEvent>(target));
    }
}

void QtTreeWidget::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1) {
        event->acceptProposedAction();
    }
}

void QtTreeWidget::dropEvent(QDropEvent *event) {
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
        if (ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item)) {
            if (contact->supportsFeature(ContactRosterItem::FileTransferFeature)) {
                QString filename = event->mimeData()->urls().at(0).toLocalFile();
                if (!filename.isEmpty()) {
                    eventStream_->send(std::make_shared<SendFileUIEvent>(contact->getJID(), Q2PSTRING(filename)));
                }
            }
        }
    }
}

void QtTreeWidget::dragMoveEvent(QDragMoveEvent* event) {
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
        if (ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item)) {
            if (contact->supportsFeature(ContactRosterItem::FileTransferFeature)) {
                event->accept();
                return;
            }
        }
    }
    QTreeView::dragMoveEvent(event);
}

bool QtTreeWidget::event(QEvent* event) {
    QChildEvent* childEvent = nullptr;
    if ((childEvent = dynamic_cast<QChildEvent*>(event))) {
        if (childEvent->polished()) {
            if (dynamic_cast<QLabel*>(childEvent->child())) {
                tooltipShown_ = true;
            }
        }
        else if (childEvent->removed()) {
            if (childEvent->child()->objectName() == "qtooltip_label") {
                tooltipShown_ = false;
            }
        }
    }
    return QAbstractItemView::event(event);
}

void QtTreeWidget::handleExpanded(const QModelIndex& index) {
    GroupRosterItem* item = dynamic_cast<GroupRosterItem*>(static_cast<RosterItem*>(index.internalPointer()));
    if (item) {
        item->setExpanded(true);
    }
}

void QtTreeWidget::handleCollapsed(const QModelIndex& index) {
    GroupRosterItem* item = dynamic_cast<GroupRosterItem*>(static_cast<RosterItem*>(index.internalPointer()));
    if (item) {
        item->setExpanded(false);
    }
}

void QtTreeWidget::handleModelItemExpanded(const QModelIndex& index, bool shouldExpand) {
    if (!index.isValid()) {
        return;
    }
    bool alreadyRight = this->isExpanded(index) == shouldExpand;
    if (alreadyRight) {
        return;
    }
    setExpanded(index, shouldExpand);
}

void QtTreeWidget::drawBranches(QPainter*, const QRect&, const QModelIndex&) const {
}

void QtTreeWidget::show() {
    QWidget::show();
}

void QtTreeWidget::setMessageTarget(MessageTarget messageTarget) {
    messageTarget_ = messageTarget;
}

JID QtTreeWidget::jidFromIndex(const QModelIndex& index) const {
    JID target;
    if (messageTarget_ == MessageDisplayJID) {
        target = JID(Q2PSTRING(index.data(DisplayJIDRole).toString()));
    }
    if (!target.isValid()) {
        target = JID(Q2PSTRING(index.data(JIDRole).toString()));
    }
    return target;
}

JID QtTreeWidget::selectedJID() const {
    QModelIndexList list = selectedIndexes();
    if (list.size() != 1) {
        return JID();
    }
    return jidFromIndex(list[0]);
}

void QtTreeWidget::setOnline(bool isOnline) {
    isOnline_ = isOnline;
}

bool QtTreeWidget::isOnline() const {
    return isOnline_;
}

}
