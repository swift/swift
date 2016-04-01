/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {

class SWIFTEN_API StreamInitiationFileInfo : public Payload {
public:
    typedef std::shared_ptr<StreamInitiationFileInfo> ref;

public:
    StreamInitiationFileInfo(const std::string& name = "", const std::string& description = "", unsigned long long size = 0,
                 const std::string& hash = "", const boost::posix_time::ptime &date = boost::posix_time::ptime(), const std::string& algo="md5") :
        name(name), description(description), size(size), hash(hash), date(date), algo(algo), supportsRangeRequests(false), rangeOffset(0) {}

    void setName(const std::string& name) {
        this->name = name;;
    }

    const std::string& getName() const {
        return this->name;
    }

    void setDescription(const std::string& description) {
        this->description = description;
    }

    const std::string& getDescription() const {
        return this->description;
    }

    void setSize(const unsigned long long size) {
        this->size = size;
    }

    unsigned long long getSize() const {
        return this->size;
    }

    void setHash(const std::string& hash) {
        this->hash = hash;
    }

    const std::string& getHash() const {
        return this->hash;
    }

    void setDate(const boost::posix_time::ptime& date) {
        this->date = date;
    }

    const boost::posix_time::ptime& getDate() const {
        return this->date;
    }

    void setAlgo(const std::string& algo) {
        this->algo = algo;
    }

    const std::string& getAlgo() const {
        return this->algo;
    }

    void setSupportsRangeRequests(const bool supportsIt) {
        supportsRangeRequests = supportsIt;
    }

    bool getSupportsRangeRequests() const {
        return supportsRangeRequests;
    }

    void setRangeOffset(unsigned long long offset) {
        supportsRangeRequests = true;
        rangeOffset = offset;
    }

    unsigned long long getRangeOffset() const {
        return rangeOffset;
    }

private:
    std::string name;
    std::string description;
    unsigned long long size;
    std::string hash;
    boost::posix_time::ptime date;
    std::string algo;
    bool supportsRangeRequests;
    unsigned long long rangeOffset;
};

}
