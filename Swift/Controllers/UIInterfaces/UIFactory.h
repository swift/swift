/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/ChatListWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/EventWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/LoginWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/MainWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/MUCSearchWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/JoinMUCWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/UserSearchWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/XMLConsoleWidgetFactory.h>
#include <Swift/Controllers/UIInterfaces/ProfileWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/ContactEditWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/AdHocCommandWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/FileTransferListWidgetFactory.h>

namespace Swift {
	class UIFactory : 
			public ChatListWindowFactory, 
			public ChatWindowFactory, 
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
			public FileTransferListWidgetFactory {
		public:
			virtual ~UIFactory() {}
	};
}
