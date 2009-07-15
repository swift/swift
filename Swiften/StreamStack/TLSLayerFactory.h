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
