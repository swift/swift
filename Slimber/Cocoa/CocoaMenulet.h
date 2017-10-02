/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Cocoa/Cocoa.h>

#include <Slimber/Menulet.h>

#include <SwifTools/Cocoa/CocoaAction.h>

class CocoaMenulet : public Menulet {
    public:
        CocoaMenulet();
        virtual ~CocoaMenulet();

    private:
        virtual void clear();
        virtual void addItem(const std::string& name, const std::string& icon);
        virtual void addSeparator();
        void setIcon(const std::string& icon);
        virtual void addAboutItem();
        virtual void addRestartItem();
        virtual void addExitItem();

    private:
        NSStatusItem* statusItem;
        NSMenu* menu;
        CocoaAction* restartAction;
};
