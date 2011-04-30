/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swiften/Base/boost_bsignals.h>

class Menulet {
	public:
		virtual ~Menulet();

		virtual void clear() = 0;
		virtual void addItem(const std::string& name, const std::string& icon = std::string()) = 0;
		virtual void addAboutItem() = 0;
		virtual void addRestartItem() = 0;
		virtual void addExitItem() = 0;
		virtual void addSeparator() = 0;
		virtual void setIcon(const std::string&) = 0;

		boost::signal<void ()> onRestartClicked;
};
