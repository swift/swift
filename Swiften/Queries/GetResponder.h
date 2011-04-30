/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Queries/Responder.h>

namespace Swift {
	template<typename T>
	class GetResponder : public Responder<T> {
		public:
			GetResponder(IQRouter* router) : Responder<T>(router) {}

		private:
			virtual bool handleSetRequest(const JID&, const JID&, const std::string&, boost::shared_ptr<T>) { return false; }
	};
}
