/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/MUC/MUC.h>

namespace Swift {
	class IQRouter;
	class StanzaChannel;
	class DirectedPresenceSender;
	class MUCRegistry;

	class MUCManager {
		public:
			MUCManager(StanzaChannel* stanzaChannel, IQRouter* iqRouter, DirectedPresenceSender* presenceSender, MUCRegistry* mucRegistry);

			MUC::ref createMUC(const JID&);

		private:
			StanzaChannel* stanzaChannel;
			IQRouter* iqRouter;
			DirectedPresenceSender* presenceSender;
			MUCRegistry* mucRegistry;
	};
}
