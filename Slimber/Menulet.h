/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/signals2.hpp>

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

        boost::signals2::signal<void ()> onRestartClicked;
};
