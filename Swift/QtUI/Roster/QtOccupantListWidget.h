/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

#include <Swift/QtUI/Roster/QtTreeWidget.h>

namespace Swift {
class SettingsProvider;

class QtOccupantListWidget : public QtTreeWidget {
    Q_OBJECT
    public:
        QtOccupantListWidget(UIEventStream* eventStream, SettingsProvider* settings, MessageTarget privateMessageTarget, QWidget* parent = nullptr);
        virtual ~QtOccupantListWidget();
        void setAvailableOccupantActions(const std::vector<ChatWindow::OccupantAction>& actions);
        boost::signals2::signal<void (ChatWindow::OccupantAction, ContactRosterItem*)> onOccupantActionSelected;
    protected:
        void contextMenuEvent(QContextMenuEvent* event);
    private:
        std::vector<ChatWindow::OccupantAction> availableOccupantActions_;
};

}

