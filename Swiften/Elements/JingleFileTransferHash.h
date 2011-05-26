/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>

#include <Swiften/Elements/JingleDescription.h>

namespace Swift {

class JingleFileTransferHash : public Payload {
public:
	typedef std::map<std::string, std::string> HashesMap;
public:
	typedef boost::shared_ptr<JingleFileTransferHash> ref;

	void setHash(const std::string& algo, const std::string& hash) {
		hashes[algo] = hash;
	}

	const HashesMap& getHashes() const {
		return hashes;
	}

private:
	HashesMap hashes;
};
	
}
