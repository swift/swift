/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/HashElement.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {

    /**
     * @brief This class represents the file info used in XEP-0234.
     */
    class SWIFTEN_API JingleFileTransferFileInfo : public Payload {
        typedef std::shared_ptr<JingleFileTransferFileInfo> ref;

        public:
            JingleFileTransferFileInfo(const std::string& name = "", const std::string& description = "", unsigned long long size = 0, const boost::posix_time::ptime &date = boost::posix_time::ptime()) :
                name_(name), description_(description), size_(size), date_(date), supportsRangeRequests_(false), rangeOffset_(0) {
            }

        public:
            typedef std::map<std::string, ByteArray> HashElementMap;

        public:
            void setName(const std::string& name) {
                name_ = name;;
            }

            const std::string& getName() const {
                return name_;
            }

            void setDescription(const std::string& description) {
                description_ = description;
            }

            const std::string& getDescription() const {
                return description_;
            }

            void setMediaType(const std::string& mediaType) {
                mediaType_ = mediaType;
            }

            const std::string& getMediaType() const {
                return mediaType_;
            }

            void setSize(const boost::uintmax_t size) {
                size_ = size;
            }

            boost::uintmax_t getSize() const {
                return size_;
            }

            void setDate(const boost::posix_time::ptime& date) {
                date_ = date;
            }

            const boost::posix_time::ptime& getDate() const {
                return date_;
            }

            void setSupportsRangeRequests(const bool supportsIt) {
                supportsRangeRequests_ = supportsIt;
            }

            bool getSupportsRangeRequests() const {
                return supportsRangeRequests_;
            }

            void setRangeOffset(const boost::uintmax_t offset) {
                supportsRangeRequests_ = true;
                rangeOffset_ = offset;
            }

            boost::uintmax_t getRangeOffset() const {
                return rangeOffset_;
            }

            void addHash(const HashElement& hash) {
                hashes_[hash.getAlgorithm()] = hash.getHashValue();
            }

            const std::map<std::string, ByteArray>& getHashes() const {
                return hashes_;
            }

            boost::optional<ByteArray> getHash(const std::string& algorithm) const {
                boost::optional<ByteArray> ret;
                if (hashes_.find(algorithm) != hashes_.end()) {
                    ret = boost::optional<ByteArray>(hashes_.find(algorithm)->second);
                }
                return ret;
            }

        private:
            std::string name_;
            std::string description_;
            std::string mediaType_;
            boost::uintmax_t size_;
            boost::posix_time::ptime date_;
            bool supportsRangeRequests_;
            boost::uintmax_t rangeOffset_;
            HashElementMap hashes_;
    };
}
