/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Client/CoreClient.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class ClientXMLTracer {
		public:
			ClientXMLTracer(CoreClient* client);

		private:
			static void printData(char direction, const SafeByteArray& data);
			static void printLine(char c);
	};
}
