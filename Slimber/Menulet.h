#pragma once

#include "Swiften/Base/String.h"

class Menulet {
	public:
		virtual ~Menulet();

		virtual void clear() = 0;
		virtual void addItem(const Swift::String& name, const Swift::String& icon = Swift::String()) = 0;
		virtual void addAboutItem() = 0;
		virtual void addExitItem() = 0;
		virtual void addSeparator() = 0;
		virtual void setIcon(const Swift::String&) = 0;
};
