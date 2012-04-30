/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class TLSContext;

	class TLSContextFactory {
		public:
			virtual ~TLSContextFactory();

			virtual bool canCreate() const = 0;

			virtual TLSContext* createTLSContext() = 0;
	};
}
