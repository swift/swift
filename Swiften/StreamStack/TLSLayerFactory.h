#ifndef SWIFTEN_TLSLayerFactory_H
#define SWIFTEN_TLSLayerFactory_H

namespace Swift {
	class TLSLayer;

	class TLSLayerFactory {
		public:
			virtual ~TLSLayerFactory();
			virtual bool canCreate() const = 0;

			virtual TLSLayer* createTLSLayer() = 0;
	};
}

#endif
