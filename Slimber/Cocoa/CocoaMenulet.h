#pragma once

#include <Cocoa/Cocoa.h>

#include "Slimber/Menulet.h"
#include "Slimber/Cocoa/CocoaMenuletDelegate.h"

class CocoaMenulet : public Menulet {
	public:
		CocoaMenulet();
		~CocoaMenulet();		
	
	private:
		virtual void clear();
		virtual void addItem(const Swift::String& name, const Swift::String& icon);
		virtual void addSeparator();
		void setIcon(const Swift::String& icon);
		virtual void addAboutItem();
		virtual void addRestartItem();
		virtual void addExitItem();
	
	private:
		NSStatusItem* statusItem;
		NSMenu* menu;
		CocoaMenuletDelegate* delegate;
};
