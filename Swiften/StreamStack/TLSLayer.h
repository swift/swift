/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Base/boost_bsignals.h"

#include "Swiften/Base/ByteArray.h"
#include "Swiften/StreamStack/StreamLayer.h"

namespace Swift {
	class TLSContext;
	class TLSContextFactory;
	class PKCS12Certificate;

	class TLSLayer : public StreamLayer {
		public:
			TLSLayer(TLSContextFactory*);
			~TLSLayer();

			virtual void connect();
			virtual bool setClientCertificate(const PKCS12Certificate&);

			virtual void writeData(const ByteArray& data);
			virtual void handleDataRead(const ByteArray& data);

		public:
			boost::signal<void ()> onError;
			boost::signal<void ()> onConnected;

		private:
			TLSContext* context;
	};
}
