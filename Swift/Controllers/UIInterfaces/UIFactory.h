/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/AdHocCommandWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/BlockListEditorWidgetFactory.h>
#include <Swift/Controllers/UIInterfaces/ChatListWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/ContactEditWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/EventWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/FdpFormSubmitWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/FileTransferListWidgetFactory.h>
#include <Swift/Controllers/UIInterfaces/HighlightEditorWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/HistoryWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/JoinMUCWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/LoginWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/MUCSearchWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/MainWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/ProfileWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/UserSearchWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/WhiteboardWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/XMLConsoleWidgetFactory.h>

namespace Swift {
    class UIFactory :
            public ChatListWindowFactory,
            public ChatWindowFactory,
            public HistoryWindowFactory,
            public EventWindowFactory,
            public LoginWindowFactory,
            public MainWindowFactory,
            public MUCSearchWindowFactory,
            public XMLConsoleWidgetFactory,
            public UserSearchWindowFactory,
            public JoinMUCWindowFactory,
            public ProfileWindowFactory,
            public ContactEditWindowFactory,
            public AdHocCommandWindowFactory,
            public FileTransferListWidgetFactory,
            public WhiteboardWindowFactory,
            public HighlightEditorWindowFactory,
            public BlockListEditorWidgetFactory,
            public FdpFormSubmitWindowFactory {
        public:
            virtual ~UIFactory() {}
    };
}
