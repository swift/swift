/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Client/CoreClient.h>
#include <Swiften/Client/XMLBeautifier.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class ClientXMLTracer {
		public:
			ClientXMLTracer(CoreClient* client, bool bosh = false);
			~ClientXMLTracer();
		private:
			void printData(char direction, const SafeByteArray& data);
			void printLine(char c);

		private:
			XMLBeautifier *beautifier;
			bool bosh;
	};
}
