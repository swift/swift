/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swift/Controllers/UIInterfaces/EventWindow.h>

#include <Swift/QtUI/EventViewer/EventDelegate.h>
#include <Swift/QtUI/EventViewer/EventModel.h>
#include <Swift/QtUI/EventViewer/EventView.h>

class QPushButton;
class QTreeView;

namespace Swift {
    class UIEventStream;

    class QtEventWindow : public QWidget, public EventWindow {
        Q_OBJECT
        public:
            QtEventWindow(UIEventStream* eventStream);
            ~QtEventWindow();
            void addEvent(std::shared_ptr<StanzaEvent> event, bool active);
            void removeEvent(std::shared_ptr<StanzaEvent> event);

        signals:
            void onNewEventCountUpdated(int count);

        private slots:
            void handleItemActivated(const QModelIndex& item);
            void handleItemClicked(const QModelIndex& item);
            void handleReadClicked();

        private:
            EventModel* model_;
            EventDelegate* delegate_;
            UIEventStream* eventStream_;
            QTreeView* view_;
            QPushButton* readButton_;
    };

}

