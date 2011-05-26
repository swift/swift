/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Payload.h>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <string>

namespace Swift {

class StreamInitiationFileInfo : public Payload {
public:
	typedef boost::shared_ptr<StreamInitiationFileInfo> ref;
	
public:
	StreamInitiationFileInfo(const std::string& name = "", const std::string& description = "", int size = 0,
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
	
	void setSize(const boost::uintmax_t size) {
		this->size = size;
	}
	
	boost::uintmax_t getSize() const {
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
	
	void setRangeOffset(const int offset) {
		supportsRangeRequests = offset >= 0 ? true : false;
		rangeOffset = offset;
	}
	
	int getRangeOffset() const {
		return rangeOffset;
	}

private:
	std::string name;
	std::string description;
	boost::uintmax_t size;
	std::string hash;
	boost::posix_time::ptime date;
	std::string algo;
	bool supportsRangeRequests;
	boost::uintmax_t rangeOffset;
};

}
