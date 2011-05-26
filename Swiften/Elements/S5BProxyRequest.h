/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/HostAddressPort.h>

namespace Swift {

class S5BProxyRequest : public Payload {
public:
	typedef boost::shared_ptr<S5BProxyRequest> ref;

public:
	struct StreamHost {
		HostAddressPort addressPort;
		JID jid;
	};

public:
	const boost::optional<StreamHost>& getStreamHost() const {
		return streamHost;
	}

	void setStreamHost(const StreamHost& streamHost) {
		this->streamHost = boost::optional<StreamHost>(streamHost);
	}

	const std::string& getSID() const {
		return sid;
	}

	void setSID(const std::string& sid) {
		this->sid = sid;
	}

	const boost::optional<JID>& getActivate() const {
		return activate;
	}

	void setActivate(const JID& activate) {
		this->activate = activate;
	}

private:
	boost::optional<StreamHost> streamHost;

	std::string sid;
	boost::optional<JID> activate;
};

}
