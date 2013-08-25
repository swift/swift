/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <string>



namespace Swift {
	class SWIFTEN_API PubSubEventRedirect : public Payload {
		public:
			
			PubSubEventRedirect();
			
			virtual ~PubSubEventRedirect();

			const std::string& getURI() const {
				return uri;
			}

			void setURI(const std::string& value) {
				this->uri = value ;
			}


		private:
			std::string uri;
	};
}
