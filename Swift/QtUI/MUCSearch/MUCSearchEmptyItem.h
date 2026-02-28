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

    class MUCSearchEmptyItem : public MUCSearchItem {
        public:
            MUCSearchEmptyItem();

            void setParent(std::weak_ptr<MUCSearchServiceItem> parent);
            std::shared_ptr<MUCSearchServiceItem> getParent();

            QVariant data(int role);

        private:
            std::weak_ptr<MUCSearchServiceItem> parent_;
    };
}
