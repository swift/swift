/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/TLSProceed.h>

namespace Swift {
	class TLSProceedParser : public GenericElementParser<TLSProceed> {
		public:
			TLSProceedParser() : GenericElementParser<TLSProceed>() {}
	};
}
