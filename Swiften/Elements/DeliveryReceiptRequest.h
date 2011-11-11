/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the BSD license.
 * See http://www.opensource.org/licenses/bsd-license.php for more information.
 */

#pragma once

#include <Swiften/Elements/Payload.h>

namespace Swift {

class DeliveryReceiptRequest : public Payload {
	public:
		typedef boost::shared_ptr<DeliveryReceiptRequest> ref;

	public:
		DeliveryReceiptRequest() {}
};

}
