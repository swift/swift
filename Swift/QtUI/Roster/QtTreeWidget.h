/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QModelIndex>
#include <QTreeView>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

#include <Swift/QtUI/Roster/RosterDelegate.h>
#include <Swift/QtUI/Roster/RosterModel.h>

namespace Swift {
class UIEventStream;
class SettingsProvider;

class QtTreeWidget : public QTreeView {
    Q_OBJECT
    public:
        enum MessageTarget {MessageDefaultJID, MessageDisplayJID};

        QtTreeWidget(UIEventStream* eventStream, SettingsProvider* settings, MessageTarget messageTarget, QWidget* parent = nullptr);
        ~QtTreeWidget();
        void show();
        QtTreeWidgetItem* getRoot();
        void setRosterModel(Roster* roster);
        Roster* getRoster() {return roster_;}
        void refreshTooltip();
        void setMessageTarget(MessageTarget messageTarget);
        JID jidFromIndex(const QModelIndex& index) const;
        JID selectedJID() const;
        void setOnline(bool isOnline);

    public:
        boost::signals2::signal<void (RosterItem*)> onSomethingSelectedChanged;

    private slots:
        void handleItemActivated(const QModelIndex&);
        void handleModelItemExpanded(const QModelIndex&, bool expanded);
        void handleExpanded(const QModelIndex&);
        void handleCollapsed(const QModelIndex&);
        void handleClicked(const QModelIndex&);
        void handleSettingChanged(const std::string& setting);
        void handleRefreshTooltip();

    protected:
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
        void dragMoveEvent(QDragMoveEvent* event);
        bool event(QEvent* event);
        QModelIndexList getSelectedIndexes() const;
        bool isOnline() const;

    private:
        void drawBranches(QPainter*, const QRect&, const QModelIndex&) const;

    protected slots:
        virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous);
    protected:
        UIEventStream* eventStream_;

    private:
        RosterModel* model_;
        Roster* roster_ = nullptr;
        RosterDelegate* delegate_;
        QtTreeWidgetItem* treeRoot_ = nullptr;
        SettingsProvider* settings_;
        bool tooltipShown_ = false;
        MessageTarget messageTarget_;
        bool isOnline_ = false;
};

}
