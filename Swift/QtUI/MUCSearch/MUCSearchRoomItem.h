/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swift/QtUI/MUCSearch/MUCSearchItem.h>

namespace Swift {
    class MUCSearchServiceItem;
    class MUCSearchRoomItem : public MUCSearchItem {
        public:
            MUCSearchRoomItem(const QString& node);
            void setParent(std::weak_ptr<MUCSearchServiceItem> parent);
            std::shared_ptr<MUCSearchServiceItem> getParent();
            QVariant data(int role);
            QString getNode() const {return node_;}

        private:
            std::weak_ptr<MUCSearchServiceItem> parent_;
            QString node_;
    };
}
