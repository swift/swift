/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {

class PlatformNATTraversalWorker;

class PlatformNATTraversalRequest : public boost::enable_shared_from_this<PlatformNATTraversalRequest> {
public:
	typedef boost::shared_ptr<PlatformNATTraversalRequest> ref;

public:
	PlatformNATTraversalRequest(PlatformNATTraversalWorker* worker);
	virtual ~PlatformNATTraversalRequest();

	virtual void doRun();
	virtual void runBlocking() = 0;

private:
	PlatformNATTraversalWorker* worker;
};

}
