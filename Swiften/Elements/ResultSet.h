/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API ResultSet : public Payload {
        public:
            virtual ~ResultSet();

            void setMaxItems(const boost::optional<int>& maxItems) { maxItems_ = maxItems; }
            const boost::optional<int>& getMaxItems() const { return maxItems_; }

            void setCount(const boost::optional<int>& count) { count_ = count; }
            const boost::optional<int>& getCount() const { return count_; }

            void setIndex(const boost::optional<int>& index) { index_ = index; }
            const boost::optional<int>& getIndex() const { return index_; }

            void setFirstIDIndex(const boost::optional<int>& firstIndex) { firstIndex_ = firstIndex; }
            const boost::optional<int>& getFirstIDIndex() const { return firstIndex_; }

            void setFirstID(const boost::optional<std::string>& firstID) { firstID_ = firstID; }
            const boost::optional<std::string>& getFirstID() const { return firstID_; }

            void setLastID(const boost::optional<std::string>& lastID) { lastID_ = lastID; }
            const boost::optional<std::string>& getLastID() const { return lastID_; }

            void setBefore(const boost::optional<std::string>& before) { before_ = before; }
            const boost::optional<std::string>& getBefore() const { return before_; }

            void setAfter(const boost::optional<std::string>& after) { after_ = after; }
            const boost::optional<std::string>& getAfter() const { return after_; }


        private:
            boost::optional<int> maxItems_;
            boost::optional<int> count_;
            boost::optional<int> index_;
            boost::optional<int> firstIndex_;
            boost::optional<std::string> firstID_;
            boost::optional<std::string> lastID_;
            boost::optional<std::string> before_;
            boost::optional<std::string> after_;
    };
}
