/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

namespace Swift {
	class TLSLayer;

	class TLSLayerFactory {
		public:
			virtual ~TLSLayerFactory();
			virtual bool canCreate() const = 0;

			virtual boost::shared_ptr<TLSLayer> createTLSLayer() = 0;
	};
}
