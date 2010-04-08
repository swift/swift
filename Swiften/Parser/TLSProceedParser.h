/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_TLSProceedParser_H
#define SWIFTEN_TLSProceedParser_H

#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/TLSProceed.h"

namespace Swift {
	class TLSProceedParser : public GenericElementParser<TLSProceed> {
		public:
			TLSProceedParser() : GenericElementParser<TLSProceed>() {}
	};
}

#endif
