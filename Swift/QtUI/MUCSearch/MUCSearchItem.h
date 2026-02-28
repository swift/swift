/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QVariant>

namespace Swift {

class MUCSearchServiceItem;

class MUCSearchItem {
        public:
            virtual ~MUCSearchItem() {}
            virtual void setParent(std::weak_ptr<MUCSearchServiceItem>) { }
            virtual QVariant data(int role) = 0;
    };
}
