/*
 * Copyright (c) 2014 Remko Tronçon and Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Forwarded.h>

namespace Swift {
	class SWIFTEN_API IsodeIQDelegation : public Payload {
		public:
			
			IsodeIQDelegation();
			
			virtual ~IsodeIQDelegation();

			boost::shared_ptr<Forwarded> getForward() const {
				return forward;
			}

			void setForward(boost::shared_ptr<Forwarded> value) {
				this->forward = value ;
			}


		private:
			boost::shared_ptr<Forwarded> forward;
	};
}
