/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/QtUI/Roster/QtTreeWidget.h>

namespace Swift {
class QtUIPreferences;

class QtRosterWidget : public QtTreeWidget {
    Q_OBJECT
    public:
        QtRosterWidget(UIEventStream* eventStream, SettingsProvider* settings, QWidget* parent = nullptr);
        virtual ~QtRosterWidget();
    public slots:
        void handleEditUserActionTriggered(bool checked);
    protected:
        void contextMenuEvent(QContextMenuEvent* event);
    private:
        void renameGroup(GroupRosterItem* group);
};

}
