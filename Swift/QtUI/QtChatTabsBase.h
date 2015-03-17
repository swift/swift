/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

namespace Swift {

class QtTabbable;

class QtChatTabsBase {
	public:
		QtChatTabsBase();
		virtual ~QtChatTabsBase();

		virtual void addTab(QtTabbable* tab) = 0;
};

}
